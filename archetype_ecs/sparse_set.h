#pragma once

#include "error_handler.h"
#include "paged_array.h"
#include "constants.h"

namespace Vivium {
	namespace ECS {
		template <typename T>
		concept is_valid_sparse_key = std::is_integral_v<T>;

		template <typename value_t, is_valid_sparse_key key_t, typename func_t, func_t key_func, uint32_t max_size, uint32_t page_size, key_t null_value>
		struct sparse_set_t {
		private:
			std::vector<value_t> m_dense_array;
			paged_array_t<key_t, max_size, page_size, null_value> m_sparse_array;

		public:
			uint32_t get_index_of(const key_t& key) {
				const uint32_t index = m_sparse_array.at(key);

				if (index == null_value) {
					VIVIUM_ECS_ERROR(severity::FATAL, "Key was invalid, got null value from sparse array");
				}

				return index;
			}

			uint32_t size() const { return m_dense_array.size(); }

			value_t& at(const key_t& key) {
				return m_dense_array[get_index_of(key)];
			}

			const value_t& at(const key_t& key) const {
				return m_dense_array[get_index_of(key)];
			}

			// Return index of element
			void push(const value_t& element) {
				uint32_t index = m_dense_array.size();
				m_dense_array.push_back(element);

				m_sparse_array.push(key_func(element), index);
			}

			void remove(const value_t& element) {
				erase(key_func(element));
			}

			void erase(const key_t& element_key) {
				const value_t& last_element = m_dense_array.back();

				key_t last_key = key_func(last_element);

				uint32_t last_index = get_index_of(last_key);
				uint32_t element_index = get_index_of(element_key);

				// Swap in dense array
				std::swap(m_dense_array[last_index], m_dense_array[element_index]);
				m_dense_array.pop_back();

				key_t& last_element_sparse = m_sparse_array.at(last_key);
				key_t& current_element_sparse = m_sparse_array.at(element_key);

				std::swap(last_element_sparse, current_element_sparse);

				// Remove element at index
				m_sparse_array.pop(last_key);
			}
		};
	}
}