require 'Path'

# Enumerate the locations a tile can be accessed from for different job scenarios
# These are offsets from the @position.
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
            status = @task.respond_to?(:action) ? @task.action(self, elapsed) : nil
            if status == :failed or status == :finished
                unless @jobmanager.nil?
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
                $logger.info "Path blocked from #{self.to_s} to position."
                @path = nil
            elsif @path.end_of_path?
                $logger.info "End of path reached for #{self.to_s}."
                @path = nil
            else
                next_step = @path.next_step
                $logger.info "#{self.to_s} moving to #{next_step.inspect}."
                set_position(*next_step)
            end
        end
    end

    def create_path(move_to, dest)
        # convert e.g. :at => AT
        access_locations = @task.move_to.to_s.upcase.constantize

        # Take an array of locations, trying to find a path to each one,
        #  using the first successful query as the destination.
        path_found = access_locations.find do |offset|
            # Adjust destination by offset.
            real_dest = dest.piecewise_add(offset)
            $logger.info "real_dest is #{real_dest.inspect}"

            # The position isn't out of bounds.
            !self.world.out_of_bounds?(real_dest) and
            # The position is empty.
            self.world.terrain.tile_empty?(*real_dest) and
            # The path to this position isn't blocked.
            !(@path = Path.new(self.world, position, real_dest)).blocked?
        end
        $logger.info "Found path to #{path_found.piecewise_add(dest)}" if path_found
    end
end

module Worker
    # Store some extra information for workers.
    attr_accessor :task, :jobmanager

    def task_done(status)
        if @jobmanager
            @jobmanager.report_task(@task, status)
            request_task
        end
    end

    # Get new task, if applicable.
    def request_task
        if @jobmanager
            set_task(@jobmanager.request_task)
        else
            @task = nil
        end
    end

    # Set the current task and create a path to it.
    def set_task(task)
        @task = task
        return if task.nil?

        # fail current task if the new task is interrupting
        if @task && task.interrupt?
            self.task_failed
        end

        $logger.info "Received task #{@task.to_s}"
        if self.class.include?(Movement)
            $logger.info "Pathing to #{@task.position} with approach #{@task.move_to}"
            create_path(@task.move_to, @task.position)
        end
    end
end

class JobManager
    # Register a new Worker with JobManager.
    def register_worker(worker)
        return unless worker.class.include?(Worker)

        worker.jobmanager = self

        # Add worker to job's lists of capable workers.
        # For now, sets every worker capable of every job.
        # There can eventually be logic here determining jobs_capable/jobs_active etc.
        # (worker.jobs_capable == :all ? self.class.jobs : worker.jobs_capable)
        self.class.jobs.each { |job|
            job.register_worker(worker)
        }
        @workers << worker
    end

    # JobManager stores all types of Job subclasses.
    def self.jobs; @jobs ||= []; end
    def self.jobs=(jobs); @jobs ||= jobs; end

    def initialize(world)
        @world = world

        @time = 0
        @workers = []
        @tasks_to_assign = []
    end

    def add_job(klass, position, *opts)
        job = klass.new(position, *opts)
        job.tasks.each { |task| @tasks_to_assign << task }
        $logger.info "tasks to assign: #{@tasks_to_assign}"
    end

    # Behave appropriately at task failure or success.
    def report_task(task, status)
        $logger.info "Task #{task} #{status}"
        case status
        when :failed
            # Put the task back in the pool if the worker failed to complete it.
            @tasks_to_assign << task
        when :finished
            # Notify the job that the task is finished.
            # report_task will return the next task for the same worker to
            # complete, if applicable.
            next_task = task.job.task_finished(task)
            unless next_task.nil?
                # The same worker will immediately ask for a task. Be sure to return this one.
                @tasks_to_assign.insert(0, next_task)
            end
        end
    end

    # Workers call this. Just return first task in the queue for now.
    # This will become more intelligent in the future.
    def request_task
        $logger.info "Assigning task #{@tasks_to_assign[0].inspect}" unless @tasks_to_assign.empty?
        @tasks_to_assign.shift
    end
end

# A job stores a series of Tasks.
# A Task is a movement, an action, or both.

class Job
    # A basic default task.
    class Task
        def self.move_to
            @move_to || :at
        end

        attr_accessor :position
        attr_reader :job

        # Unless the action is overridden, a no-op task.
        def action(worker, elapsed); :finished; end

        def initialize(position, job)
            $logger.info("new #{self.to_s}")
            @position = position
            @job = job
        end

        def interrupt?; false; end
        def move_to; self.class.move_to; end
        def to_s; self.class.to_s; end
    end

    # The tasks this job must farm out to workers.
    attr_accessor :tasks

    # Inform JobManager about kinds of jobs.
    def inherited(klass)
        $logger.info "Telling JobManager about #{klass.to_s}"
        JobManager.jobs << klass
    end

    def register_worker(worker)
        @capable_workers ||= []
        @capable_workers << worker
    end

    def unregister_worker(worker)
        @capable_workers ||= []
        @capable_workers.delete(worker)
    end

    def initialize(position, *opts)
        $logger.info("new #{self.to_s}")
        # Can only instantiate Job subclasses.
        throw NotImplementedError, "Can't instantiate the generic Job" if self.class == Job

        @position = position
        @opts = opts
    end

    # Remove the task from the tasklist.
    def task_finished(task)
        # look for it in the list of tasks
        @tasks.delete(task)
        nil
    end

    def to_s; self.class.to_s; end
end

class Move < Job
    # Necessary to distinguish a Move::Task from a generic Job::Task.
    class Task < Job::Task; end
    def tasks
        @tasks = @tasks || [Move::Task.new(@position, self)]
    end
end

class Mine < Job
    class Task < Job::Task
        def self.move_to; :adjacent; end

        def action(actor, elapsed)
            $logger.info "Mining tile at #{@position.inspect}"
            actor.world.terrain.set_tile_empty(*@position)
            :finished
        end
    end

    def tasks
        @tasks = @tasks || [Mine::Task.new(@position, self)]
    end
end