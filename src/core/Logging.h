#ifndef LOGGING_H
#define LOGGING_H

#include <ostream>
#include <string>
#include <boost/optional/optional.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_feature.hpp>

namespace Logging {

//! Logging severity levels
enum Severity
{
    trace,
    debug,
    info,
    warning,
    error,
    fatal
};

#ifndef LOGGING_NO_DEFINES
// Assumes a convention of m_log being the class logger
#define LOG(sev) BOOST_LOG_SEV(m_log, sev)
#define GLOG(sev) BOOST_LOG_SEV(log, sev)

#endif


//! Typedef for custom severity + channel logger
using Logger = boost::log::sources::severity_channel_logger_mt<
    Severity, // Severity level
    std::string // Channel name
>;

std::ostream& operator<<(std::ostream& stream, const Severity severity);

/**
 * @return Severity level from given string. boost::none if severity is invalid.
 */
boost::optional<Severity> severityFrom(const std::string& severity);

/**
 * @brief Initializes logging with a console and a file log and the given severity.
 * @param fileName File name for file log.
 * @param minimalSeverity Severity below which to discard log messages.
 */
void initialize(const std::string& fileName, Severity minimalSeverity = info);

/**
 * @return Returns a Logger with the given channel name.
 */
Logger initLogger(const std::string& channel);

} // namespace Logging

#endif //LOGGING_H
