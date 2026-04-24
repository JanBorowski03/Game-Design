#include "Unit.h"

void Unit::render()
{
    if (!isActive() || isDead()) return;
    Entity::render();
}
