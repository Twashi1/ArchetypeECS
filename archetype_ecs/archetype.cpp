#include "archetype.h"

namespace Vivium {
	namespace ECS {
		bool archetype_t::operator==(const archetype_t& other) const {
			return signature == other.signature;
		}

		bool archetype_t::operator!=(const archetype_t& other) const {
			return signature != other.signature;
		}

		void archetype_t::m_clear() {
			for (uint32_t i = 0; i < MAX_COMPONENTS; i++) {
				if (signature.enabled.test(i)) {
					arrays[i].components.clear();
				}
			}

			size = 0;
		}

		void archetype_t::remove_entity(entity_t& entity) {
			// Iterate enabled arrays
			for (uint32_t i = 0; i < MAX_COMPONENTS; i++) {
				if (signature.enabled.test(i)) {
					component_array_t& components = arrays[i].components;

					// Swap remove this entity from that array
					components.erase(entity.index);
				}
			}

			// Update this entity's data
			entity.index = INVALID_INDEX;
			entity.archetype = nullptr;

			--size;
		}
	}
}
