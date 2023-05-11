#pragma once

#include <memory>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>


struct log
{
	using logger_ptr = const std::shared_ptr<spdlog::logger>;

	// loggers available to use
	logger_ptr main_logger;
	logger_ptr network_logger;
	logger_ptr statement_logger;
	logger_ptr server_logger;
	logger_ptr table_logger;

	const log& get_instance()
	{
		return *this;
	}

	private:
		log() : 
			main_logger(spdlog::stdout_color_mt("main_logger")),
			network_logger(spdlog::stdout_color_mt("network_logger")),
			statement_logger(spdlog::stdout_color_mt("statement_logger")),
			server_logger(spdlog::stdout_color_mt("server_logger")),
			table_logger(spdlog::stdout_color_mt("table_logger"))
		{}
}
