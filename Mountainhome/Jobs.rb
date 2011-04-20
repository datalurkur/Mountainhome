# This scheduler will assign tasks to workers, with the relative probability of a
# worker being assigned a given type of task based on the number of those tasks
# activated among the workers. The single craftworker is more likely to
# craft than to haul, which everydwarf could do.
class Scheduler
    attr_reader :workers, :tasks
    def initialize(workers=[], tasks=[])
        @workers = workers.select { |w| w.class.include?(Worker) }
        @busy_workers = []
        @free_workers = []

        @tasks_to_assign = tasks
        @assigned_tasks = {}
        # Increment counts of workers for each task.
#        @num_task_workers = {}
#        @workers.each { |w| w.activated_tasks.each { |at| @num_task_workers[at] += 1 } }
    end

    def add_worker(worker)
        @workers << worker
#        worker.activated_tasks.each { |at| @num_task_workers[at] += 1 }
    end

    def remove_worker(worker)
#        worker.activated_tasks.each { |at| @num_task_workers[at] -= 1 }
        @worker.delete(worker)
    end

    def add_task(task)
        $logger.info "Adding new #{task.class} [#{task.position}] to scheduler"
        @tasks_to_assign << task
        find_worker_for(task)
    end

    def remove_task(task, finished)
        worker = @assigned_tasks[task]
        $logger.info "Removing task #{task} for #{worker}"
        worker.task = nil
        worker_is_free(worker)
        @assigned_tasks.delete(task)
        @tasks_to_assign << task unless finished
    end

    def assign(worker, task)
        $logger.info "Assigning task #{task} to #{worker}"
        @tasks_to_assign.delete(task)
        @assigned_tasks[task] = worker
        worker.task = task
        worker_is_busy(worker)
    end

    def find_task_for(worker)
        if @busy_workers.include?(worker)
            $logger.warn "Trying to assign task to busy worker #{worker}"
            return nil
        end
        @tasks_to_assign.each do |task|
            # Just assign the first doable task found for now.
            if worker.can_do_task?(task)
                assign(worker, task)
                return task
            end
            # Eventually, we want to assign probabilities based on the number
            # of workers of each type of those tasks.
            #@num_task_workers[task.class.to_s.downcase.to_sym]
        end
        return nil
    end

    def find_worker_for(task)
        $logger.info "Looking for worker for #{task}"
        @free_workers.each do |worker|
            # Just assign the first worker found for now.
            if worker.can_do_task?(task)
                assign(worker, task)
                return worker
            end
        end
        return nil
    end

    def worker_is_busy(worker)
        $logger.info "Worker #{worker} is busy"
        @free_workers.delete(worker)
        @busy_workers << worker
    end

    def worker_is_free(worker)
        $logger.info "Worker #{worker} is free"
        @busy_workers.delete(worker)
        @free_workers << worker
    end
end

# Receives changes from the workers, manages jobs
# and sends job tasks and task changes to the scheduler.
class JobManager
    attr_reader :world
    def initialize(world, workers=[], jobs=[])
        @world = world
        @scheduler = Scheduler.new(workers)
        @jobs = jobs
    end

    def add_job(klass, position)
        job = klass.new(position)
        @jobs << job
        $logger.info "#{job.tasks}"
        job.tasks.each { |t| @scheduler.add_task(t) }
    end

    def add_worker(worker)
        @scheduler.add_worker(worker)
        worker.jobmanager = self
    end

    def remove_task(task, finished=false)
        @scheduler.remove_task(task, finished)
        if finished
            # Jobs maintain their own list of tasks completed.
            task.job.report_task_done(task)
            # Jobs may produce new tasks when current tasks are finished.
            if task.job.new_tasks
                task.job.new_tasks.each { |t| @scheduler.add_task(t) }
            # Clean up the job if it's finished.
            elsif task.job.completed?
                @jobs.delete(task.job)
            end
        end
    end

    delegate_to :scheduler, :find_task_for
end

# Enumerate the locations a tile can be accessed from for different job scenarios
# AT: Workers default to moving to the task's position to perform the task.
AT = [[0, 0, 0]]
# ADJACENT: Move next to the tile to be mined.
ADJACENT =
    # just the same z-level or above in cardinal directions
    [
    [1, 0, 0], [0, 1, 0], [-1, 0, 0], [0, -1, 0],
    [1, 0, 1], [0, 1, 1], [-1, 0, 1], [0, -1, 1]
    ]

class Task
    extend RecordChildren
    attr_reader :job, :position, :parameters

    def initialize(job, position, parameters={})
        @job = job
        @position = position
        @parameters = parameters
    end

    def self.relative_location_strategy() AT; end

    def relative_locations
        self.class.relative_location_strategy.collect { |loc| loc.piecewise_add(@position) }
    end
