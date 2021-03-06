class Scheduler
    attr_reader :workers, :tasks_to_assign, :free_workers
    def initialize(workers=[], tasks=[])
        @workers = []
        workers.each do |worker|
            add_worker(worker) if worker.class.include?(Worker)
        end
        @busy_workers = []
        @free_workers = []

        @tasks_to_assign = tasks
        @assigned_tasks = {}
    end

    def add_worker(worker) @workers << worker; @free_workers << worker; end
    def remove_worker(worker) @workers.delete(worker); @busy_workers.delete(worker); @free_workers.delete(worker); end

    def add_task(task)
        $logger.info "Adding new #{task.class} #{task.position} to Scheduler"
        @tasks_to_assign << task
    end

    def remove_task(task)
        if task.finished && task.incomplete
            $logger.warn "Task #{task} by worker #{worker} both finished and incomplete"
        end
        worker = @assigned_tasks[task]
        unassign(worker, task)

        # Task isn't done; put the task back in the available task pool.
        if task.incomplete
            task.incomplete = false
            add_task(task)
        end
    end

    # Subclasses should either define find_task_for or redefine check_tasks.
    def check_tasks
        if !(@free_workers.empty? || @tasks_to_assign.empty?)
            @free_workers.each { |worker| find_task_for(worker) }
        end
    end

    def assign(worker, task)
        $logger.info "Assigning task #{task} to #{worker}"
        @tasks_to_assign.delete(task)
        @assigned_tasks[task] = worker
        worker.task = task
        worker_is_busy(worker)
    end

    def unassign(worker, task)
        $logger.info "Removing task #{task} for #{worker}"
        @assigned_tasks.delete(task)
        worker_is_free(worker)
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

    def to_s
        "Workers: " + @workers.join(" ") + "\n"
        "Tasks: " + @tasks_to_assign.join(" ")
    end
end

# Basic scheduler - just assign the first doable task found.
class FIFOScheduler < Scheduler
    def find_task_for(worker)
        if @busy_workers.include?(worker)
            $logger.warn "Trying to assign task to busy worker #{worker}"
            return nil
        end

        @tasks_to_assign.each do |task|
            if worker.can_do_task?(task) && worker.can_path_to?(task)
                assign(worker, task)
                return task
            end
        end
    end
end

# Only check a few jobs at a time.
# FIXME: Saw a locking issue where mining jobs were all getting invalidated
# even though there was a path.
class FIFOMeteredScheduler < Scheduler
    def initialize(*args)
        # iterators for doing metered task checking.
        @free_workers_counter = -1
        @tasks_counter = -1
        super(*args)
    end

    def check_tasks
        return if @free_workers.empty? || @tasks_to_assign.empty?

        @free_workers_counter = (@free_workers_counter + 1) % @free_workers.size
        worker = @free_workers[@free_workers_counter]

        @tasks_counter = (@tasks_counter + 1) % @tasks_to_assign.size
        task = @tasks_to_assign[@tasks_counter]

        if worker.can_do_task?(task) && worker.can_path_to?(task)
            assign(worker, task)
            @free_workers_counter -= 1
            @tasks_counter -= 1
        end
    end
end

# This scheduler will assign tasks to workers, with the relative probability of a
# worker being assigned a given type of task based on the number of those tasks
# activated among the workers. The single craftworker is more likely to
# craft than to haul, which everydwarf could do.
class SmallWorkerPoolPriorityScheduler < Scheduler
    def initialize(workers=[], tasks=[])
        super(workers, tasks)
        # Increment counts of workers for each task.
        @workers.each { |w| w.activated_tasks.each { |at| @num_task_workers[at] += 1 } }
    end

    def add_worker(worker)
        super(worker)
        worker.activated_tasks.each { |at| @num_task_workers[at] += 1 }
    end

    def remove_worker(worker)
        worker.activated_tasks.each { |at| @num_task_workers[at] -= 1 }
        super(worker)
    end

    # to be written.
    def find_task_for(worker)
        nil
    end
end

# Priority: do the closest job first.
class ClosestScheduler < Scheduler
    def find_task_for(worker)
        if @busy_workers.include?(worker)
            $logger.warn "Trying to assign task to busy worker #{worker}"
            return nil
        end

        unless @tasks_to_assign.any? { |task| worker.can_do_task?(task) }
            return nil
        end

        # First look up all the positions for all the tasks.
        tasks_at = {}
        positions = []
        @tasks_to_assign.each do |task|
            # NOTE: Later, change this to get_shortest_path when speed is no longer an issue
            potential_path = worker.world.pathfinder.get_shortest_path(*(worker.position), task.possible_worker_positions(worker.world))

            unless potential_path.empty?
