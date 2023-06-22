#pragma once

#include "constants.h"

namespace Vivium {
	namespace ECS {
		struct archetype_t;

		struct entity_t {
			entity_value_t value;
			archetype_t* archetype;
			uint32_t index; // Index within archetype

			entity_t();

			static uint32_t id(entity_value_t value);
			static uint32_t version(entity_value_t value);
			static uint32_t set_id(uint32_t value, uint32_t id);
			static uint32_t set_version(uint32_t value, uint32_t version);
		};
	}
}