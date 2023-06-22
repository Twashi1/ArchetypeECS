#pragma once

#include "archetype.h"

namespace Vivium {
	namespace ECS {
		template <typename... Ts>
		struct archetype_t::iterator {
		private:
			archetype_t* m_archetype;
			registry_id_t m_registry;
			uint32_t m_index;

			std::vector<component_array_t*> m_components;

			iterator(archetype_t* archetype, registry_id_t registry, uint32_t index = 0)
				: m_archetype(archetype), m_registry(registry), m_index(index)
			{
				if (m_archetype == nullptr)
					VIVIUM_ECS_ERROR(severity::FATAL, "Can't iterate a null archetype");
				if (index != m_archetype->size)
				{
					([&]() {
						component_id_t component_id = component_registry<Ts>::get_id(m_registry);

						m_components.push_back(
							&(m_archetype->arrays[component_id].components)
						);
						}(), ...);
				}
			}

			friend archetype_t;

		public:
			using iterator_category = std::forward_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = std::tuple<Ts&...>;
			using pointer = value_type*;
			using reference = value_type&;

			reference operator*() const {
				uint32_t index = 0;

				return std::make_tuple<Ts&...>(
					m_components[index++]->at<Ts>(m_index)...
				);
			}

			template <typename T>
			T& get() {
				uint32_t index = 0;

				T* component = nullptr;

				([&]() {
					if constexpr (std::is_same_v<T, Ts>) {
						component = &m_components[index]->at<T>(m_index);

						return;
					}

					++index;
				}(), ...);

				if (component == nullptr) {
					VIVIUM_ECS_ERROR(severity::FATAL, "Type {} was not in archetype", typeid(T).name());
				}

				return *component;
			}

			// TODO: something about this
			pointer operator->() = delete;

			iterator& operator++() { ++m_index; return *this; }
			iterator& operator++(int) { iterator tmp = *this; ++(*this); return tmp; }

			bool operator==(const iterator& other) const {
				return m_archetype == other.m_archetype &&
					m_index == other.m_index && m_registry == other.m_registry;
			}

			bool operator!=(const iterator& other) const {
				return m_archetype != other.m_archetype ||
					m_index != other.m_index || m_registry != other.m_registry;
			}
		};
	}
}