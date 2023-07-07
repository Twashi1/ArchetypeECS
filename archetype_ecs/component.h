#pragma once

#include "error_handler.h"

namespace Vivium {
	namespace ECS {
		// All functions assume destination is unallocated memory
		// Move will destroy the source, maintaining total number of instances
		// Clone will simply copy, creating new instance
		template <typename T>
		struct component_manager_definitions {
			static void move(uint8_t* src, uint8_t* dest) {
				if constexpr (std::is_trivial_v<T>) {
					std::memcpy(dest, src, sizeof(T));
				}
				else if constexpr (std::is_move_constructible_v<T>) {
					new (dest) T(std::move(*reinterpret_cast<T*>(src)));
				}
				else if constexpr (std::is_copy_constructible_v<T>) {
					new (dest) T(*reinterpret_cast<const T*>(src));
					destroy(src);
				}
				else {
					VIVIUM_ECS_ERROR(severity::ERROR, "No method to move type {}", typeid(T).name());
				}
			}

			static void move_range(uint8_t* src, uint8_t* dest, uint32_t count) {
				if constexpr (std::is_trivial_v<T>) {
					memcpy(dest, src, sizeof(T) * count);
				}
				else {
					for (uint32_t i = 0; i < count; i++) {
						move(&dest[i * sizeof(T)], &src[i * sizeof(T)]);
					}
				}
			}

			static void clone(uint8_t* src, uint8_t* dest) {
				if constexpr (std::is_trivial_v<T>) {
					std::memcpy(dest, src, sizeof(T));
				}
				else if constexpr (std::is_copy_constructible_v<T>) {
					new (dest) T(*reinterpret_cast<const T*>(src));
				}
				else {
					VIVIUM_ECS_ERROR(severity::ERROR, "No method to clone type {}", typeid(T).name());
				}
			}

			static void clone_range(uint8_t* src, uint8_t* dest, uint32_t count) {
				if constexpr (std::is_trivial_v<T>) {
					std::memcpy(dest, src, sizeof(T) * count);
				}
				else {
					for (uint32_t i = 0; i < count; i++) {
						clone(&dest[i * sizeof(T)], &src[i * sizeof(T)]);
					}
				}
			}

			static void destroy(uint8_t* location) {
				if constexpr (!std::is_trivial_v<T>) {
					reinterpret_cast<T*>(location)->~T();
				}
			}

			static void destroy_range(uint8_t* location, uint32_t count) {
				if constexpr (!std::is_trivial_v<T>) {
					for (uint32_t i = 0; i < count; i++) {
						destroy(&location[i * sizeof(T)]);
					}
				}
			}

			static void swap(uint8_t* a, uint8_t* b) {
				// Location to temporarily store
				uint8_t* tmp = new uint8_t[sizeof(T)];

				move(a, tmp);
				move(b, a);
				move(tmp, b);

				delete[] tmp;
			}

			static void swap_remove(uint8_t* remove, uint8_t* replacement) {
				// Destroy element at remove
				destroy(remove);
				// Move replacement to location (this deconstructs replacement)
				move(replacement, remove);
			}

			static uint8_t* at(uint8_t* src, uint32_t index) {
				return &src[index * sizeof(T)];
			}

			static uint32_t size() {
				return sizeof(T);
			}
		};

		struct component_manager_t {
			typedef void (*move_t)(uint8_t* src, uint8_t* dest);
			typedef void (*move_range_t)(uint8_t* src, uint8_t* dest, uint32_t count);

			typedef void (*clone_t)(uint8_t* src, uint8_t* dest);
			typedef void (*clone_range_t)(uint8_t* src, uint8_t* dest, uint32_t count);

			typedef void (*destroy_t)(uint8_t* location);
			typedef void (*destroy_range_t)(uint8_t* location, uint32_t count);

			typedef void (*swap_t)(uint8_t* a, uint8_t* b);
			typedef void (*swap_remove_t)(uint8_t* remove, uint8_t* replacement);

			typedef uint8_t* (*at_t)(uint8_t* src, uint32_t index);
			typedef uint32_t(*get_size_t)();

			move_t move;
			move_range_t move_range;

			clone_t clone;
			clone_range_t clone_range;

