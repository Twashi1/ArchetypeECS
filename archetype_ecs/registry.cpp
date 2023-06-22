#include "registry.h"
#include "archetype.h"

namespace Vivium {
	namespace ECS {
		id_generator<registry_id_t, MAX_REGISTRIES, REGISTRY_NULL_ID> registry_t::m_registry_gen;

		entity_value_t registry_t::m_entity_id_getter(const entity_t& entity) {
			return entity.value;
		}

		archetype_t* registry_t::m_get_archetype(signature_t signature) {
			auto it = m_archetypes.find(signature);

			// If found, return archetype
			if (it != m_archetypes.end()) {
				return &(it->second);
			}

			return nullptr;
		}

		registry_t::registry_t()
			: m_id(m_registry_gen.get())
		{}
		
		registry_t::~registry_t() {
			for (auto& [signature, archetype] : m_archetypes) {
				archetype.m_clear();
			}

			m_registry_gen.free(m_id);
		}

		entity_value_t registry_t::get_entity()
		{
			entity_t new_entity;
			// TODO: no complications with version number right now
			// TODO: implement version number in future
			new_entity.value = m_entity_gen.get();

			m_entity_sparse.push(new_entity);

			return new_entity.value;
		}

		void registry_t::free_entity(entity_value_t entity)
		{
			clear_entity(entity);

			m_entity_sparse.erase(entity);

			m_entity_gen.free(entity);
		}

		void registry_t::clear_entity(entity_value_t entity_id)
		{
			entity_t& entity = m_entity_sparse.at(entity_id);

			if (entity.archetype != nullptr)
				entity.archetype->remove_entity(entity);
			else
				VIVIUM_ECS_ERROR(severity::WARN, "Attempted to clear entity with no components");
		}
	}
}
