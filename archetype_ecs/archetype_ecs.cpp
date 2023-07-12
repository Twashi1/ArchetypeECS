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

    registry.push_components<int, float>(x, 5, 7.0f);

    entity_value_t y = registry.get_entity();

    registry.push_components<float, int>(y, 1.0f, 3);

    entity_value_t z = registry.get_entity();

    registry.push_component<int>(z, 4);

    registry.free_entity(z);

    auto begin = registry.begin<int, float>();
    auto end = registry.end<int, float>();

    while (begin != end) {
        std::cout << begin.get<int>() << ", " << begin.get<float>() << std::endl;
        
        ++begin;
    }
}

int main() {
    ecs_test();
}