#include <memory>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>


namespace logging
{
	using logger_ptr = const std::shared_ptr<spdlog::logger>;

	// loggers available to use

	logger_ptr main_logger = spdlog::stdout_color_mt("main_logger");
	logger_ptr network_logger = spdlog::stdout_color_mt("network_logger");
	logger_ptr statement_logger = spdlog::stdout_color_mt("statement_logger");
	logger_ptr server_logger = spdlog::stdout_color_mt("statement_logger");
}

inline void setup_logger()
{
	spdlog::set_default_logger(logging::main_logger);
}
