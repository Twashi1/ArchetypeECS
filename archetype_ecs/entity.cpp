#include "entity.h"
#include "archetype.h"

namespace Vivium {
	namespace ECS {
		entity_t::entity_t() : value(ENTITY_NULL), archetype(nullptr), index(INVALID_INDEX) {}
		
		uint32_t entity_t::id(entity_value_t value) {
			return value & ENTITY_MASK_ID;
		}
		
		uint32_t entity_t::version(entity_value_t value) {
			return (value & ENTITY_MASK_VERSION) >> ENTITY_SHIFT_VERSION;
		}
		
		uint32_t entity_t::set_id(uint32_t value, uint32_t id) {
			return value & ENTITY_MASK_VERSION + id;
		}
		
		uint32_t entity_t::set_version(uint32_t value, uint32_t version) {
			return value & ENTITY_MASK_ID + (version << ENTITY_SHIFT_VERSION);
		}
	}
}