#                $logger.info "path not blocked to #{pos}"
                task_pos = potential_path.last
                tasks_at[task_pos] ||= []
                tasks_at[task_pos] << task
                positions << task_pos
            else
#                $logger.warn "blocked path to #{pos}"
            end
        end

        # Read the path for the closest position.
        # XXXBMW: Path removal.
        path = nil # worker.world.pathfinder.get_shortest_path(*(worker.position), positions.uniq)

=begin
        $logger.info "tasks_at #{tasks_at.keys.inspect}"
        $logger.info "positions #{positions.inspect}"
        $logger.info "path #{path.inspect}"
=end
        destination = nil
        if path.empty?
            destination = worker.position
            if tasks_at[destination].nil?
#                $logger.warn "No path to anything in #{tasks_at.keys} from #{worker.position}?"
                return nil
            end
        else
            destination = path[-1]
        end
#        $logger.info "destination #{destination}"
        random_index = rand(tasks_at[destination].size)
        task = tasks_at[destination][random_index]
        tasks_at[destination].delete(random_index)
        # We know the path, so go ahead and set the worker path. This must happen before
        # the worker's task is set in assign, or the worker will look up the path again.
        worker.path = path
        # Assign one of the tasks to the worker.
        assign(worker, task)
        # Save other tasks at this location for later.
        #worker.next_tasks = tasks_at[destination]
        return task
    end
end

# Receives changes from the workers, manages jobs
# and sends job tasks and task changes to the scheduler.
class JobManager
    attr_reader :world
    include Schedulable
    def initialize(world, workers=[], jobs=[])
        @world = world
        @scheduler = FIFOMeteredScheduler.new(workers)
        @jobs = jobs

        @check_tasks = Proc.new { @scheduler.check_tasks }
    end

    def add_job(klass, *args)
        job = klass.new(self, *args)
        if job.completed?
            $logger.warn "Creation of #{klass} failed: no tasks"
        else
            @jobs << job
            job.tasks.each { |t| @scheduler.add_task(t) }
            @force_check = true
        end
    end

    def add_worker(worker)
        @scheduler.add_worker(worker)
    end

    def remove_task(task)
        @scheduler.remove_task(task)
        if task.finished
            # Jobs maintain their own list of tasks completed.
            task.job.mark_task_done(task)
            # Jobs may produce new tasks when current tasks are finished.
            #if task.job.respond_to?(:new_tasks) && new_tasks = task.job.new_tasks
            #    new_tasks.each { |t| @scheduler.add_task(t) }
            # Clean up the job if it's finished.
            if task.job.completed?
                @jobs.delete(task.job)
            end
            @force_check = true
        end
    end

    def update(elapsed)
        if @force_check
            @check_tasks.call
            @force_check = false
        else
            schedule_with_max(:check_tasks, 1000, elapsed, 50, @check_tasks)
        end
        schedule(:invalidate_blocked_paths, 10000, elapsed) do
            invalidate_blocked_paths
        end
    end

    def invalidate_blocked_paths
        @jobs.each { |job| job.blocked_workers = [] }
    end

    def to_s() @scheduler.to_s; end

    delegate_to :scheduler, :find_task_for, :assign
end

# Enumerate the locations a voxel can be accessed from for different job scenarios
# AT: Workers default to moving to the task's position to perform the task.
AT = [[0, 0, 0]]
# ADJACENT: Move next to the voxel to be mined.
ADJACENT =
    # just the same z-level or above in cardinal directions
    [
    [1, 0, 0], [0, 1, 0], [-1, 0, 0], [0, -1, 0],
    [1, 0, 1], [0, 1, 1], [-1, 0, 1], [0, -1, 1]
    ]

DIAG_ADJACENT = ADJACENT +
    # the same z-level or above in diagonal directions
    [
    [1, 1, 0], [-1, -1, 0], [-1, 1, 0], [1, -1, 0],
    [1, 1, 1], [-1, -1, 1], [-1, 1, 1], [1, -1, 1]
    ]

class TaskFactory
    def self.create(klass, world, params = {})
