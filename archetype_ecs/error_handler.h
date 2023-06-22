#pragma once

#include <cstdint>
#include <iostream>
#include <format>
#include <chrono>
#include <string>

namespace Vivium {
	namespace ECS {
		struct error_detail;

		// Do nothing
		void default_error_callback(error_detail detail);

		typedef void (*error_callback_t)(error_detail);

		inline error_callback_t error_callback = default_error_callback;

		enum severity : uint32_t {
			DEBUG,
			WARN,
			ERROR,
			FATAL
		};

		const char* severity_to_string(severity sev);

		struct error_detail {
			const char* file;
			const char* function;
			uint32_t line;

			severity sev;
			std::string message;

			std::chrono::system_clock::time_point timestamp;

			error_detail(const char* file, const char* function, uint32_t line,
				severity sev, std::string message);
		};

		inline std::vector<error_detail> error_list;

		void print_errors(severity minimum = severity::WARN);

		void consume_errors();

		void push_error(const error_detail& detail);
	}
}

#define VIVIUM_ECS_ERROR(_severity, _message, ...) \
	Vivium::ECS::push_error(Vivium::ECS::error_detail(__FILE__, __FUNCSIG__, __LINE__, \
		_severity, std::format(_message, __VA_ARGS__)))