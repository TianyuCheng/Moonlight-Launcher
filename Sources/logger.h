#ifndef LOGGER_H
#define LOGGER_H

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

struct RingBufferSink : public spdlog::sinks::sink
{
public:
    explicit RingBufferSink(size_t capacity) : capacity(capacity) {}

    void log(const spdlog::details::log_msg& msg) override
    {
        std::string message = fmt::to_string(msg.payload);
        if (buffer.size() < capacity) {
            buffer.push_back(message);
        } else {
            buffer[index] = message; // overwrite oldest message
            index         = (index + 1) % capacity;
        }
    }

    void flush() override
    {
        // Do nothing because statement executed in sink_it_().
    }

    void set_pattern(const std::string& pattern) override
    {
        // Don't format log message.
    }

    void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) override
    {
        // Don't format log message.
    }

    const auto& logs() const
    {
        return buffer;
    }

private:
    size_t                   capacity = 0;
    size_t                   index    = 0;
    std::vector<std::string> buffer;
};

struct Logger
{
    static std::shared_ptr<spdlog::logger> logger;

    static void set_logger(std::shared_ptr<spdlog::logger> logger)
    {
        Logger::logger = logger;
        spdlog::register_logger(logger);
    }

    static auto get_logger() -> std::shared_ptr<spdlog::logger>
    {
        return Logger::logger;
    }

    template <typename... Args>
    static void info(const char* fmt, Args&&... args)
    {
        Logger::logger->info(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void warn(const char* fmt, Args&&... args)
    {
        Logger::logger->warn(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void error(const char* fmt, Args&&... args)
    {
        Logger::logger->error(fmt, std::forward<Args>(args)...);
    }
};

#endif // LOGGER_H