#        $logger.info "TaskFactory: creating #{klass} with #{params}"
        klass = klass.constantize if klass.respond_to?(:constantize)
        if !params[:object] && object_type = klass.needs_object
            params[:object] = world.find(object_type).first
            if params[:object]
                world.lock(params[:object])
                params[:position] = params[:object].position
            else
                $logger.warn "#{klass} failed: can't find #{object_type.to_s.humanize}"
                return
            end
        end
        klass.setup(params) if klass.respond_to?(:setup)
        klass.new(params)
    end
end

class Task
    extend RecordChildren
    attr_reader :finished, :incomplete, :parameters

    def self.needs_object() nil; end

    def job() @parameters[:job]; end
    def position() @parameters[:position]; end
    def position=(pos) @parameters[:position] = pos; end

    def initialize(parameters)
        @parameters = parameters
        @finished = false
        @incomplete = false
    end

    # A worker must be in one of these locations to perform the task.
    def relative_locations
        relative_location_strategy.collect { |loc| loc.piecewise(self.position, :+) }
    end

    # Relative locations that actually exist on-map.
    def possible_worker_positions(world)
        relative_locations.reject { |loc| world.out_of_bounds?(*loc) }
    end

    # Normal tasks have a single action; when that action is completed the task is finished.
    def action_complete() self.finished = true; end

    def finished=(val)
        if val == !!val
            @finished = val
            finalize(val, "finished")
            # We're done with this object.
            if val && @parameters[:object]
                @parameters[:object].world.unlock(@parameters[:object])
            end
        end
    end

    def incomplete=(val)
        if val == !!val
            @incomplete = val
            finalize(val, "incomplete")
        end
    end

    def self.to_s() super.humanize; end
    def to_s() self.class.to_s; end

    private

    def finalize(val, note)
        # Report to jobmanager.
        if val && self.job && self.job.jobmanager
            $logger.info "#{self.class} closed: #{note}"
            self.job.jobmanager.remove_task(self)
        end
    end

    def relative_location_strategy() AT; end
end

class MoveTask < Task; end

class MineTask < Task
    def action() :mine; end
    private
    def relative_location_strategy() DIAG_ADJACENT; end
end

class MultiActionTask < Task
    def action() @actions.first[0]; end
    def position() @actions.first[1]; end
    def position=(pos) @actions.first[1] = pos; end
    private
    def relative_location_strategy() @actions.first[2]; end
    public

    def initialize(*args)
        super(*args)
        init_actions
    end
    def init_actions(); end

    def action_complete
        @actions.shift
        self.finished = true if @actions.empty?
    end

    def incomplete=(val)
        super(val)
        init_actions if val
    end
end

class MoveObjectTask < MultiActionTask
    def init_actions
        @actions = [[:pickup, @parameters[:object].position, AT], [:drop, @parameters[:final_position], AT]]
    end
end

class BuildWallTask < MultiActionTask
    def self.needs_object() BoulderModule; end

    def init_actions
        @actions = [[:pickup, @parameters[:object].position, AT], [:build_wall, @parameters[:final_position], DIAG_ADJACENT]]
    end
end

class FleeTask < MoveTask; end
class GrazeTask < Task; end
class EatTask < Task
    def self.needs_object() FoodModule; end
end

class Job
    attr_writer :blocked_workers
    attr_reader :jobmanager
    def blocked_workers() @blocked_workers ||= []; end

    def initialize(jobmanager, position = nil)
        @jobmanager = jobmanager
        @position = position
        @tasks = self.tasks
    end

    # Initial tasks for this job.
    def tasks() []; end

    def mark_task_done(task)
        @tasks.delete(task)
    end

    def completed?
        @tasks.empty?
    end
end

class Move < Job
    def tasks
        @tasks ||= [TaskFactory.create(MoveTask, jobmanager.world, :position => @position, :job => self)].compact
    end
end

class Mine < Job
    def tasks
        @tasks ||= [TaskFactory.create(MineTask, jobmanager.world, :position => @position, :job => self)].compact
    end
end

class MoveObject < Job
    def initialize(jobmanager, position, object)
        @object = object
        # Go to the position of the task (where to move the object) afterwards.
        @final_position = position
        super(jobmanager)
    end

    def tasks
        @tasks ||= [TaskFactory.create(MoveObjectTask, jobmanager.world, :object => @object, :final_position => @final_position, :job => self)].compact
    end
