#pragma once

#include <array>
#include <cstdint>
#include <set>

namespace Vivium {
	namespace ECS {
		

		template <typename T, uint32_t max_size, uint32_t page_size, T null_value>
		struct paged_array_t {
			struct page {
				uint32_t start_index;
				std::array<T, page_size> data;
				uint32_t size; // Amount of elements stored
				
				page(uint32_t start_index)
					: start_index(start_index), size(0)
				{}

				bool operator<(const page& other) const { return start_index < other.start_index; }
				bool operator==(const page& other) const { return start_index == other.start_index; }
			
				bool operator<(const uint32_t& other) const { return start_index < other; }
			};

			std::set<page> pages;

			void clear() {
				pages.clear();
			}

			// Shrink to necessary size
			void shrink_to_size() {
				for (auto it = pages.begin(); it != pages.end();) {
					if (it->size <= 0) {
						it = pages.erase(it);
					}
					else {
						++it;
					}
				}
			}

			const T& at(uint32_t index) const {
				// Round index down to nearest page_size
				uint32_t start_index = index / page_size * page_size;
				// Look for start_index
				auto it = pages.find(start_index);

				// Found that page
				if (it != pages.end()) {
					return it->data[index - start_index];
				}
				
				// We can't create a new page, so return the null value
				return null_value;
			}

			T& at(uint32_t index, int delta = 0) {
				// Round index down to nearest page_size
				uint32_t start_index = index / page_size * page_size;
				// Look for start_index
				typename std::set<page>::iterator it = pages.find(start_index);

				// Found that page
				if (it != pages.end()) {
					const_cast<page*>(&*it)->size += delta;

					return const_cast<page*>(&*it)->data[index - start_index];
				}

				// Construct page and return data
				typename std::set<page>::iterator insert_it = pages.insert(page(start_index)).first;
				
				const_cast<page*>(&*insert_it)->size += delta;

				return const_cast<page*>(&*insert_it)->data[index - start_index];
			}
		};
	}
}