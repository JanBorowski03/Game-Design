#ifndef LOON_LAKE_H
#define LOON_LAKE_H

#include "Faction.h"

class LoonLake : public Faction
{
public:
    LoonLake() : Faction("LoonLake") {}
    virtual ~LoonLake() = default;
};

#endif