end

class BuildWall < Job
    def initialize(jobmanager, position, object = nil)
        @object = object
        # Go to the position of the task (where to move the object) afterwards.
        @final_position = position
        super(jobmanager)
    end

    def tasks
        @tasks ||= [TaskFactory.create(BuildWallTask, jobmanager.world, :object => @object, :final_position => @final_position, :job => self)].compact
    end
end

# Should only be included in Actors, or things with a world reference.
module Movement
    attr_accessor :path
    attr_reader :last_position

    def move(elapsed)
        if @path
            # Note: We are relying on something else to determine whether or not the path has been interrupted,
            #  operating under the assumption that eventually some external thing will be responsible for doing
            #  the actual moving of entities, with entities dictating where they intend to go (so that things like
            #  physics can be abstracted outside of the movers)
            # TODO - The move speed will eventually be a function of the instantiated dwarf, not just the dwarf class (ie an agility attribute that affects how fast a dwarf can move, armor that slows dwarves down, etc)
            distance_covered = self.class.move_speed * elapsed
            previous_position = self.position
            while true
                if @path.empty?
                    $logger.info "This shouldn't happen here! #{self.to_s} at #{self.position}"
                end
                next_position = @path.first
                distance_to_next_position = previous_position.distance_to(next_position)
                if distance_covered >= distance_to_next_position
                    $logger.info "#{self.inspect} reached path node #{next_position.inspect}"
                    @last_position = @path.shift
                    if @path.empty?
                        set_position(*next_position)
                        $logger.info "End of path reached for #{self.to_s}."
                        @path = nil
                        break
                    end
                    previous_position = next_position
                    distance_covered -= distance_to_next_position
                else
                    lerp_scalar     = distance_covered / distance_to_next_position
                    scaled_previous = previous_position.collect { |i| i * (1.0 - lerp_scalar) }
                    scaled_next     = next_position.collect     { |i| i * lerp_scalar }
                    final_position  = scaled_previous.piecewise(scaled_next, :+)
#                   $logger.info "#{self.inspect} moves to #{final_position.inspect}, path #{path.inspect}"
                    set_position(*final_position)
                    break
                end
            end
        end
    end
end

# Should only be assigned to Actors, or things with a world reference.
module TaskHandling
    # Assume workers can move...
    include Movement

    # Activated tasks being a subset of the tasks a worker is capable of
    attr_writer :capable_tasks, :activated_tasks
    attr_reader :task
    attr_accessor :next_tasks

    # Default to being capable of all tasks, and all tasks activated.
    def capable_tasks() @capable_tasks ||= Task.children; end
    def activated_tasks() @activated_tasks ||= Task.children; end

    def can_do_task?(task)
        activated_tasks.include?(task.class) && !(task.job && task.job.blocked_workers.include?(self))
    end

    def can_path_to?(task)
        # XXXBMW: Path removal.
        # calculate_path
        return false

        # Worker is already in a position to do this task.
#        return true if task.relative_locations.include?(self.position)
#
#        path = @world.pathfinder.get_first_path(*self.position, task.possible_worker_positions(@world))
#        if path.empty?
#            $logger.info "#{self} can't path to #{task.inspect} (#{task.possible_worker_positions(@world)}) from #{self.position}"
#            task.job.blocked_workers << self if task.job
#            return false
#        else
#            @path = path
#            $logger.info "#{self.to_s} (#{self.position}) has path to #{task}'s (#{@path[-1]})"
#            return true
#        end
    end

    def task=(task)
        @task = task
        # XXXBMW: Path removal.
        # calculate_path
    end

    # Recalculation needs to be forced when a path node has been removed but
    # still may be the endpoint of the path and in the relative_locations.
    def calculate_path(force_recalculation = false)
        if @task
#            $logger.info "calculating path for #{self.name}: #{@path}, #{@task.possible_worker_positions(@world).include?(@path[-1])}, #{force_recalculation}" if @path
            # Are we there already? No path necessary.
            if @task.relative_locations.include?(self.position)
                @path = nil
            # No need to generate a path if we're already moving there.
            elsif !(@path && @task.relative_locations.include?(@path[-1])) || force_recalculation
                @path = @world.pathfinder.get_shortest_path(*self.position, task.possible_worker_positions(@world))
                if @path.empty?
                    $logger.info "#{self} can't path to #{@task.inspect} (#{@task.possible_worker_positions(@world)}) from #{self.position}"
                    @path = nil
                    @task.job.blocked_workers << self if @task.job
                    # We already have a task, and we can't path to it?
                    incomplete_task
