#include "error_handler.h"

namespace Vivium {
	namespace ECS {
		void default_error_callback(error_detail detail) {}

		const char* severity_to_string(severity sev) {
			switch (sev) {
			case DEBUG: return "DEBUG";
			case WARN:	return "WARN";
			case ERROR: return "ERROR";
			case FATAL: return "FATAL";
			default:	return "UNKNOWN";
			}
		}

		void print_errors(severity minimum) {
			for (const error_detail& detail : error_list) {
				if (detail.sev < minimum) continue;

				std::cout << std::format(
					"[{:%H:%M:%OS}] {}: {}:{} {}",
					detail.timestamp,
					severity_to_string(detail.sev),
					detail.file,
					detail.line,
					detail.message
				) << std::endl;
			}
		}

		void consume_errors() {
			print_errors();
			error_list.clear();
		}

		void push_error(const error_detail& detail) {
			error_list.push_back(detail);

			error_callback(detail);

			if (detail.sev == severity::FATAL) {
				std::cout << "-- FATAL ERROR OCCURED --" << std::endl;

				print_errors(Vivium::ECS::DEBUG);
				std::terminate();
			}
		}

		error_detail::error_detail(const char* file, const char* function, uint32_t line, severity sev, std::string message) :
			file(file), function(function), line(line),
			sev(sev), message(message),
			timestamp(std::chrono::system_clock::now()) {}
	}
}