#include <iostream>

#include "archetype_ecs.h"

using namespace Vivium::ECS;

void full_test() {
    registry_t registry;

    registry.register_component<int>();
    registry.register_component<float>();

    entity_value_t x = registry.get_entity();

    registry.push_component<int>(x, 5);

    registry.remove_component<int>(x);
}

int main() {
    full_test();
}