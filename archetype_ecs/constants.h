#pragma once

#include <cstdint>
#define VIVIUM_ECS_MACROS_ENABLED

namespace Vivium {
	namespace ECS {
		using component_id_t = uint16_t;
		using entity_value_t = uint32_t;
		using registry_id_t	 = uint16_t;

		constexpr component_id_t COMPONENT_NULL_ID = 0xff;
		constexpr uint32_t MAX_COMPONENTS = COMPONENT_NULL_ID + 1;

		constexpr registry_id_t REGISTRY_NULL_ID = 0xffff;
		constexpr uint32_t MAX_REGISTRIES = REGISTRY_NULL_ID + 1;

		constexpr entity_value_t ENTITY_NULL  = 0xffffffff;

		constexpr entity_value_t ENTITY_NULL_ID	= 0x000fffff;
		constexpr uint32_t MAX_ENTITIES		= ENTITY_NULL_ID;
		constexpr uint32_t ENTITY_MASK_ID	= ENTITY_NULL_ID;
		constexpr uint32_t MAX_ENTITY_ID	= ENTITY_NULL_ID;

		constexpr entity_value_t ENTITY_NULL_VERSION = 0xfff00000;
		constexpr uint32_t ENTITY_MASK_VERSION	= ENTITY_NULL_VERSION;
		constexpr uint32_t MAX_ENTITY_VERSION	= ENTITY_NULL_VERSION;
		constexpr uint32_t ENTITY_SHIFT_VERSION = 5 * 4;

		constexpr uint32_t SPARSE_PAGE_SIZE = 1024;

		constexpr uint32_t INVALID_INDEX = 0xffffffff;
	}
}