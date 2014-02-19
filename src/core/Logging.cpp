/*
    Copyright (c) 2013-2014, Stefan Hacker <dd0t@users.sourceforge.net>

    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

    3. Neither the name of the copyright holder nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/
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
BOOST_LOG_ATTRIBUTE_KEYWORD(channel, "Channel", std::string)

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

void initializeLogging() {
    boost::log::register_simple_formatter_factory<Severity, char>("Severity");
    boost::log::add_common_attributes();
}

void addLoggingConsoleSink(Severity minimalSeverity, const string& formatString) {
    boost::log::add_console_log(clog,
        keywords::format = formatString,
        keywords::filter = severity >= minimalSeverity
    );
}

void addLoggingFileSink(const string& fileName, Severity minimalSeverity, const string& formatString) {
    boost::log::add_file_log(
        keywords::file_name = fileName,
        keywords::format = formatString,
        keywords::auto_flush = true,
        keywords::filter = severity >= minimalSeverity
    );
}

void addChannelSpecificLoggingConsoleSink(const std::string& specificChannel, Severity minimalSeverity, const string& formatString) {
    boost::log::add_console_log(clog,
        keywords::format = formatString,
        keywords::filter = severity >= minimalSeverity && (channel == specificChannel)
        );
}

void addChannelSpecificLoggingFileSink(const string& fileName, const std::string& specificChannel, Severity minimalSeverity, const string& formatString) {
    boost::log::add_file_log(
        keywords::file_name = fileName,
        keywords::format = formatString,
        keywords::auto_flush = true,
        keywords::filter = severity >= minimalSeverity && (channel == specificChannel)
        );
}

Logger initLogger(const string &channel) {
    return Logger(keywords::channel = channel);
}

} // namespace logging
