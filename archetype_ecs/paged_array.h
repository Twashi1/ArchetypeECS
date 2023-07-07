#pragma once

#include <array>
#include <cstdint>
#include <set>

#include "error_handler.h"

namespace Vivium {
	namespace ECS {
		template <typename T, uint32_t max_size, uint32_t page_size, T null_value>
		struct paged_array_t {
		private:
			struct page_t {
				uint32_t start_index;
				uint32_t size;			// Amount of elements stored
				std::array<T, page_size> data;
				
				page_t(uint32_t start_index)
					: start_index(start_index), size(0) {}
				page_t()
					: start_index(UINT_MAX), size(UINT_MAX) {}

				page_t(const page_t&) = default;
				page_t& operator=(const page_t&) = default;

				page_t(page_t&&) noexcept = default;
				page_t& operator=(page_t&&) noexcept = default;

				bool operator<(const page_t& other) const { return start_index < other.start_index; }
				bool operator==(const page_t& other) const { return start_index == other.start_index; }
			
				bool operator<(const uint32_t& other) const { return start_index < other; }
			};

			std::vector<page_t> m_pages;

			uint32_t m_get_page_index_linear(uint32_t start_index) {
				for (uint32_t page_index = 0; page_index < m_pages.size(); page_index++) {
					if (m_pages[page_index].start_index == start_index)
						return page_index;
				}

				return UINT_MAX;
			}

			// Returns index of new page
			uint32_t m_make_page(uint32_t start_index) {
				std::vector<page_t> new_pages;
				new_pages.resize(m_pages.size() + 1);

				uint32_t has_moved_new_page = 0;
				uint32_t new_page_index = UINT_MAX;

				for (uint32_t page_index = 0; page_index < m_pages.size(); page_index++) {
					if (m_pages[page_index].start_index > start_index && !has_moved_new_page) {
						new_pages[page_index] = page_t(start_index);
						
						has_moved_new_page = 1;

						new_page_index = page_index;
					}

					new_pages[page_index + has_moved_new_page] = std::move(m_pages[page_index]);
				}

				// If we haven't moved a new page, create it at the end
				if (!has_moved_new_page) {
					new_page_index = new_pages.size() - 1;

					new_pages[new_page_index] = page_t(start_index);
				}

				m_pages = std::move(new_pages);

				return new_page_index;
			}

			uint32_t m_get_page_index(uint32_t start_index) {
				// TODO: should throw error
				if (m_pages.empty())		return UINT_MAX;
				if (m_pages.size() < 10)	return m_get_page_index_linear(start_index);

				uint32_t high = m_pages.size() - 1;
				// TODO: could approximate page index using
				// start index and page size
				uint32_t mid = high / 2;
				uint32_t lower = 0;
				uint32_t search_range = high - lower + 1;

				const uint32_t MAX_ITERATION_LIMIT = 500;
				uint32_t iterations = 0;

				while (search_range > 0 && iterations < MAX_ITERATION_LIMIT) {
					if (m_pages[mid].start_index == start_index)
						return mid;

					// Only want the lower bound
					if (m_pages[mid].start_index > start_index)
						high = mid - 1;

					// Only want the upper bound
					if (m_pages[mid].start_index < start_index)
						lower = mid + 1;

					search_range = high - lower + 1;
					mid = (high + lower) / 2;

					++iterations;
				}	

				if (search_range == 0) {
					return UINT_MAX;
				}

				// Must've exited from hitting max iterations
				VIVIUM_ECS_ERROR(FATAL, "Hit max iteration limit when getting page index");
				
				return UINT_MAX;
			}

		public:
			void clear() {
				m_pages.clear();
			}

			// Shrink to necessary size
			void shrink_to_size() {
				for (auto it = m_pages.begin(); it != m_pages.end();) {
					if (it->size <= 0) {
						it = m_pages.erase(it);
					}
					else {
						++it;
					}
				}
			}

			void push(uint32_t index, const T& value) {
				uint32_t start_index = index / page_size * page_size;
				uint32_t page_index = m_get_page_index(start_index);

				if (page_index == UINT_MAX) {
					page_index = m_make_page(start_index);
				}

				page_t& page = m_pages[page_index];

				page.data[index - start_index] = value;
				page.size++;
			}

			void pop(uint32_t index) {
				uint32_t start_index = index / page_size * page_size;
				uint32_t page_index = m_get_page_index(start_index);

				if (page_index == UINT_MAX) {
					VIVIUM_ECS_ERROR(FATAL, "Attempted to remove from page that didn't exist");
				}

				page_t& page = m_pages[page_index];

				page.data[index - start_index] = null_value;
				page.size--;

				// Delete the page now
				if (page.size == 0) {
					m_pages.erase(m_pages.begin() + page_index);
				}
			}

			const T& at(uint32_t index) const {
				uint32_t start_index = index / page_size * page_size;
				uint32_t page_index = m_get_page_index(start_index);

				if (page_index == UINT_MAX) {
					return null_value;
				}

				page_t& page = m_pages[page_index];

				return page.data[index - start_index];
			}

			T& at(uint32_t index) {
				uint32_t start_index = index / page_size * page_size;
				uint32_t page_index = m_get_page_index(start_index);

				if (page_index == UINT_MAX) {
					VIVIUM_ECS_ERROR(FATAL, "Attempted to access element that didn't exist with at");
				}

				page_t& page = m_pages[page_index];

				return page.data[index - start_index];
			}
		};
	}
}