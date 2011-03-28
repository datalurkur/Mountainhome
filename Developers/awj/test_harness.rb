class FakeLogger
    def info(msg)
        puts "INFO: #{msg}"
    end
    def error(msg)
        puts "ERROR: #{msg}"
    end
end

$logger = FakeLogger.new
