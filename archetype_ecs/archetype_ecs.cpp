#include <iostream>

#include "archetype_ecs.h"

using namespace Vivium::ECS;

// TODO: swap entity_t and entity_value_t
//  maybe to like entity_full_t, entity_t

void page_array_test() {
    paged_array_t<uint8_t, 100, 3, 999> my_array;

    my_array.push(0, 0xa);
    my_array.push(1, 0xb);
    my_array.push(2, 0xc);

    my_array.push(3, 0xd);
    my_array.push(4, 0xe);
    my_array.push(5, 0xf);

    my_array.pop(0);
    my_array.pop(1);
    my_array.pop(2);

    my_array.push(7, 0x1);
    my_array.push(0, 0xb);
}

void ecs_test() {
    registry_t registry;

    registry.register_component<int>();
    registry.register_component<float>();

    entity_value_t x = registry.get_entity();

    registry.push_component<int>(x, 5);
    registry.push_component<float>(x, 7.0f);

    registry.remove_component<float>(x);

    entity_value_t y = registry.get_entity();

    registry.push_component<float>(y, 1.0f);
    registry.push_component<int>(y, 3);

    registry.remove_component<float>(y);

    entity_value_t z = registry.get_entity();

    registry.push_component<int>(z, 4);

    registry.free_entity(z);

    auto begin = registry.begin<int>();
    auto end = registry.end<int>();

    while (begin != end) {
        std::cout << begin.get<int>() << std::endl;
        
        ++begin;
    }
}

int main() {
    ecs_test();
}