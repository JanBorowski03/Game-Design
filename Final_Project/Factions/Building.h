#ifndef BUILDING_H
#define BUILDING_H

#include <string>
#include <vector>

class LocalGameState;
class Unit;

class Building
{
private:
    std::string mName;
    std::string mDescription;
    int mCost = 0;
    bool mIsBuilt = false;
    std::vector<std::string> mRequirements;

public:
    Building(
        const std::string &name = "",
        const std::string &description = "",
        int cost = 0
    ) : mName(name),
        mDescription(description),
        mCost(cost >= 0 ? cost : 0)
    {}
    virtual ~Building() = default;

    const std::string &getName() const { return mName; }
    const std::string &getDescription() const { return mDescription; }
    std::string getPictureFilepath(const std::string &factionName) const
    {
        std::string normalizedName = mName;
        for (char &ch : normalizedName)
        {
            if (ch == ' ') ch = '_';
        }
        return "assets/factions/" + factionName + "/buildings/" + normalizedName + ".png";
    }
    int getCost() const { return mCost; }
    bool isBuilt() const { return mIsBuilt; }
    const std::vector<std::string> &getRequirements() const { return mRequirements; }

    void setName(const std::string &name) { mName = name; }
    void setDescription(const std::string &description) { mDescription = description; }
    void setCost(int cost) { mCost = cost >= 0 ? cost : 0; }
    void setBuilt(bool isBuilt) { mIsBuilt = isBuilt; }
    void addRequirement(const std::string &requirement) { mRequirements.push_back(requirement); }

    virtual void onShopRoundStart(LocalGameState &gameState) { (void)gameState; }
    virtual void onUnitPurchased(LocalGameState &gameState, Unit &purchasedUnit)
    {
        (void)gameState;
        (void)purchasedUnit;
    }
};

#endif