#                else
#                    $logger.info "#{self.to_s} (#{self.position}) has path to #{task}'s (#{@path[-1]})"
                end
            end
        end
    end

    def action_complete
        return if self.task.nil?
        unless self.task.parameters[:no_complete]
            self.task.action_complete
            if self.task.finished
                self.task = nil
            else
                # The action shifted, meaning there's probably a new position. Recalculate the path.
                calculate_path
            end
        end
    end

    def incomplete_task(followup = nil)
        return if @task.nil?
        if task.parameters[:object_held]
            self.task.parameters[:no_complete] = true
            drop(task, 0)
            self.task.parameters[:no_complete] = nil
        end
        @task.incomplete = true
        self.task = followup
    end
end

module Actions
    def mine(task, elapsed)
        $logger.info "Mining voxel at #{task.position}"

        voxel_type = @world.get_voxel_type(*task.position)
        if voxel_type.respond_to?(:drops)
            drops = voxel_type.drops.constantize
            # Create an instance of drops class.
            if drops.respond_to?(:manager)
                new_item = drops.manager.create_child(world, drops, task.position)
            else
                new_item = world.create(drops)
                new_item.position = task.position
            end
        end

        @world.set_voxel_type(*task.position, nil)
        action_complete
    end

    # Pickups are assumed to be instantaneous.
    def pickup(task, elapsed = 0)
        object = task.parameters[:object]
        # Find an unfilled carrying slot.
        self.inventory.values.each do |slot|
            if slot.is_a?(CarryingSlot) && slot.can_fit(object)
                $logger.info "#{self.inspect} puts the #{object} in #{slot.class}"
                slot.add_object(@world, object)
                # Tell the task we've picked up the object.
                task.parameters[:object_held] = true
                action_complete
                return object
            end
        end
        # No unfilled slots found; this is unlikely.
        task.job.blocked_workers << self if task.job
        incomplete_task
    end

    # Drops are assumed to be instantaneous.
    def drop(task, elapsed = 0)
        object = task.parameters[:object]
        # Find the slot carrying the object.
        self.inventory.values.each do |slot|
            if slot.contents.include?(object)
                $logger.info "#{self.inspect} removes the #{object} from #{slot.class}"
                slot.remove_object(@world, object, self.position)
                # Tell the task we've dropped the object.
                task.parameters[:object_held] = false
                action_complete
                return object
            end
        end
        # Object was not found; this really shouldn't happen, ever.
        $logger.error "Worker #{self.inspect} shouldn't have received #{task} for object #{object}!"
        incomplete_task
    end

    def build_wall(task, elapsed)
        object = task.parameters[:object]
        # We have the object, and we've pathed to the final destination.
        # No cask of Amontillado situations allowed. However, dwarves just wall over items.
        collidees = @world.find(CreatureModule, :position => task.position)
        if !collidees.empty?
            $logger.info "collidees are #{collidees}"
            incomplete_task
            return
        end
        $logger.info "#{self} building wall at #{task.position}"
        # Don't call action_complete from drop.
        task.parameters[:no_complete] = true
        drop(task)
        task.parameters[:no_complete] = nil
        @world.destroy(object)
        @world.set_voxel_type(*task.position, Wall)
        action_complete
    end
end

# Handle both @path movement and @task actions if either is defined.
class Actor < MHActor
    # Eventually actors won't update themselves, but will all have managers doing that.
    def update(elapsed)
        # Short-circuit plants and other boring Actors that do nothing.
        return unless self.class.include?(Movement) || self.class.include?(TaskHandling)

        @sleep = (@sleep? @sleep - elapsed : 0)
        # Do nothing if still sleeping.
        return unless @sleep <= 0

        # Move there first.
        if @path
            move(elapsed)
        # Then run the task until it reports itself complete.
        elsif self.class.include?(TaskHandling)
            if @task
                if @task.respond_to?(:action)
                    self.send(@task.action, @task, elapsed)
                else
                    action_complete
                end
            elsif self.class.manager
                schedule(:decide_task, 3000, elapsed) do
                    self.class.manager.decide_task(self)
                end
            end
        end
    end
end
