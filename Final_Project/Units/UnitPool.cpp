#include "UnitPool.h"
#include "CommonUnit.h"
#include "RareUnit.h"
#include "EpicUnit.h"
#include "LegendaryUnit.h"
#include <algorithm>

UnitPool::UnitPool()
{
    std::random_device device;
    mRng.seed(device());
}

float UnitPool::getTierWeight(int tier, const ShopTierOdds &odds) const
{
    switch (tier)
    {
        case UNIT_TIER_1: return odds.tier1;
        case UNIT_TIER_2: return odds.tier2;
        case UNIT_TIER_3: return odds.tier3;
        case UNIT_TIER_4: return odds.tier4;
        default: return 0.0f;
    }
}

void UnitPool::initialise(const std::array<int, 4> &unitsPerTier, Vector2 defaultPosition)
{
    clear();

    for (int tier = UNIT_TIER_1; tier <= UNIT_TIER_4; tier++)
    {
        const int count = unitsPerTier[static_cast<size_t>(tier - UNIT_TIER_1)];
        for (int i = 0; i < count; i++)
        {
            std::unique_ptr<Unit> unit;

            switch (tier)
            {
                case UNIT_TIER_1:
                    unit.reset(new CommonUnit(defaultPosition));
                    break;
                case UNIT_TIER_2:
                    unit.reset(new RareUnit(defaultPosition));
                    break;
                case UNIT_TIER_3:
                    unit.reset(new EpicUnit(defaultPosition));
                    break;
                case UNIT_TIER_4:
                    unit.reset(new LegendaryUnit(defaultPosition));
                    break;
                default:
                    continue;
            }

            mPool.push_back(unit.get());
            mOwnedUnits.push_back(std::move(unit));
        }
    }
}

void UnitPool::clear()
{
    mPool.clear();
    mOwnedUnits.clear();
}

bool UnitPool::addToPool(Unit *unit)
{
    if (!unit) return false;
    if (contains(unit)) return false;

    mPool.push_back(unit);
    return true;
}

bool UnitPool::removeFromPool(Unit *unit)
{
    if (!unit) return false;

    const std::vector<Unit *>::iterator it = std::find(mPool.begin(), mPool.end(), unit);
    if (it == mPool.end()) return false;

    mPool.erase(it);
    return true;
}

bool UnitPool::contains(Unit *unit) const
{
    return std::find(mPool.begin(), mPool.end(), unit) != mPool.end();
}

std::vector<Unit *> UnitPool::getUnitsByTier(int tier) const
{
    std::vector<Unit *> units;
    for (Unit *unit : mPool)
    {
        if (unit && unit->getTier() == tier) units.push_back(unit);
    }
    return units;
}

Unit *UnitPool::drawRandomUnit(const ShopTierOdds &odds)
{
    struct TierBucket
    {
        int tier = UNIT_TIER_1;
        float weight = 0.0f;
    };

    std::vector<TierBucket> candidateTiers;
    candidateTiers.reserve(4);

    float totalWeight = 0.0f;
    for (int tier = UNIT_TIER_1; tier <= UNIT_TIER_4; tier++)
    {
        const std::vector<Unit *> units = getUnitsByTier(tier);
        if (units.empty()) continue;

        const float weight = getTierWeight(tier, odds);
        if (weight <= 0.0f) continue;

        TierBucket bucket;
        bucket.tier = tier;
        bucket.weight = weight;
        candidateTiers.push_back(bucket);
        totalWeight += weight;
    }

    if (candidateTiers.empty() || totalWeight <= 0.0f) return nullptr;

    std::uniform_real_distribution<float> tierDistribution(0.0f, totalWeight);
    const float roll = tierDistribution(mRng);

    float cumulative = 0.0f;
    int selectedTier = candidateTiers.back().tier;
    for (const TierBucket &bucket : candidateTiers)
    {
        cumulative += bucket.weight;
        if (roll <= cumulative)
        {
            selectedTier = bucket.tier;
            break;
        }
    }

    const std::vector<Unit *> tierUnits = getUnitsByTier(selectedTier);
    if (tierUnits.empty()) return nullptr;

    std::uniform_int_distribution<size_t> unitDistribution(0, tierUnits.size() - 1);
    Unit *chosenUnit = tierUnits[unitDistribution(mRng)];
    removeFromPool(chosenUnit);
    return chosenUnit;
}
