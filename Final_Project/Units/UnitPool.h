#ifndef UNIT_POOL_H
#define UNIT_POOL_H

#include "Unit.h"
#include <array>
#include <memory>
#include <random>
#include <vector>

struct ShopTierOdds
{
    float tier1;
    float tier2;
    float tier3;
    float tier4;

    ShopTierOdds(
        float tier1Value = 0.0f,
        float tier2Value = 0.0f,
        float tier3Value = 0.0f,
        float tier4Value = 0.0f
    ) : tier1(tier1Value),
        tier2(tier2Value),
        tier3(tier3Value),
        tier4(tier4Value)
    {
    }
};

class UnitPool
{
private:
    std::vector<std::unique_ptr<Unit>> mOwnedUnits;
    std::vector<Unit *> mPool;
    std::mt19937 mRng;

    float getTierWeight(int tier, const ShopTierOdds &odds) const;

public:
    UnitPool();

    void initialise(const std::array<int, 4> &unitsPerTier, Vector2 defaultPosition = { 0.0f, 0.0f });
    void clear();

    bool addToPool(Unit *unit);
    bool removeFromPool(Unit *unit);
    bool contains(Unit *unit) const;

    Unit *drawRandomUnit(const ShopTierOdds &odds);
    std::vector<Unit *> getUnitsByTier(int tier) const;

    int getPoolSize() const { return static_cast<int>(mPool.size()); }
    int getOwnedUnitCount() const { return static_cast<int>(mOwnedUnits.size()); }
};

#endif
