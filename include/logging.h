#pragma once

#include <memory>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>


using logger_ptr = std::shared_ptr<spdlog::logger>;

// loggers available to use

extern logger_ptr main_logger;
extern logger_ptr network_logger;
extern logger_ptr statement_logger;
extern logger_ptr server_logger;
extern logger_ptr table_logger;

inline void setup_logger()
{
	main_logger = spdlog::stdout_color_mt("main_logger");
	table_logger = spdlog::stdout_color_mt("table_logger");
	network_logger = spdlog::stdout_color_mt("network_logger");
	server_logger = spdlog::stdout_color_mt("server_logger");
	statement_logger = spdlog::stdout_color_mt("statement_logger");
}