			destroy_t destroy;
			destroy_range_t destroy_range;

			swap_t swap;
			swap_remove_t swap_remove;

			at_t at;
			get_size_t size;

			component_manager_t();

			component_manager_t(const component_manager_t& other);
			component_manager_t& operator=(const component_manager_t& other);

			component_manager_t(component_manager_t&& other) noexcept;
			component_manager_t& operator=(component_manager_t&& other) noexcept;

			template<typename T, typename... Args>
			void create(uint8_t* src, uint32_t index, Args&&... args) const {
				new (at(src, index)) T(std::forward<Args>(args)...);
			}

			template <typename T>
			T& value_at(uint8_t* src, uint32_t index) const {
				return *reinterpret_cast<T*>(at(src, index));
			}

			template <typename T>
			void setup() {
				move = component_manager_definitions<T>::move;
				move_range = component_manager_definitions<T>::move_range;

				clone = component_manager_definitions<T>::clone;
				clone_range = component_manager_definitions<T>::clone_range;

				destroy = component_manager_definitions<T>::destroy;
				destroy_range = component_manager_definitions<T>::destroy_range;

				swap = component_manager_definitions<T>::swap;
				swap_remove = component_manager_definitions<T>::swap_remove;
				at = component_manager_definitions<T>::at;
				size = component_manager_definitions<T>::size;
			}
		};

		struct component_array_t {
		private:
			uint32_t m_size;
			uint32_t m_capacity;

			// Component data
			uint8_t* m_data;

			component_manager_t m_manager;

			void m_fit_to(uint32_t index);
			void m_destroy_data();

		public:
			component_array_t();
			~component_array_t();

			component_array_t(component_manager_t manager);

			component_array_t(component_array_t&& other) noexcept;
			component_array_t& operator=(component_array_t&& other) noexcept;

			component_array_t(const component_array_t&) = delete;
			component_array_t& operator=(const component_array_t&) = delete;

			// Setup the manager with the type this array will be using
			template <typename T>
			void setup() {
				VIVIUM_ECS_ERROR(severity::DEBUG, "Deleting all data for array {}, size {}",
					(void*)this, m_size);

				m_destroy_data();

				m_manager.setup<T>();
			}

			// Clear manager
			void clear_setup();
			// Clear all components
			void clear();

			void reserve(uint32_t new_capacity);

			uint32_t size() const;
			bool is_empty() const;
			bool within_bounds(uint32_t index) const;
			component_manager_t get_manager() const;

			void transfer_index_to_end_of(uint32_t index, component_array_t& other);

			// Assuming element at index is just uninitialised memory
			template <typename T>
			void construct_at(const T& element, uint32_t index) {
				m_manager.create<T>(m_data, index, element);
				m_handles[index].m_internal = new handle_internal_t(
					m_manager.at(m_data, index)
				);
			}

			template <typename T>
			void replace_at(const T& element, uint32_t index) {
				// Destroy element
				m_manager.destroy(m_manager.at(m_data, index));
				// Construct at location
				construct_at<T>(element, index);
				
				// TODO: should give indication in handle that component was deleted,
				// then create new handle, thus we need refcounted handles/version numbered
			}

			template <typename T>
			void push_back(const T& element) {
				// Make array fit another element at least
				m_fit_to(m_size);
				// Construct element at end of array
				construct_at<T>(element, m_size++);
			}

			template <typename T, typename... Args>
			void emplace_back(Args&&... args) {
				// Make array fit another element at least
				m_fit_to(m_size);
				// Construct element at end of array
				m_manager.create<T>(m_data, m_size++, std::forward<Args&&>(args)...);
			}

			void pop_back();
			void erase(uint32_t index);

			template <typename T>
			T& at(uint32_t index) {
				if (!within_bounds(index))
					VIVIUM_ECS_ERROR(severity::ERROR, "Attempted to access OOB element");
				else {
					return m_manager.value_at<T>(m_data, index);
				}
			}

			template <typename T>
			const T& at(uint32_t index) const {
				if (!within_bounds(index))
					VIVIUM_ECS_ERROR(severity::ERROR, "Attempted to access OOB element");
				else {
					return m_manager.value_at<T>(m_data, index);
				}
			}
		};
	}
}