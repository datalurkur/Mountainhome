# Enumerate the locations a tile can be accessed from for different job scenarios
# These are offsets from the @task[:position].
AT = [[0, 0, 0]]
ADJACENT =
    # just the same z-level or above in cardinal directions
    [
    [1, 0, 0], [0, 1, 0], [-1, 0, 0], [0, -1, 0],
    [1, 0, 1], [0, 1, 1], [-1, 0, 1], [0, -1, 1]
    ]

# Handle both @path movement and @task actions if either is defined.
class Actor < MHActor
    def update(elapsed)
        # Move there first.
        if @path
            move
        # Then run the task until it reports itself complete.
        elsif @task
            $logger.info "#{self.to_s} is acting."
            status = @task.include?(:action) ? self.send(@task[:action], elapsed, @task[:params]) : :task_finished
            if status == :task_failed or status == :task_finished
                if @jobmanager
                    # Report task status to JobManager and request a new task.
                    task_done(status)
                else
                    # Nothing remains to be done; clear the @task.
                    @task = nil
                end
            end
        # Lastly, request a job.
        elsif @jobmanager
            request_task
        end
    end
end

# Create a @path variable containing the future path.
module Movement
    def move
        if @path && @path.size > 0
            # Note: We are relying on something else to determine whether or not the path has been interrupted,
            #  operating under the assumption that eventually some external thing will be responsible for doing
            #  the actual moving of entities, with entities dictating where they intend to go (so that things like
            #  physics can be abstracted outside of the movers)
            # task_done(:no_path_to_task) if respond_to?(:task_done)
            #  Normally this would be called in the event a path is blocked

            next_step = @path.shift
            $logger.info "#{self.to_s} moving to #{next_step.inspect}."
            set_position(*next_step)
        elsif @path.empty?
            $logger.info "End of path reached for #{self.to_s}."
            @path = nil
        end
    end

    def set_relative_start(world, start)
        world.pathfinder.set_start_position(*start)
    end

    def find_relative_path(world, relative_location, dest)
        $logger.info "Finding a path for #{self.inspect} at #{self.position}"
        access_locations = relative_location.to_s.upcase.constantize.collect { |rel| rel.piecewise_add(dest) }
        access_locations.reject! { |loc| world.out_of_bounds?(*loc) }
        path = []
        world.pathfinder.closest_path_to(access_locations) do |path_node|
            path << path_node
        end
        $logger.info "Found path: #{path.inspect}"
        [path.size != 0, path]
    end

    def find_path_to(world, relative_location, dest)
        set_relative_start(world, position)
        find_relative_path(world, relative_location, dest)
    end
end

module Worker
    # Store some extra information for workers.
    attr_accessor :task, :jobmanager, :skills

    def task_done(status)
        if @jobmanager
            next_task = @jobmanager.report_task(@task, status)
            # If this task requires a followup task, set that task active.
            set_task(next_task)
        end
    end

    # Get new task, if applicable.
    def request_task
        if @jobmanager
            set_task(@jobmanager.request_task(self))
        else
            @task = nil
        end
    end

    # Set the current task and create a path to it.
    def set_task(task)
        # fail current task if the new task is interrupting
        if @task and task and task[:interrupt]
            task_done(:task_failed)
        end

        @task = task
        return if task.nil?

        $logger.info "Received task #{@task.to_s}"
        if self.class.include?(Movement)
            $logger.info "Pathing to #{@task[:position]} with approach #{@task[:relative_location].to_s.upcase}"
            path_found, path = find_path_to(@task[:params][:world], @task[:relative_location], @task[:position])
            path_found ? @path = path : task_done(:no_path_to_task)
        end
    end

    def mine(elapsed, params = {})
        $logger.info "Mining tile at #{@task[:position].inspect}"
        params[:world].terrain.set_tile_empty(*@task[:position])
        :task_finished
    end
end

