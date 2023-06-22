#pragma once

#include "id_generator.h"
#include "constants.h"
#include "error_handler.h"

#include <unordered_map>

namespace Vivium {
	namespace ECS {
		template <typename T>
		struct component_registry {
		private:
			static std::unordered_map<registry_id_t, component_id_t> m_registry_to_component;

		public:
			// Register a component for a specific registry, will invoke error if re-registered
			static void register_component(registry_id_t registry, component_id_t component) {
				if (m_registry_to_component.contains(registry)) {
					VIVIUM_ECS_ERROR(severity::ERROR, "Attempted to re-register component {}", typeid(T).name());
				}
				else {
					m_registry_to_component.insert({ registry, component });
				}
			}

			// Get component id for a given registry, returns null ID if doesn't exist
			static component_id_t get_id(registry_id_t registry) {
				auto it = m_registry_to_component.find(registry);
				
				if (it != m_registry_to_component.end()) {
					return it->second;
				}

				// Didn't find a component for that specific registry
				return COMPONENT_NULL_ID;
			}
		};

		// Instantiate
		template <typename T>
		std::unordered_map<registry_id_t, component_id_t> component_registry<T>::m_registry_to_component;
	}
}