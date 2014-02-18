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
 * @brief Basic initialization for logging.
 * @see addLoggingConsoleSind
 * @see addLoggingFileSink
 */
void initializeLogging();

/**
 * @brief Adds a sink for logging to clog
 * @param minimalSeverity Minimum severity filter
 * @param formatString Format string for log output (default boost log with added Severity variable)
 */
void addLoggingConsoleSink(
    Severity minimalSeverity = info,
    const std::string& formatString = "%TimeStamp% | %Severity% @ <%Channel%>: %Message%");

/**
* @brief Adds a sink for logging to a file
* @param fileName File to log to
* @param minimalSeverity Minimum severity filter
* @param formatString Format string for log output (default boost log with added Severity variable)
*/
void addLoggingFileSink(const std::string& fileName, Severity minimalSeverity = info,
    const std::string& formatString = "%TimeStamp% | %Severity% @ <%Channel%>: %Message%");


/**
* @brief Adds a sink for logging to clog for one channel
* @param specificChannel Channel to log
* @param minimalSeverity Minimum severity filter
* @param formatString Format string for log output (default boost log with added Severity variable)
*/
void addChannelSpecificLoggingConsoleSink(
    const std::string& specificChannel,
    Severity minimalSeverity = info,
    const std::string& formatString = "%TimeStamp% | %Severity% @ <%Channel%>: %Message%");

/**
* @brief Adds a sink for logging to a file for one channel
* @param fileName File to log to
* @param specificChannel Channel to log
* @param minimalSeverity Minimum severity filter
* @param formatString Format string for log output (default boost log with added Severity variable)
*/
void addChannelSpecificLoggingFileSink(
    const std::string& fileName,
    const std::string& specificChannel,
    Severity minimalSeverity = info,
    const std::string& formatString = "%TimeStamp% | %Severity% @ <%Channel%>: %Message%");

/**
 * @return Returns a Logger with the given channel name.
 */
Logger initLogger(const std::string& channel);

} // namespace Logging

#endif //LOGGING_H
