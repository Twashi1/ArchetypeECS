#pragma once

#include "registry.h"
#include "archetype.h"
#include "archetype_iterator.inl"

namespace Vivium {
	namespace ECS {
		template <typename... Ts>
		archetype_t::iterator<Ts...> archetype_t::begin(registry_id_t registry) { return iterator<Ts...>(this, registry, 0); }
		template <typename... Ts>
		archetype_t::iterator<Ts...> archetype_t::end(registry_id_t registry) { return iterator<Ts...>(this, registry, size); }

		template <typename T>
		void archetype_t::push_component(entity_t& entity, registry_t& registry, const T& component) {
			// Calculate/find new archetype for entity

			// Check if its in our add component vector
			component_id_t new_component_id = component_registry<T>::get_id(registry.m_id);

			archetype_t*& add_archetype = arrays[new_component_id].connections.add;

			// We didn't already have it, so find it in registry
			if (add_archetype == nullptr) {
				// Calculate signature
				signature_t new_signature = signature;
				new_signature.enabled.set(new_component_id, true);

				archetype_t* new_archetype = registry.m_get_archetype(new_signature);

				// Archetype was in registry at least
				if (new_archetype != nullptr) {
					// Update connection to point to this archetype
					add_archetype = new_archetype;
				}
				else {
					// Create this archetype
					add_archetype = registry.m_extend_archetype<T>(*this);
				}
			}

			// add_archetype now stores the new archetype
			// Copy old component data over, and add new component
			// First iterate enabled component arrays
			uint32_t array_size = 0;

			for (uint32_t i = 0; i < MAX_COMPONENTS; i++) {
				if (signature.enabled.test(i)) {
					arrays[i].components.transfer_index_to_end_of(
						entity.index,
						add_archetype->arrays[i].components
					);

					array_size = add_archetype->arrays[i].components.size();
				}
			}

			// Adding new component
			add_archetype->arrays[new_component_id].components.push_back(component);

			// Update entity to point to new archetype
			entity.index = array_size - 1;
			entity.archetype = add_archetype;

			// Decrement our size
			--size;
			// Increment size of the archetype that got added to
			++(add_archetype->size);
		}

		template<typename ...Ts>
		void archetype_t::push_components(entity_t& entity, registry_t& registry, const Ts&... components)
		{
			signature_t new_signature = signature;
			new_signature.extend<Ts...>(registry.m_id);

			archetype_t* new_archetype = registry.m_get_archetype(new_signature);

			if (new_archetype == nullptr) {
				new_archetype = registry.m_extend_archetype<Ts...>(*this);
			}

			uint32_t array_size = 0;

			// Move old components
			for (uint32_t i = 0; i < MAX_COMPONENTS; i++) {
				if (signature.enabled.test(i)) {
					arrays[i].components.transfer_index_to_end_of(
						entity.index,
						new_archetype->arrays[i].components
					);

					array_size = new_archetype->arrays[i].components.size();
				}
			}

			// Add new components
			([&]() {
				component_id_t component_id = component_registry<Ts>::get_id(registry.m_id);

				new_archetype->arrays[component_id].components.push_back(components);
			}(), ...);

			entity.index = array_size - 1;
			entity.archetype = new_archetype;

			--size;
			++(new_archetype->size);
		}

		template<typename T>
		void archetype_t::remove_component(entity_t& entity, registry_t& registry)
		{
			// Calculate/find new archetype for entity

			component_id_t new_component_id = component_registry<T>::get_id(registry.m_id);

			// Check if its in our remove component vector
			archetype_t*& rem_archetype = arrays[new_component_id].connections.remove;

			// We didn't have it cached, so find it in registry
			if (rem_archetype == nullptr) {
				// Calculate signature
				signature_t new_signature = signature;
				new_signature.enabled.set(new_component_id, false);

				archetype_t* new_archetype = registry.m_get_archetype(new_signature);

				// Archetype was in registry
				if (new_archetype != nullptr) {
					// Update connection to point to this archetype
					rem_archetype = new_archetype;
				}
				else {
					// Create this archetype
					rem_archetype = registry.m_shrink_archetype<T>(*this);
				}
			}

			// rem_archetype now stores the new archetype
			
			// Copy old component data over, except component to be removed
			// First iterate enabled component arrays
			uint32_t array_size = INVALID_INDEX;

			for (uint32_t i = 0; i < MAX_COMPONENTS; i++) {
				if (signature.enabled.test(i)) {
					// Archetype is null since all components were removed
					if (rem_archetype == nullptr) {
						// So just delete all the components which were
						// enabled in the old archetype
						arrays[new_component_id].components.erase(
							entity.index
						);
					}
					// Check if its the component to be removed
					else if (!rem_archetype->signature.enabled.test(i)) {
						// It was one of the components that we're removing,
						// since its in the old archetype but not the new one
						// So just remove that component from the array,
						// but don't transfer it over
						arrays[new_component_id].components.erase(
							entity.index
						);
					}
					else {
						arrays[i].components.transfer_index_to_end_of(
							entity.index,
							rem_archetype->arrays[i].components
						);

						array_size = rem_archetype->arrays[i].components.size();
					}
				}
			}

			// Update entity to point to new archetype
			entity.index = array_size - 1;
			entity.archetype = rem_archetype;

			// Decrement our size
			--size;
			// Increment size of the archetype that got added to
			++(rem_archetype->size);
		}

		template <typename T>
		T& archetype_t::get_component(const entity_t& entity, registry_id_t registry)
		{
			component_id_t component_id = component_registry<T>::get_id(registry);

			return arrays[component_id].components.at<T>(entity.index);
		}

		template <typename T>
		const T& archetype_t::get_component(const entity_t& entity, registry_id_t registry) const
		{
			component_id_t component_id = component_registry<T>::get_id(registry);

			return arrays[component_id].components.at<T>(entity.index);
		}
	}
}