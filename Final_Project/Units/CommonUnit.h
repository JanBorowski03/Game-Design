#ifndef COMMON_UNIT_H
#define COMMON_UNIT_H

#include "Unit.h"

class CommonUnit : public Unit
{
public:
    CommonUnit(
        Vector2 position,
        Vector2 scale = DEFAULT_SCALE
    ) : Unit(
            position,
            scale,
            "Ranger",
            "assets/units/ranger.png",
            "assets/units/ranger portrait.png",
            UNIT_TIER_1,
            100,
            10,
            0,
            1.0f,
            1.0f,
            1
        )
    {
    }
};

#endif
