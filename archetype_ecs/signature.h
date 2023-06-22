#pragma once

#include "constants.h"
#include "component_registry.h"

#include <bitset>

namespace Vivium {
	namespace ECS {
		struct signature_t {
			std::bitset<MAX_COMPONENTS> enabled;

			signature_t();

			bool operator==(const signature_t& other) const;
			bool operator!=(const signature_t& other) const;

			template <typename... Ts>
			void setup(registry_id_t registry) {
				([&]() {
					component_id_t component = component_registry<Ts>::get_id(registry);

					enabled.set(component, true);
				} (), ... );
			}
		};
	}
}

namespace std {
	template <> struct hash<Vivium::ECS::signature_t>
	{
		size_t operator()(const Vivium::ECS::signature_t& signature) const {
			return hash<decltype(signature.enabled)>()(signature.enabled);
		}
	};
}