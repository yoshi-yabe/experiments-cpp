#include "gtest/gtest.h"
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE  // デフォルトは INFO
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

TEST(SpdLog, testBasicFile)
{
    auto logger = spdlog::basic_logger_mt("basic_logger", "logs/basic-log.txt");
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e][%l] %v");
    logger->set_level(spdlog::level::trace);

    logger->info("Welcome to spdlog!");
    SPDLOG_LOGGER_TRACE(logger, "logger->level()={}", logger->level());
    SPDLOG_LOGGER_DEBUG(logger, "logger->name()={}", logger->name());

    spdlog::set_default_logger(logger);
    SPDLOG_TRACE("logger 省略時は default logger");
    SPDLOG_DEBUG("logger 省略時は default logger");
}
