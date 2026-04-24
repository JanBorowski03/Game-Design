#ifndef LEGENDARY_UNIT_H
#define LEGENDARY_UNIT_H

#include "Unit.h"

class LegendaryUnit : public Unit
{
public:
    LegendaryUnit(
        Vector2 position,
        Vector2 scale = DEFAULT_SCALE
    ) : Unit(
            position,
            scale,
            "Mage",
            "assets/units/mage.png",
            "assets/units/mage portrait.png",
            UNIT_TIER_4,
            190,
            28,
            4,
            1.0f,
            1.0f,
            4
        )
    {
    }
};

#endif
