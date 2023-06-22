#pragma once

#include "component_registry.h"
#include "id_generator.h"
#include "signature.h"
#include "entity.h"
#include "sparse_set.h"

#include "archetype.h"

#include <optional>

namespace Vivium {
	namespace ECS {
		struct archetype_t;
		
		// TODO: missing lots of safety checks on if entity already has component

		struct registry_t {
		private:
			static entity_value_t m_entity_id_getter(const entity_t& entity);

			static id_generator<registry_id_t, MAX_REGISTRIES, REGISTRY_NULL_ID> m_registry_gen;

			std::unordered_map<signature_t, archetype_t> m_archetypes;
			id_generator<component_id_t, MAX_COMPONENTS, COMPONENT_NULL_ID> m_component_gen;

			id_generator<entity_value_t, MAX_ENTITIES, ENTITY_NULL_ID> m_entity_gen;

			sparse_set_t<entity_t, entity_value_t, decltype(m_entity_id_getter),
				m_entity_id_getter, MAX_ENTITIES, SPARSE_PAGE_SIZE, ENTITY_NULL> m_entity_sparse;

			registry_id_t m_id;

			archetype_t* m_get_archetype(signature_t signature);

			template <typename... Ts>
			archetype_t* m_extend_archetype(const archetype_t& old_archetype);

			template <typename... Ts>
			archetype_t* m_shrink_archetype(const archetype_t& old_archetype);

			template <typename... Ts>
			archetype_t* m_create_archetype();

			template <typename... Ts>
			archetype_t* m_get_or_create_archetype();

		public:
			friend archetype_t;

			registry_t();
			~registry_t();

			[[nodiscard]] entity_value_t get_entity();
			void free_entity(entity_value_t entity);

			// Clear entity of all components
			void clear_entity(entity_value_t entity);

			template <typename T>
			void push_component(entity_value_t entity_id, const T& component);

			template <typename T>
			void remove_component(entity_value_t entity_id);

			template <typename T>
			T& get_component(entity_value_t entity_id);

			template <typename T>
			const T& get_component(entity_value_t entity_id) const;

			// TODO: multi-push
			// TODO: emplace?

			template <typename... Ts>
			archetype_t::iterator<Ts...> begin();

			template <typename... Ts>
			archetype_t::iterator<Ts...> end();

			template <typename T>
			void register_component() {
				// Ensure component not already registered
				if (component_registry<T>::get_id(m_id) != COMPONENT_NULL_ID)
					VIVIUM_ECS_ERROR(severity::ERROR, "Attempted to re-register component {}", typeid(T).name());
				else {
					component_registry<T>::register_component(m_id, m_component_gen.get());
				}
			}
		};
	}
}