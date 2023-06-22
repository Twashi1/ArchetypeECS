#pragma once

#include "archetype.h"
#include "registry.h"

#include "archetype_iterator.inl"

namespace Vivium {
	namespace ECS {
		template <typename... Ts>
		archetype_t* registry_t::m_extend_archetype(const archetype_t& old_archetype) {
			archetype_t new_archetype;

			// TODO: should only iterate 1s in old signature
			for (uint32_t i = 0; i < MAX_COMPONENTS; i++) {
				new_archetype.arrays[i].components = component_array_t(
					old_archetype.arrays[i].components.get_manager()
				);
			}

			signature_t new_signature = old_archetype.signature;

			([&]() {
				component_id_t component_id = component_registry<Ts>::get_id(m_id);

				new_signature.enabled.set(component_id, true);

				new_archetype.arrays[component_id].components.setup<Ts>();
				}(), ...);

			new_archetype.signature = new_signature;

			// Add archetype to our map
			auto cond_pair = m_archetypes.insert({ new_signature, std::move(new_archetype) });
			archetype_t& archetype_in_map = cond_pair.first->second;

			return &archetype_in_map;
		}

		template<typename ...Ts>
		archetype_t* registry_t::m_shrink_archetype(const archetype_t& old_archetype)
		{
			archetype_t new_archetype;

			// TODO: should only iterate 1s in old signature
			for (uint32_t i = 0; i < MAX_COMPONENTS; i++) {
				new_archetype.arrays[i].components = component_array_t(
					old_archetype.arrays[i].components.get_manager()
				);
			}

			signature_t new_signature = old_archetype.signature;

			([&]() {
				component_id_t component_id = component_registry<Ts>::get_id(m_id);

				new_signature.enabled.set(component_id, false);

				new_archetype.arrays[component_id].components.clear_setup();
			}(), ...);

			new_archetype.signature = new_signature;

			// It's an empty archetype
			if (new_archetype.signature.enabled.count() == 0) {
				return nullptr;
			}

			// Add archetype to our map
			auto cond_pair = m_archetypes.insert({ new_signature, std::move(new_archetype) });
			archetype_t& archetype_in_map = cond_pair.first->second;

			return &archetype_in_map;
		}

		template <typename... Ts>
		archetype_t* registry_t::m_get_or_create_archetype() {
			// Compute signature
			signature_t signature;
			signature.setup<Ts...>(m_id);

			// Attempt to find
			archetype_t* archetype = m_get_archetype(signature);

			if (archetype == nullptr) {
				return m_create_archetype<Ts...>();
			}
			
			return archetype;
		}

		template <typename... Ts>
		archetype_t* registry_t::m_create_archetype() {
			// Compute signature
			signature_t signature;
			signature.setup<Ts...>(m_id);

			// Ensure archetype not already existing
			if (m_archetypes.contains(signature)) {
				VIVIUM_ECS_ERROR(severity::ERROR, "Attempted to create archetype for signature that already had an archetype");

				return nullptr;
			}

			// Create new archetype with that setup
			auto cond_pair = m_archetypes.insert({ signature, archetype_t() });
			archetype_t& new_archetype = cond_pair.first->second;

			new_archetype.setup<Ts...>(m_id, signature);

			return &new_archetype;
		}

		template <typename T>
		void registry_t::push_component(entity_value_t entity_id, const T& component) {
			entity_t& entity = m_entity_sparse.at(entity_id);

			// Get current archetype of this entity
			archetype_t* current_archetype = entity.archetype;

			if (current_archetype != nullptr) {
				current_archetype->push_component<T>(entity, *this, component);
			}
			else {
				// Get/create new archetype
				current_archetype = m_get_or_create_archetype<T>();
				current_archetype->push_entity<T>(entity, m_id, component);
			}
		}

		template<typename T>
		void registry_t::remove_component(entity_value_t entity_id)
		{
			entity_t& entity = m_entity_sparse.at(entity_id);

			// Get current archetype of this entity
			archetype_t* current_archetype = entity.archetype;

			if (current_archetype != nullptr) {
				current_archetype->remove_component<T>(entity, *this);
			}
			else
				VIVIUM_ECS_ERROR(severity::ERROR, "Attempted to remove component from entity with no components");
		}

		template<typename T>
		T& registry_t::get_component(entity_value_t entity_id)
		{
			const entity_t& entity = m_entity_sparse.at(entity_id);

			if (entity.archetype != nullptr) {
				return entity.archetype->get_component<T>(entity, m_id);
			}

			VIVIUM_ECS_ERROR(severity::FATAL, "Attempted to get component from an entity with no components");
		}
		
		template<typename T>
		const T& registry_t::get_component(entity_value_t entity_id) const
		{
			const entity_t& entity = m_entity_sparse.at(entity_id);

			if (entity.archetype != nullptr) {
				return entity.archetype->get_component<T>(entity, m_id);
			}

			VIVIUM_ECS_ERROR(severity::FATAL, "Attempted to get component from an entity with no components");
		}

		template <typename... Ts>
		archetype_t::iterator<Ts...> registry_t::begin() {
			// Find archetype
			signature_t signature;
			signature.setup<Ts...>(m_id);

			archetype_t* archetype = m_get_archetype(signature);

			if (archetype == nullptr) {
				VIVIUM_ECS_ERROR(severity::FATAL, "Attempted to iterate types that don't have associated archetype created");
			}
			else {
				return archetype->begin<Ts...>(m_id);
			}
		}

		template <typename... Ts>
		archetype_t::iterator<Ts...> registry_t::end() {
			// Find archetype
			signature_t signature;
			signature.setup<Ts...>(m_id);

			archetype_t* archetype = m_get_archetype(signature);

			if (archetype == nullptr) {
				VIVIUM_ECS_ERROR(severity::FATAL, "Attempted to iterate types that don't have associated archetype created");
			}
			else {
				return archetype->end<Ts...>(m_id);
			}
		}
	}
}