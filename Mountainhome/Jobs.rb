require 'Path'

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
        if @path
            if @path.blocked?
                check_path_blocked
            elsif @path.end_of_path?
                $logger.info "End of path reached for #{self.to_s}."
                clear_path
            else
                next_step = @path.next_step
                $logger.info "#{self.to_s} moving to #{next_step.inspect}."
                set_position(*next_step)
            end
        end
    end

    # Path is blocked; this worker can't path there, so hand off the task to another worker.
    def check_path_blocked(path)
        if path.blocked?
            $logger.info "Path blocked from #{self.to_s} to position."
            clear_path
            task_done(:no_path_to_task) if respond_to?(:task_done)
        end
    end

    def clear_path
        @path = nil
    end

    def find_path(world, relative_location, dest)
        # convert e.g. :at => AT
        access_locations = relative_location.to_s.upcase.constantize

        # Take an array of locations, trying to find a path to each one,
        #  using the first successful query as the destination.
        path = nil
        path_found = access_locations.find do |offset|
            # Adjust destination by offset.
            real_dest = dest.piecewise_add(offset)
            $logger.info "real_dest is #{real_dest.inspect}"

            # The position isn't out of bounds.
            !world.out_of_bounds?(real_dest) and
            # The position is empty.
            world.terrain.tile_empty?(*real_dest) and
            # The path to this position isn't blocked.
            !(path = Path.new(world, position, real_dest)).blocked?
        end
        path_found = true if path_found
        return [path_found, path]
    end

    def can_path_to?(world, relative_location, dest)
        path_found, path = find_path(world, relative_location, dest)
        path_found
    end

    def create_path(world, relative_location, dest)
        path_found, path = find_path(world, relative_location, dest)
        if path_found
            $logger.info "Found path from #{@location} to #{dest} with strategy :#{relative_location}"
            @path = path
        end
        return path_found
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
            path_found = create_path(@task[:params][:world], @task[:relative_location], @task[:position])
            task_done(:no_path_to_task) unless path_found
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
        if new_task = @avail_tasks.find do |task|
                $logger.info "skills include #{worker.skills.include?(task[:skill])}"
                $logger.info "prereq tasks #{task[:prerequisites].nil?}"
                $logger.info "prereq tasks2 #{task[:prerequisites] and
                @done_tasks.collect { |task| task[:prerequisites].include?(task) }.size == task[:prerequisites].size
                }"
                # Matches the skill required for the task.
                worker.skills.include?(task[:skill]) and
                # Worker can path to the task.
                worker.can_path_to?(@params[:world], task[:relative_location], task[:position]) and
                # And either no prerequisites defined or all the prerequisites are done.
                # Prerequisites are assumed to be local to the Job.
                (task[:prerequisites].nil? or
                (task[:prerequisites] and
                task[:prerequisites].select { |task| @done_tasks.include?(task) }.size == task[:prerequisites].size
                ))
            end
            task_started(new_task)
            return new_task
        end
        nil
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