end

class MoveTask < Task; end

class MineTask < Task
    def self.relative_location_strategy() ADJACENT; end
    def self.callback() :mine; end
end

class Job
    attr_writer :blocked_workers
    def blocked_workers() @blocked_workers ||= []; end

    def initialize(position)
        @position = position
        @tasks = self.tasks
    end

    # Initial tasks for this job.
    def tasks
        @tasks ||= [MoveTask.new(self, @position)]
    end

    # Jobs may override this with their own logic in the event that completed
    # tasks are prerequisites for other tasks that shouldn't be added to the
    # scheduler immediately.
    def new_tasks
        nil
    end

    def report_task_done(task)
        @tasks.delete(task)
    end

    def completed?
        @tasks.empty?
    end
end

class Move < Job; Task = MoveTask; end

class Mine < Job
    Task = MineTask
    def tasks
        @tasks ||= [MineTask.new(self, @position)]
    end
end

# Should only ever be assigned to Actors, or things with a world reference.
module Movement
    def can_path_to(task)
        @world.pathfinder.set_start_position(*self.position)
        path = []
#        $logger.info "calling first_path_to with args #{access_locations_for(task).inspect}"
        @world.pathfinder.first_path_to(access_locations_for(task)) do |path_node|
            # We've found a path, and we're starting to receive it. We're done here.
            $logger.info "#{self.to_s} (#{self.position}) CAN path to #{task} (#{task.position})"
            return true
        end
        return false
    end

    def path_to_task(task)
        return nil if task.nil?
        $logger.info "setting start position #{self.position.inspect}"
        @world.pathfinder.set_start_position(*self.position)
        path = []
#        $logger.info "calling closest_path_to with args #{access_locations_for(task)}"
        @world.pathfinder.closest_path_to(access_locations_for(task)) do |path_node|
            path << path_node
        end
        path
    end

    def access_locations_for(task)
        task.relative_locations.reject { |loc| @world.out_of_bounds?(*loc) }
    end
end

# Should only ever be assigned to Actors, or things with a world reference.
module Worker
    # Assume workers can move...
    include Movement

    # Activated tasks being a subset of the tasks a worker is capable of
    attr_writer :capable_tasks, :activated_tasks, :jobmanager
    attr_reader :task

    def move
        if @path
            # Note: We are relying on something else to determine whether or not the path has been interrupted,
            #  operating under the assumption that eventually some external thing will be responsible for doing
            #  the actual moving of entities, with entities dictating where they intend to go (so that things like
            #  physics can be abstracted outside of the movers)
            next_step = @path.shift
            $logger.info "#{self.to_s} moving to #{next_step.inspect}."
            set_position(*next_step)
            if @path.empty?
                $logger.info "End of path reached for #{self.to_s}."
                @path = nil
            end
        end
    end

    # Default to capable of all tasks, and all tasks activated.
    def capable_tasks() @capable_tasks ||= Task.children; end
    def activated_tasks() @activated_tasks ||= Task.children; end

    def can_do_task?(task)
        result = (activated_tasks.include?(task.class) && can_path_to(task) && !task.job.blocked_workers.include?(self))
        $logger.info "can_do_task?(#{task}) parts: #{activated_tasks.include?(task.class)} && #{can_path_to(task)} && #{!task.job.blocked_workers.include?(self)}"
        result
    end

    def task=(task)
        @task = task
        @path = path_to_task(task)
    end

    def mine(task, elapsed, params = {})
        $logger.info "Mining tile at #{task.position}"
        @jobmanager.world.terrain.set_tile(*task.position, nil)
        # Task is finished.
        true
    end
end

# Handle both @path movement and @task actions if either is defined.
class Actor < MHActor
    # Eventually actors won't update themselves, but will all have managers doing that.
    def update(elapsed)
        # Short-circuit plants and other boring Actors that do nothing.
        return unless self.class.include?(Movement) || self.class.include?(Worker)

        @sleep = (@sleep? @sleep - elapsed : 0)
        # Do nothing if still sleeping.
        return unless @sleep <= 0

        # Move there first.
        if @path
            $logger.info "#{self.to_s} is moving."
            move
        # Then run the task until it reports itself complete.
        elsif self.class.include?(Worker)
            if @task
                $logger.info "#{self.to_s} is acting."
                if @task.class.respond_to?(:callback)
                    finished = self.send(@task.class.callback, @task, elapsed, task.parameters)
                    @jobmanager.remove_task(@task, finished)
                else
                    @jobmanager.remove_task(@task, true)
                end
            else
                # Request a task
                @jobmanager.find_task_for(self)
                # And wait a quarter of a second before looking again.
                @sleep = 250
            end
        end
    end
end

=begin
# All code following is untested! This also follows the old Job model, which no longer exists.

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
=end