class JobManager
    # Register a new Worker with JobManager.
    def register_worker(worker)
        return unless worker.class.include?(Worker)
        @workers << worker
        worker.jobmanager = self

        # Eventually this will happen in a more complex way, but it's hardcoded for now.
        # Note that setting skills to [nil] will let that worker work on all tasks.
        worker.skills = [:move, :mine]
    end

    def initialize(world)
        @world = world
        @tasks_to_assign = []
        @jobs = []
        @workers = []
    end

    def add_job(klass, position, params = {})
        params[:world] = @world
        job = klass.new(position, params)
        @jobs << job
    end

    # Search the current jobs for tasks that are doable with the given skillset.
    # Return the task to the Worker.
    def request_task(worker)
        @jobs.each do |job|
            if task = job.find_task_for_worker(worker)
                $logger.info "Assigning task #{task}"
                return task
            end
        end
        nil
    end

    # Behave appropriately at task failure or success.
    def report_task(task, status)
        $logger.info "Task #{task} #{status}"
        # Notify the job that the task is finished.
        # :task_failed will put the task back in the job pool.
        # :no_path_to_task will also generally put the task back in the job pool.
        # :task_finished will return the next task for the same worker to
        # complete, if applicable.
        @jobs.each do |job|
            next_task = job.send(status, task)
            @jobs.delete(job) if job.finished?
            # Give the same worker the next_task returned.
            return next_task if next_task
        end
        nil
    end
end

# A job stores a series of Tasks.
# A Task is a movement followed by an action, or both.

class Job
    # The tasks this job must farm out to workers.
    attr_accessor :avail_tasks, :next_task, :finished

    def finished?; @finished; end

    def initialize(position, params = {})
        $logger.info("new #{self.to_s}")
        # Can only instantiate Job subclasses.
        throw NotImplementedError, "Can't instantiate the generic Job" if self.class == Job

        @position = position
        @params = params

        # initial_tasks is defined differently for every Job class.
        @avail_tasks = initial_tasks
        @curr_tasks = []
        @done_tasks = []
        @finished = false
    end

    def find_task_for_worker(worker)
        $logger.info("looking for tasks for #{worker.skills.inspect} in #{self.inspect}")
        worker.set_relative_start(@params[:world], worker.position)

        new_task = @avail_tasks.find do |task|
            # Matches the skill required for the task.
            req_skills = worker.skills.include?(task[:skill])

            # Worker can path to the task.
            req_location = worker.find_relative_path(@params[:world], task[:relative_location], task[:position])

            # And either no prerequisites defined or all the prerequisites are done.
            # Prerequisites are assumed to be local to the Job.
            prereqs_met = (task[:prerequisites].nil? or (task[:prerequisites] - @done_tasks).size == 0)

            req_skills and req_location and prereqs_met
        end

        if new_task
            task_started(new_task)
            new_task
        else
            nil
        end
    end

    def task_started(task)
        @avail_tasks.delete(task)
        @curr_tasks << task
        nil
    end

    def no_path_to_task(task)
        task_failed(task)
    end

    # Put the task back in the available pool.
    def task_failed(task)
        if @curr_tasks.include?(task)
            @curr_tasks.delete(task)
            @avail_tasks << task
        end
        nil
    end

    # Remove the task from the tasklist.
    def task_finished(task)
        # look for it in the list of tasks
        if @curr_tasks.include?(task)
            @curr_tasks.delete(task)
            @done_tasks << task

            @finished = (@avail_tasks.empty? and @curr_tasks.empty?)
            # Force the next task for the worker; e.g. the same worker that picked up an item must now drop it somewhere.
            return task[:next_task] if task[:next_task]
        end
        nil
    end

    # Basic default task. Overridden using default_task.merge(hash).
    def default_task
        {:skill => :move, :position => @position, :relative_location => :at, :params => @params}
    end

    def to_s; self.class.to_s; end
end

class Move < Job
    def initial_tasks
        [default_task]
    end
end

class Mine < Job
    def initial_tasks
        mine_task = default_task.merge({:action => :mine, :skill => :mine, :relative_location => :adjacent})
        [mine_task]
    end
end

# All code following is untested!

# This is probably what forced task continuation will look like.
class MoveItem < Job
    def initial_tasks
        item_dropoff = default_task.merge({:action => :drop})
        item_pickup  = default_task.merge({:action => :pickup, :position => @params[:item].position, :next_task => item_dropoff})
        # FIXME: How to include item_dropoff in task list yet not get it picked up by a second dwarf?
        [item_pickup, item_dropoff]
    end
end

# This is what prerequisite tasks will look like.
class ConstructBuilding < Job
    def initial_tasks
        tasks = []
        prerequisites = []
        @params[:items].each { |item|
            # It bothers me that this essentially duplicates the MoveItem task creation.
            item_dropoff = default_task.merge({:action => :drop})
            item_pickup  = default_task.merge({:action => :pickup, :position => item.position, :next_task => item_dropoff})
            tasks += [item_pickup, item_dropoff]
            prerequisites << item_dropoff
        }
        construct = default_task.merge({:action => :build, :skill => @params[:skill], :prerequisites => prerequisites})
        tasks << construct
        tasks
    end
end
