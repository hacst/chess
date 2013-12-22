#include <gtest/gtest.h>
#include <core/Logging.h>

using namespace std;
using namespace testing;
using namespace Logging;

#ifndef TEST_LOG_NAME
#define TEST_LOG_NAME "test.log"
#endif

#ifndef TEST_LOG_LEVEL
#define TEST_LOG_LEVEL info
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
            LOG(warning) << "Failure "
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
    addLoggingConsoleSink(error);
    addLoggingFileSink(TEST_LOG_NAME, TEST_LOG_LEVEL);

    // Register our own event listener so the test log contains enough information
    // to match log messages to specific tests.
    TestEventListeners& listeners = UnitTest::GetInstance()->listeners();
    listeners.Append(new LoggingPrinter());

    InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}