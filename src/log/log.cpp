#include "log.h"
#include <vector>



// TODO(ale): todo!



// boost includes are not always warning-clean. Disable warnings that
// cause problems before including the headers, then re-enable the warnings.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra"

#include <boost/log/support/date_time.hpp>
#include <boost/log/expressions/formatters/date_time.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>

#pragma GCC diagnostic pop

using namespace std;
using boost::format;
namespace expr = boost::log::expressions;
namespace src = boost::log::sources;
namespace keywords = boost::log::keywords;

namespace Cthun {
namespace Log {


bool is_log_enabled(const std::string &logger, log_level level) {
    // If the severity_logger returns a record for the specified
    // level, logging is enabled. Otherwise it isn't.
    // This is the guard call used in BOOST_LOG_SEV; see
    // http://www.boost.org/doc/libs/1_54_0/libs/log/doc/html/log/detailed/sources.html
    src::severity_logger<log_level> slg;
    return (slg.open_record(keywords::severity = level) ? true : false);
}

void configure_logging(facter::logging::log_level level, ostream &dst)
{
    // Set filtering based on log_level (info, warning, debug, etc).
    auto sink = boost::log::add_console_log(dst);

    sink->set_formatter(
        expr::stream
            << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f")
            << " " << left << setfill(' ') << setw(5) << facter::logging::log_level_attr
            << " " << facter::logging::namespace_attr
            << " - " << expr::smessage);

    sink->set_filter(facter::logging::log_level_attr >= level);
    boost::log::add_common_attributes();
}

std::ostream& operator<<(std::ostream& strm, log_level level)
{
    std::vector<std::string> strings = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
    if (static_cast<size_t>(level) < strings.size()) {
        strm << strings[static_cast<size_t>(level)];
    } else {
        strm << static_cast<int>(level);
    }
    return strm;
}

void log(const string &logger, log_level level, format& message)
{
    log(logger, level, message.str());
}

}  // namespace Cthun
}  // namespace Log
