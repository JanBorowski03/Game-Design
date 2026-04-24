#ifndef SQUIREL_GATE_H
#define SQUIREL_GATE_H

#include "Faction.h"
#include "SquirelGateBuildings.h"
#include <memory>

class SquirelGate : public Faction
{
public:
    SquirelGate() : Faction("SquirelGate", 15)
    {
        std::vector<std::unique_ptr<Building>> &buildings = getBuildings();
        buildings.clear();
        buildings.reserve(15);

        buildings.push_back(std::unique_ptr<Building>(new GatehouseBuilding()));
        buildings.push_back(std::unique_ptr<Building>(new AcornVaultBuilding()));
        buildings.push_back(std::unique_ptr<Building>(new BarkBarracksBuilding()));
        buildings.push_back(std::unique_ptr<Building>(new SapperBurrowBuilding()));
        buildings.push_back(std::unique_ptr<Building>(new GliderRoostBuilding()));
        buildings.push_back(std::unique_ptr<Building>(new ResinWorkshopBuilding()));
        buildings.push_back(std::unique_ptr<Building>(new TwigArtilleryBuilding()));
        buildings.push_back(std::unique_ptr<Building>(new NutMarketBuilding()));
        buildings.push_back(std::unique_ptr<Building>(new ScoutPostBuilding()));
        buildings.push_back(std::unique_ptr<Building>(new CanopyLibraryBuilding()));
        buildings.push_back(std::unique_ptr<Building>(new HerbalDenBuilding()));
        buildings.push_back(std::unique_ptr<Building>(new MoonwatchTowerBuilding()));
        buildings.push_back(std::unique_ptr<Building>(new RootForgeBuilding()));
        buildings.push_back(std::unique_ptr<Building>(new ElderOaksCouncilBuilding()));
        buildings.push_back(std::unique_ptr<Building>(new GreatNestCitadelBuilding()));
    }
    virtual ~SquirelGate() = default;
};

#endif
