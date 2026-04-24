#ifndef FACTION_H
#define FACTION_H

#include "Building.h"
#include <memory>
#include <string>
#include <vector>

class Faction
{
private:
    std::string mName;
    std::string mPortraitFilepath;
    std::string mGroundsFilepath;
    std::vector<std::unique_ptr<Building>> mBuildings;

public:
    explicit Faction(const std::string &name, int numberOfBuildings = 15)
        : mName(name),
          mPortraitFilepath("assets/factions/" + name + "/Portrait.png"),
          mGroundsFilepath("assets/factions/" + name + "/Grounds.png")
    {
        const int count = numberOfBuildings > 0 ? numberOfBuildings : 0;
        mBuildings.reserve(static_cast<size_t>(count));
        for (int i = 0; i < count; i++)
        {
            const std::string buildingName = "Building " + std::to_string(i + 1);

            std::unique_ptr<Building> building(new Building(buildingName, "", 5 + i));
            if (i > 0) building->addRequirement("Building " + std::to_string(i));
            mBuildings.push_back(std::move(building));
        }
    }
    virtual ~Faction() = default;

    virtual const std::string &getName() const { return mName; }
    virtual const std::string &getPortraitFilepath() const { return mPortraitFilepath; }
    virtual const std::string &getGroundsFilepath() const { return mGroundsFilepath; }
    virtual int getNumberOfBuildings() const { return static_cast<int>(mBuildings.size()); }
    virtual const std::vector<std::unique_ptr<Building>> &getBuildings() const { return mBuildings; }
    virtual std::vector<std::unique_ptr<Building>> &getBuildings() { return mBuildings; }
    virtual std::string getBuildingPictureFilepath(int buildingIndex) const
    {
        if (buildingIndex < 0 || buildingIndex >= static_cast<int>(mBuildings.size())) return "";
        if (!mBuildings[buildingIndex]) return "";
        return mBuildings[buildingIndex]->getPictureFilepath(mName);
    }
};

#endif
