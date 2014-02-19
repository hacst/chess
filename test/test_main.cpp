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
#include <gtest/gtest.h>
#include <core/Logging.h>

using namespace std;
using namespace testing;
using namespace Logging;

#ifndef TEST_LOG_NAME
#define TEST_LOG_NAME "test.log"
#endif

#ifndef TEST_LOG_LEVEL
#define TEST_LOG_LEVEL debug
#endif

/**
 * @brief Logger for redirecting key Google Test events into our log.
 */
class LoggingPrinter : public EmptyTestEventListener  {
public:
    LoggingPrinter()
        : m_log(initLogger("GoogleTest")) { /* Empty */ };

    virtual void OnTestStart(const TestInfo& test_info) override {
        LOG(info) << "Test " << test_info.test_case_name() << "." << test_info.name() << " starting";
    }

    virtual void OnTestPartResult(const TestPartResult& test_part_result) {
        if (test_part_result.failed()) {
            LOG(error) << "Failure "
                << test_part_result.file_name() << ":" << test_part_result.line_number()
                << endl
                << test_part_result.summary();
        } else {
            LOG(info) << "Success "
                << test_part_result.file_name() << ":" << test_part_result.line_number()
                << endl
                << test_part_result.summary();
        }
    }

    virtual void OnTestEnd(const TestInfo& test_info) override {
        LOG(info) << "Completed " << test_info.test_case_name() << "." << test_info.name();
    }

private:
    Logger m_log;
};

int main(int argc, char **argv) {
    initializeLogging();
    addLoggingFileSink(TEST_LOG_NAME, TEST_LOG_LEVEL);

    // Register our own event listener so the test log contains enough information
    // to match log messages to specific tests.
    TestEventListeners& listeners = UnitTest::GetInstance()->listeners();
    listeners.Append(new LoggingPrinter());

    InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
