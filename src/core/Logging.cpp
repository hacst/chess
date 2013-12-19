#include "Logging.h"

#include <algorithm>
#include <iostream>
#include <vector>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

using namespace std;

namespace keywords = boost::log::keywords;

namespace Logging {

BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", Severity)

static const vector<string> stringSeverities = {
    "trace",
    "debug",
    "info",
    "warning",
    "error",
    "fatal"
};

std::ostream& operator<<(std::ostream& stream, const Severity severity) {
    const size_t idx = static_cast<size_t>(severity);
    
    if (idx < stringSeverities.size()) {
        stream << stringSeverities[idx];
    } else {
        stream << idx;
    }
    return stream;
}

boost::optional<Severity> severityFrom(const string &severity) {
    auto it = find(begin(stringSeverities), end(stringSeverities), severity);
    
    if (it == end(stringSeverities)) {
        return boost::none;
    }
    
    return static_cast<Severity>(it - begin(stringSeverities));
}

void initialize(const string& fileName, Severity minimalSeverity) {
    boost::log::register_simple_formatter_factory< Severity,char >("Severity");
    
    boost::log::add_console_log(clog,
        keywords::format = "%Severity% @ %Channel%: %Message%"
    );
    
    boost::log::add_file_log(
        keywords::file_name = fileName,
        keywords::format = "%TimeStamp% | %Severity% @ <%Channel%>: %Message%",
        keywords::auto_flush = true
    );
    
    boost::log::core::get()->set_filter(severity >= minimalSeverity);
    boost::log::add_common_attributes();
}

Logger initLogger(const string &channel) {
    return Logger(keywords::channel = channel);
}

} // namespace logging
