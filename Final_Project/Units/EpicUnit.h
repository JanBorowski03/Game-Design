#ifndef EPIC_UNIT_H
#define EPIC_UNIT_H

#include "Unit.h"

class EpicUnit : public Unit
{
public:
    EpicUnit(
        Vector2 position,
        Vector2 scale = DEFAULT_SCALE
    ) : Unit(
            position,
            scale,
            "Paladin",
            "assets/units/paladin.png",
            "assets/units/paladin portrait.png",
            UNIT_TIER_3,
            150,
            20,
            2,
            1.0f,
            1.0f,
            3
        )
    {
    }
};

#endif
