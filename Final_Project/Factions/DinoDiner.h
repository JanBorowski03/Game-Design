#ifndef DINO_DINER_H
#define DINO_DINER_H

#include "Faction.h"

class DinoDiner : public Faction
{
public:
    DinoDiner() : Faction("DinoDiner") {}
    virtual ~DinoDiner() = default;
};

#endif
