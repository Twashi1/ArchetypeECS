#pragma once

#include "signature.h"
#include "component.h"
#include "entity.h"

#include <array>
#include <memory>
#include <tuple>

namespace Vivium {
	namespace ECS {
		struct archetype_t;
		struct registry_t;

		struct archetype_connections_t {
			archetype_t* add = nullptr;
			archetype_t* remove = nullptr;
		};

		struct archetype_t {
		private:
			// Corrupts all entity identifiers, but is only called when registry is deleted
			// so this doesn't matter
			void m_clear();

			friend registry_t;

		public:
			// Data stored per component in an archetype
			struct per_component_data_t {
				component_array_t			components;
				archetype_connections_t		connections;
			};

			template <typename... Ts>
			struct iterator;

			signature_t signature;
			std::array<per_component_data_t, MAX_COMPONENTS> arrays;
			uint32_t size = 0;

			archetype_t() = default;

			bool operator==(const archetype_t& other) const;
			bool operator!=(const archetype_t& other) const;

			template <typename... Ts>
			iterator<Ts...> begin(registry_id_t registry);
			template <typename... Ts>
			iterator<Ts...> end(registry_id_t registry);

			// Setup without knowing signature
			template <typename... Ts>
			void setup(registry_id_t registry) {
				signature_t _signature;
				_signature.setup<Ts>(registry);
				
				setup<Ts...>(registry, _signature);
			}

			// Setup if signature already known/computed
			template <typename... Ts>
			void setup(registry_id_t registry, signature_t _signature) {
				signature = _signature;

				([&]() {
					component_id_t component_id = component_registry<Ts>::get_id(registry);

					arrays[component_id].components.setup<Ts>();
				}(), ...);
			}

			void remove_entity(entity_t& entity);
			
			template <typename... component_ts>
			void push_entity(entity_t& entity, registry_id_t registry, const component_ts&... components) {
				if (sizeof...(components) == 0) {
					VIVIUM_ECS_ERROR(severity::ERROR, "Attempted to push entity with 0 components");
				}
				
				([&]() {
					component_id_t component_id = component_registry<component_ts>::get_id(registry);
					arrays[component_id].components.push_back(components);
				}(), ...);

				entity.index = size++;
				entity.archetype = this;
			}

			template <typename T>
			void push_component(entity_t& entity, registry_t& registry, const T& component);
		
			template <typename... Ts>
			void push_components(entity_t& entity, registry_t& registry, const Ts&... components);

			template <typename T>
			void remove_component(entity_t& entity, registry_t& registry);

			template <typename T>
			T& get_component(const entity_t& entity, registry_id_t registry);

			template <typename T>
			const T& get_component(const entity_t& entity, registry_id_t registry) const;
		};
	}
}