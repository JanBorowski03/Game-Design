#ifndef RARE_UNIT_H
#define RARE_UNIT_H

#include "Unit.h"

class RareUnit : public Unit
{
public:
    RareUnit(
        Vector2 position,
        Vector2 scale = DEFAULT_SCALE
    ) : Unit(
            position,
            scale,
            "Rouge",
            "assets/units/rouge.png",
            "assets/units/rouge portrait.png",
            UNIT_TIER_2,
            120,
            14,
            1,
            1.0f,
            1.0f,
            2
        )
    {
    }
};

#endif
