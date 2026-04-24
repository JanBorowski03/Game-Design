#ifndef LOCAL_GAME_STATE_H
#define LOCAL_GAME_STATE_H

#include "Factions/Faction.h"
#include "Player.h"
#include "Scenes/SceneIDs.h"
#include "Units/UnitPool.h"
#include <array>
#include <string>
#include <vector>

struct LocalPlayerState
{
    int health = 100;
    int gold = 10;
    int darkGold = 15;
    Faction *faction = nullptr;
    Player *player = nullptr;
};

class LocalGameState
{
private:
    static constexpr int SHOP_SLOT_COUNT = 4;
    static constexpr int BENCH_SLOT_COUNT = 8;
    static constexpr int PLAYER_BOARD_SLOT_COUNT = 24;

    bool mSceneChangeRequested = false;
    int mNextSceneID = FACTION_SELECTION_SCENE_ID;

    LocalPlayerState mLocalPlayer = {};
    std::vector<std::string> mAvailableFactions;
    UnitPool mUnitPool;
    std::vector<Unit *> mShopUnits;
    std::vector<Unit *> mBenchUnits;
    std::vector<Unit *> mPlayerBoardUnits;
    ShopTierOdds mCurrentShopOdds = { 0.55f, 0.30f, 0.12f, 0.03f };
    int mPlayerShopTier = 1;
    bool mPendingUnitLevelUpEvent = false;
    int mShopRevision = 0;
    int mEncounterNumber = 1;

    void returnBoardUnitsToPool();
    bool resolveUnitCombinations();
    void resolveBuiltBuildingEffectsOnShopRoundStart();
    void resolveBuiltBuildingEffectsOnUnitPurchased(Unit &unit);
    void setCurrentShopOddsFromTier();

public:
    LocalGameState();
    ~LocalGameState();

    bool isSceneChangeRequested() const { return mSceneChangeRequested; }
    int getNextSceneID() const { return mNextSceneID; }
    LocalPlayerState *getLocalPlayerState() { return &mLocalPlayer; }
    const std::vector<std::string> &getAvailableFactions() const { return mAvailableFactions; }
    std::string getFactionPortraitFilepath(const std::string &factionName) const;
    const std::vector<Unit *> &getShopUnits() const { return mShopUnits; }
    const std::vector<Unit *> &getBenchUnits() const { return mBenchUnits; }
    std::vector<Unit *> &accessBenchUnits() { return mBenchUnits; }
    const std::vector<Unit *> &getPlayerBoardUnits() const { return mPlayerBoardUnits; }
    std::vector<Unit *> &accessPlayerBoardUnits() { return mPlayerBoardUnits; }
    int getShopRevision() const { return mShopRevision; }
    int findFirstFreeBenchSlot() const;
    int getEncounterNumber() const { return mEncounterNumber; }
    int getPlayerShopTier() const { return mPlayerShopTier; }
    const ShopTierOdds &getCurrentShopOdds() const { return mCurrentShopOdds; }

    void initialise();
    void initializeFactions();
    bool setSelectedFaction(const std::string &factionName);
    bool selectRandomFaction();
    void requestShopReroll();
    bool tryPurchaseShopUnit(int shopSlotIndex);
    bool tryPurchaseBuilding(int buildingIndex);
    bool trySellUnit(bool fromBench, int slotIndex);
    void forceResolveUnitCombinations() { mPendingUnitLevelUpEvent |= resolveUnitCombinations(); }
    bool consumePendingUnitLevelUpEvent()
    {
        const bool pending = mPendingUnitLevelUpEvent;
        mPendingUnitLevelUpEvent = false;
        return pending;
    }
    void addGold(int amount);
    void addDarkGold(int amount);
    void setPlayerShopTier(int tier) { mPlayerShopTier = tier; };
    void boostPurchasedUnitDamage(Unit &unit, float multiplier);
    void boostPurchasedUnitHealth(Unit &unit, float multiplier);
    void applyRoundStartBonuses();
    void markEncounterWin();
    void applyEncounterLossDamage(int enemiesLeft);
    void restoreUnitsAfterEncounter();
    void resetRunProgress();

    void requestSceneChange(int sceneID);
    void clearSceneChangeRequest();
};

#endif
