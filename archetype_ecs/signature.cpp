#include "signature.h"

namespace Vivium {
	namespace ECS {
		signature_t::signature_t() : enabled(0) {}

		bool signature_t::operator==(const signature_t& other) const {
			return enabled == other.enabled;
		}

		bool signature_t::operator!=(const signature_t& other) const {
			return enabled != other.enabled;
		}
	}
}