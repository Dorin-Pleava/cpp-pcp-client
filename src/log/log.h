#ifndef CTHUN_CLIENT_SRC_LOG_LOG_H_
#define CTHUN_CLIENT_SRC_LOG_LOG_H_



// TODO(ale): todo! So far, this is the same as the cfacter one.


// To use this header, you must:
// - Have Boost on the include path
// - Link in Boost.Log
// - Configure Boost.Log at runtime before any logging takes place

/**
 * See Boost.Log's documentation.
 */
#define BOOST_LOG_DYN_LINK
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/format.hpp>
#include <cstdio>

/**
 * Defines the root logging namespace.
 */
#define LOG_ROOT_NAMESPACE "puppetlabs.cthun.client."

/**
 * Used to declare a logging namespace for a source file.
 * This macro must be used before any other logging macro.
 * @param ns The logging namespace name.
 */
#define LOG_DECLARE_NAMESPACE(ns) static const std::string g_logger = LOG_ROOT_NAMESPACE ns;
/**
 * Logs a message.
 * @param level The logging level for the message.
 * @param format The format message.
 * @param ... The format message parameters.
 */
#define LOG_MESSAGE(level, format, ...) \
    if (facter::logging::is_log_enabled(g_logger, level)) { \
        facter::logging::log(g_logger, level, format, ##__VA_ARGS__); \
    }
/**
 * Logs a trace message.
 * @param format The format message.
 * @param ... The format message parameters.
 */
#define LOG_TRACE(format, ...) LOG_MESSAGE(facter::logging::log_level::trace, format, ##__VA_ARGS__)
/**
 * Logs a debug message.
 * @param format The format message.
 * @param ... The format message parameters.
 */
#define LOG_DEBUG(format, ...) LOG_MESSAGE(facter::logging::log_level::debug, format, ##__VA_ARGS__)
/**
 * Logs an info message.
 * @param format The format message.
 * @param ... The format message parameters.
 */
#define LOG_INFO(format, ...) LOG_MESSAGE(facter::logging::log_level::info, format, ##__VA_ARGS__)
/**
 * Logs a warning message.
 * @param format The format message.
 * @param ... The format message parameters.
 */
#define LOG_WARNING(format, ...) LOG_MESSAGE(facter::logging::log_level::warning, format, ##__VA_ARGS__)
/**
 * Logs an error message.
 * @param format The format message.
 * @param ... The format message parameters.
 */
#define LOG_ERROR(format, ...) LOG_MESSAGE(facter::logging::log_level::error, format, ##__VA_ARGS__)
/**
 * Logs a fatal message.
 * @param format The format message.
 * @param ... The format message parameters.
 */
#define LOG_FATAL(format, ...) LOG_MESSAGE(facter::logging::log_level::fatal, format, ##__VA_ARGS__)
/**
 * Determines if the given logging level is enabled.
 * @param level The logging level to check.
 */
#define LOG_IS_ENABLED(level) facter::logging::is_log_enabled(g_logger, level)
/**
 * Determines if the trace logging level is enabled.
 * @returns Returns true if trace logging is enabled or false if it is not enabled.
 */
#define LOG_IS_TRACE_ENABLED() LOG_IS_ENABLED(facter::logging::log_level::trace)
/**
 * Determines if the debug logging level is enabled.
 * @returns Returns true if debug logging is enabled or false if it is not enabled.
 */
#define LOG_IS_DEBUG_ENABLED() LOG_IS_ENABLED(facter::logging::log_level::debug)
/**
 * Determines if the info logging level is enabled.
 * @returns Returns true if info logging is enabled or false if it is not enabled.
 */
#define LOG_IS_INFO_ENABLED() LOG_IS_ENABLED(facter::logging::log_level::info)
/**
 * Determines if the warning logging level is enabled.
 * @returns Returns true if warning logging is enabled or false if it is not enabled.
 */
#define LOG_IS_WARNING_ENABLED() LOG_IS_ENABLED(facter::logging::log_level::warning)
/**
 * Determines if the error logging level is enabled.
 * @returns Returns true if error logging is enabled or false if it is not enabled.
 */
#define LOG_IS_ERROR_ENABLED() LOG_IS_ENABLED(facter::logging::log_level::error)
/**
 * Determines if the fatal logging level is enabled.
 * @returns Returns true if fatal logging is enabled or false if it is not enabled.
 */
#define LOG_IS_FATAL_ENABLED() LOG_IS_ENABLED(facter::logging::log_level::fatal)

// TODO(ale): correct namespace?

namespace Cthun {
namespace Log {

/**
 * Represents the supported logging levels.
 */
enum class log_level
{
    trace,
    debug,
    info,
    warning,
    error,
    fatal
};

/**
 * The Boost.Log attribute for log level (severity).
 * The BOOST_LOG_SEV macro implicitly adds a source-specific attribute
 * "Severity" of the template type on construction, so the attribute
 * name "Severity" of log_level_attr is tied to BOOST_LOG_SEV.
 */
BOOST_LOG_ATTRIBUTE_KEYWORD(log_level_attr, "Severity", log_level);
/**
 * The Boost.Log attribute for namespace.
 */
BOOST_LOG_ATTRIBUTE_KEYWORD(namespace_attr, "Namespace", std::string);

/**
 * Produces the printed representation of logging level.
 * @param strm The stream to write.
 * @param level The logging level to print.
 * @return Returns the stream after writing to it.
 */
std::ostream& operator<<(std::ostream& strm, log_level level);

/**
 * Configures default logging to the specified stream.
 * @param level The logging level to allow on the console.
 * @param dst Destination stream for logging output.
 */
void configure_logging(log_level level, std::ostream &dst);

/**
 * Determines if the given log level is enabled for the given logger.
 * @param logger The logger to check.
 * @param level The logging level to check.
 * @return Returns true if the logging level is enabled or false if it is not.
 */
bool is_log_enabled(const std::string &logger, log_level level);

/**
 * Logs a given message to the given logger.
 * @param logger The logger to log the message to.
 * @param level The logging level to log with.
 * @param message The message to log.
 */
void log(const std::string &logger, log_level level, std::string const& message);

/**
 * Logs a given format message to the given logger.
 * @param logger The logger to log the message to.
 * @param level The logging level to log with.
 * @param message The message being formatted.
 */
void log(const std::string &logger, log_level level, boost::format& message);

/**
 * Logs a given format message to the given logger.
 * @tparam T The type of the first argument.
 * @tparam TArgs The types of the remaining arguments.
 * @param logger The logger to log to.
 * @param level The logging level to log with.
 * @param message The message being formatted.
 * @param arg The first argument to the message.
 * @param args The remaining arguments to the message.
 */
template <typename T, typename... TArgs>
void log(const std::string &logger, log_level level, boost::format& message, T arg, TArgs... args)
{
    message % arg;
    log(logger, level, message, std::forward<TArgs>(args)...);
}

/**
 * Logs a given format message to the given logger.
 * @tparam TArgs The types of the arguments to format the message with.
 * @param logger The logger to log to.
 * @param level The logging level to log with.
 * @param format The message format.
 * @param args The remaining arguments to the message.
 */
template <typename... TArgs>
void log(const std::string &logger, log_level level, std::string const& format, TArgs... args)
{
    boost::format message(format);
    log(logger, level, message, std::forward<TArgs>(args)...);
}

}  // namespace Cthun
}  // namespace Log

#endif  // CTHUN_CLIENT_SRC_LOG_LOG_H_
