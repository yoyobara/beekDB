#pragma once

#include <memory>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

// loggers available to use
inline void setup_logger()
{
	spdlog::stdout_color_mt("main");
	spdlog::stdout_color_mt("network");
	spdlog::stdout_color_mt("table");
	spdlog::stdout_color_mt("handle");
	spdlog::stdout_color_mt("server");
}
