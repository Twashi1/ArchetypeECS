#include "component.h"

namespace Vivium {
	namespace ECS {
		void component_array_t::m_fit_to(uint32_t index) {
			// We already have enough space
			if (index < m_capacity) return;

			uint32_t three_halfs_factor = m_capacity + (m_capacity >> 1) + 1;

			// If the three halfs factor is enough
			if (three_halfs_factor > index) {
				reserve(three_halfs_factor);
			}
			// Force to fit exactly size required
			else {
				reserve(index + 1);
			}
		}

		void component_array_t::m_destroy_data()
		{
			if (m_data != nullptr) {
				m_manager.destroy_range(m_data, m_size);
				delete[] m_data;
				m_data = nullptr;
				
				m_size = 0;
				m_capacity = 0;
			}
		}

		component_array_t::component_array_t()
			: m_size(0), m_capacity(0), m_data(nullptr) {}

		component_array_t::~component_array_t()
		{
			m_destroy_data();
		}

		component_array_t::component_array_t(component_manager_t manager)
			: component_array_t()
		{
			m_manager = manager;
		}

		component_array_t::component_array_t(component_array_t&& other) noexcept
			: m_size(std::move(other.m_size)),
			m_capacity(std::move(other.m_capacity)),
			m_data(std::exchange(other.m_data, nullptr)),
			m_manager(std::move(other.m_manager))
		{}

		component_array_t& component_array_t::operator=(component_array_t&& other) noexcept {
			m_size = std::move(other.m_size);
			m_capacity = std::move(other.m_capacity);
			m_data = std::exchange(other.m_data, nullptr);
			m_manager = std::move(other.m_manager);

			return *this;
		}

		void component_array_t::clear_setup() {
			VIVIUM_ECS_ERROR(severity::DEBUG, "Deleting all data for array {}, size {}",
				(void*)this, m_size);

			m_destroy_data();

			m_manager = component_manager_t();
		}

		void component_array_t::clear() {
			if (m_data == nullptr) return;

			m_manager.destroy_range(
				m_data,
				m_size
			);

			m_size = 0;
		}

		void component_array_t::reserve(uint32_t new_capacity) {
			// Ignore if new capacity was less than our current
			if (new_capacity < m_capacity)
				VIVIUM_ECS_ERROR(
					severity::DEBUG,
					"New capacity was less than current capacity {} < {}",
					new_capacity, m_capacity
				);
			else {
				// Create bigger array
				uint8_t* new_data = new uint8_t[
					new_capacity * m_manager.size()
				];

				if (m_data != nullptr) {
					// Move all of old data into new array
					m_manager.move_range(m_data, new_data, m_size);

					// Delete old array
					delete[] m_data;
				}

				// Set m_data to the new array we created
				m_data = new_data;

				// Update capacity
				m_capacity = new_capacity;
			}
		}

		uint32_t component_array_t::size() const { return m_size; }

		bool component_array_t::is_empty() const { return m_size == 0; }

		bool component_array_t::within_bounds(uint32_t index) const
		{
			return index < m_size;
		}

		component_manager_t component_array_t::get_manager() const
		{
			return m_manager;
		}

		void component_array_t::transfer_index_to_end_of(uint32_t index, component_array_t& other) {
			// Force destination to have enough space
			other.m_fit_to(other.size());

			uint8_t* src = m_manager.at(m_data, index);
			uint8_t* dest = other.m_manager.at(other.m_data, other.size());

			// Perform move
			m_manager.move(src, dest);
			// Fill in the gap we made in ourselves
			m_manager.move(
				m_manager.at(m_data, m_size - 1),
				src
			);

			// Increment destinations size
			other.m_size++;
			// Decrement our size
			m_size--;
		}

		void component_array_t::pop_back() {
			if (is_empty())
				VIVIUM_ECS_ERROR(severity::ERROR, "Tried to pop empty array");
			else {
				m_manager.destroy(m_manager.at(m_data, --m_size));
			}
		}

		void component_array_t::erase(uint32_t index) {
			if (!within_bounds(index))
				VIVIUM_ECS_ERROR(severity::ERROR, "Tried to erase index that wasn't within bounds {} >= {}", index, m_size);
			else {
				// Swap remove component data
				m_manager.swap_remove(
					m_manager.at(m_data, index),		// this element gets deleted
					m_manager.at(m_data, m_size - 1)	// this element fills the slot
				);

				--m_size;
			}
		}

		component_manager_t::component_manager_t(const component_manager_t& other)
			: move(other.move),
			move_range(other.move_range),
			clone(other.clone),
			clone_range(other.clone_range),
			destroy(other.destroy),
			destroy_range(other.destroy_range),
			swap(other.swap),
			swap_remove(other.swap_remove),
			at(other.at),
			size(other.size)
		{}

		component_manager_t& component_manager_t::operator=(const component_manager_t& other)
		{
			move = other.move;
			move_range = other.move_range;
			clone = other.clone;
			clone_range = other.clone_range;
			destroy = other.destroy;
			destroy_range = other.destroy_range;
			swap = other.swap;
			swap_remove = other.swap_remove;
			at = other.at;
			size = other.size;

			return *this;
		}

		component_manager_t::component_manager_t()
			: move(nullptr),
			move_range(nullptr),
			clone(nullptr),
			clone_range(nullptr),
			destroy(nullptr),
			destroy_range(nullptr),
			swap(nullptr),
			swap_remove(nullptr),
			at(nullptr),
			size(nullptr) {}
		
		component_manager_t::component_manager_t(component_manager_t&& other) noexcept
			: move(std::move(other.move)),
			move_range(std::move(other.move_range)),
			clone(std::move(other.clone)),
			clone_range(std::move(other.clone_range)),
			destroy(std::move(other.destroy)),
			destroy_range(std::move(other.destroy_range)),
			swap(std::move(other.swap)),
			swap_remove(std::move(other.swap_remove)),
			at(std::move(other.at)),
			size(std::move(other.size))
		{}

		component_manager_t& component_manager_t::operator=(component_manager_t&& other) noexcept
		{
			move			= std::move(other.move);
			move_range		= std::move(other.move_range);
			clone			= std::move(other.clone);
			clone_range		= std::move(other.clone_range);
			destroy			= std::move(other.destroy);
			destroy_range	= std::move(other.destroy_range);
			swap			= std::move(other.swap);
			swap_remove		= std::move(other.swap_remove);
			at				= std::move(other.at);
			size			= std::move(other.size);

			return *this;
		}
	}
}