#ifndef SHOP_H
#define SHOP_H

#include "GameScene.h"
#include "BoardView.h"
#include <vector>

class Unit;
class Button;

class Shop : public GameScene
{
private:
    BoardView mBoardView;

    std::vector<Unit *> mShopUnits;
    std::vector<Button *> mShopSlotButtons;
    Button *mRerollButton = nullptr;
    bool mShopRefreshed = false;
    int mObservedShopRevision = -1;
    bool mBenchSelected = true;
    int mBenchSlotIndex = 0;
    int mBoardSlotIndex = 0;
    Unit *mHeldUnit = nullptr;
    Sound mUnitBuySound = {};
    bool mUnitBuySoundLoaded = false;
    Sound mUnitLevelUpSound = {};
    bool mUnitLevelUpSoundLoaded = false;

    void refreshShopFromGameState();
    void updateShopButtonPortraits();
    void syncHudFromLocalPlayer();
    void updateBoardSelectionInput();
    void drawSelectionCursor() const;

public:
    Shop(Vector2 origin, const char *bgHexCode);
    virtual ~Shop() = default;

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;

    const std::vector<Unit *> &getShopUnits() const { return mShopUnits; }
    void setShopUnits(const std::vector<Unit *> &shopUnits) { mShopUnits = shopUnits; }
};

#endif
