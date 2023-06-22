#include <iostream>

#include "archetype_ecs.h"

using namespace Vivium::ECS;

void full_test() {
    registry_t registry;

    registry.register_component<int>();
    registry.register_component<float>();

    entity_value_t x = registry.get_entity();

    registry.push_component<int>(x, 5);
    registry.push_component<float>(x, 7.4f);

    entity_value_t y = registry.get_entity();

    registry.push_component<int>(y, 4);
    registry.push_component<float>(y, 3.5f);

    auto it = registry.begin<int, float>();
    auto end = registry.end<int, float>();

    while (it != end) {
        std::cout << it.get<int>() << ", " << it.get<float>() << std::endl;
        
        ++it;
    }
}

int main() {
    full_test();
}