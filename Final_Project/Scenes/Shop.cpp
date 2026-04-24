#include "Shop.h"
#include "Button.h"
#include "../Units/Unit.h"
#include <array>
#include <string>

namespace
{
    constexpr int SHOP_UNIT_SLOT_COUNT = 4;
    constexpr int SHOP_TOTAL_SLOT_COUNT = SHOP_UNIT_SLOT_COUNT + 1; 
    constexpr int BENCH_SLOT_COUNT = 8;
    constexpr const char *EMPTY_SHOP_SLOT_TEXTURE = "assets/units/tmp.png";
    constexpr std::array<int, SHOP_UNIT_SLOT_COUNT> SHOP_SLOT_KEYS = { KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR };

    constexpr float BAR_MARGIN_X = 40.0f;
    constexpr float BAR_MARGIN_Y = 18.0f;
    constexpr float BAR_HEIGHT = 110.0f;
    constexpr float BAR_BUTTON_GAP = 8.0f;
    constexpr float BAR_BUTTON_INSET = 8.0f;
    
    constexpr float NAV_BUTTON_WIDTH = 180.0f;
    constexpr float NAV_BUTTON_HEIGHT = 44.0f;
    constexpr float NAV_BUTTON_MARGIN = 12.0f;
    constexpr const char *UNIT_BUY_SOUND_FILEPATH = "assets/sounds/unit_buy.wav";
    constexpr const char *UNIT_LEVEL_UP_SOUND_FILEPATH = "assets/sounds/unit_level_up.wav";

    Rectangle getEntityBounds(const Entity *entity)
    {
        const Vector2 position = entity->getPosition();
        const Vector2 size = entity->getScale();
        return
        {
            position.x - (size.x / 2.0f),
            position.y - (size.y / 2.0f),
            size.x,
            size.y
        };
    }

    Color getTierBorderColor(int tier)
    {
        switch (tier)
        {
            case UNIT_TIER_1: return LIGHTGRAY;
            case UNIT_TIER_2: return SKYBLUE;
            case UNIT_TIER_3: return VIOLET;
            case UNIT_TIER_4: return GOLD;
            default: return WHITE;
        }
    }
}

Shop::Shop(Vector2 origin, const char *bgHexCode)
    : GameScene(origin, bgHexCode)
{
}

void Shop::refreshShopFromGameState()
{
    if (!mLocalGameState)
    {
        mShopUnits.clear();
        mShopRefreshed = false;
        return;
    }

    const int currentRevision = mLocalGameState->getShopRevision();
    if (currentRevision == mObservedShopRevision) return;

    mObservedShopRevision = currentRevision;
    mShopUnits = mLocalGameState->getShopUnits();
    mShopRefreshed = true;
}

void Shop::initialise()
{
    GameScene::initialise();
    if (!mUnitBuySoundLoaded)
    {
        mUnitBuySound = LoadSound(UNIT_BUY_SOUND_FILEPATH);
        mUnitBuySoundLoaded = (mUnitBuySound.stream.buffer != nullptr);
        if (mUnitBuySoundLoaded) SetSoundVolume(mUnitBuySound, 0.75f);
    }
    if (!mUnitLevelUpSoundLoaded)
    {
        mUnitLevelUpSound = LoadSound(UNIT_LEVEL_UP_SOUND_FILEPATH);
        mUnitLevelUpSoundLoaded = (mUnitLevelUpSound.stream.buffer != nullptr);
        if (mUnitLevelUpSoundLoaded) SetSoundVolume(mUnitLevelUpSound, 0.80f);
    }
    syncHudFromLocalPlayer();
    mBoardView.initialise();
    mObservedShopRevision = -1;
    mShopRefreshed = false;
    mShopUnits.clear();
    mBenchSelected = true;
    mBenchSlotIndex = 0;
    mBoardSlotIndex = 0;
    mHeldUnit = nullptr;
    mShopSlotButtons.clear();
    mRerollButton = nullptr;

    const float screenW = static_cast<float>(GetScreenWidth());
    const float screenH = static_cast<float>(GetScreenHeight());
    const Rectangle bottomBar = {
        BAR_MARGIN_X,
        screenH - BAR_HEIGHT - BAR_MARGIN_Y,
        screenW - (BAR_MARGIN_X * 2.0f),
        BAR_HEIGHT
    };

    const float buttonAreaX = bottomBar.x + BAR_BUTTON_INSET;
    const float buttonAreaY = bottomBar.y + BAR_BUTTON_INSET;
    const float buttonAreaW = bottomBar.width - (BAR_BUTTON_INSET * 2.0f);
    const float buttonAreaH = bottomBar.height - (BAR_BUTTON_INSET * 2.0f);
    const float buttonW =
        (buttonAreaW - (BAR_BUTTON_GAP * static_cast<float>(SHOP_TOTAL_SLOT_COUNT - 1))) /
        static_cast<float>(SHOP_TOTAL_SLOT_COUNT);

    refreshShopFromGameState();

    for (int i = 0; i < SHOP_UNIT_SLOT_COUNT; i++)
    {
        const float x = buttonAreaX + ((i + 1) * (buttonW + BAR_BUTTON_GAP));
        Unit *unit = i < static_cast<int>(mShopUnits.size()) ? mShopUnits[i] : nullptr;
        const char *portraitFilepath = (unit && unit->getPortraitFilepath() && unit->getPortraitFilepath()[0] != '\0')
            ? unit->getPortraitFilepath()
            : EMPTY_SHOP_SLOT_TEXTURE;
        Button *slotButton = new Button({ x, buttonAreaY, buttonW, buttonAreaH }, portraitFilepath);
        slotButton->setKey(SHOP_SLOT_KEYS[static_cast<size_t>(i)]);
        mButtons.push_back(slotButton);
        mShopSlotButtons.push_back(slotButton);
    }

    LocalPlayerState *localPlayer = mLocalGameState ? mLocalGameState->getLocalPlayerState() : nullptr;
    const char *castlePortraitPath =
        (localPlayer && localPlayer->faction)
            ? localPlayer->faction->getPortraitFilepath().c_str()
            : "assets/factions/Random.png";

    Button *castleButton = new Button(
        {
            NAV_BUTTON_MARGIN,
            NAV_BUTTON_MARGIN,
            NAV_BUTTON_WIDTH,
            NAV_BUTTON_HEIGHT
        },
        castlePortraitPath,
        CASTLE_SCENE_ID,
        KEY_C,
        true
    );
    
    mButtons.push_back(castleButton);

    Button *rerollButton = new Button(
        {
            buttonAreaX,
            buttonAreaY,
            buttonW,
            buttonAreaH
        },
        "Reroll Shop",
        -1,
        KEY_R
    );
    rerollButton->setTextFontSize(22);
    rerollButton->setTextButtonColors(BLACK, BLACK, RAYWHITE, RAYWHITE);
    mButtons.push_back(rerollButton);
    mRerollButton = rerollButton;

    if (mShopRefreshed)
    {
        updateShopButtonPortraits();
        mShopRefreshed = false;
    }
}

void Shop::update(float deltaTime)
{
    GameScene::update(deltaTime);
    updateBoardSelectionInput();

    for (int i = 0; i < static_cast<int>(mShopSlotButtons.size()); i++)
    {
        Button *button = mShopSlotButtons[static_cast<size_t>(i)];
        if (!button) continue;
        if (button->isActivated())
        {
            if (mLocalGameState)
            {
                const bool purchased = mLocalGameState->tryPurchaseShopUnit(i);
                if (purchased && mUnitBuySoundLoaded)
                {
                    if (IsSoundPlaying(mUnitBuySound)) StopSound(mUnitBuySound);
                    PlaySound(mUnitBuySound);
                }
                if (purchased && mLocalGameState->consumePendingUnitLevelUpEvent() && mUnitLevelUpSoundLoaded)
                {
                    if (IsSoundPlaying(mUnitLevelUpSound)) StopSound(mUnitLevelUpSound);
                    PlaySound(mUnitLevelUpSound);
                }
            }
            break;
        }
    }

    if (mRerollButton && mRerollButton->isActivated())
    {
        if (mLocalGameState) mLocalGameState->requestShopReroll();
    }

    refreshShopFromGameState();

    if (mShopRefreshed)
    {
        updateShopButtonPortraits();
        mShopRefreshed = false;
    }

    syncHudFromLocalPlayer();
}

void Shop::render()
{
    drawBackground();
    mBoardView.renderBoard(nullptr);
    if (mLocalGameState)
    {
        mBoardView.renderPlayerFieldedUnits(mLocalGameState->getPlayerBoardUnits());
        mBoardView.renderPlayerBenchUnits(mLocalGameState->getBenchUnits());
    }
    drawSelectionCursor();

    const float screenW = static_cast<float>(GetScreenWidth());
    const float screenH = static_cast<float>(GetScreenHeight());
    const Rectangle bottomBar = {
        BAR_MARGIN_X,
        screenH - BAR_HEIGHT - BAR_MARGIN_Y,
        screenW - (BAR_MARGIN_X * 2.0f),
        BAR_HEIGHT
    };

    DrawRectangleRounded(bottomBar, 0.18f, 8, Fade(BLACK, 0.7f));
    DrawRectangleRoundedLinesEx(bottomBar, 0.18f, 8, 2.0f, WHITE);

    for (const Button *button : mButtons)
    {
        if (button) button->render();
    }

    for (int i = 0; i < static_cast<int>(mShopSlotButtons.size()); i++)
    {
        Button *shopButton = mShopSlotButtons[static_cast<size_t>(i)];
        if (!shopButton) continue;
        const Rectangle buttonBounds = getEntityBounds(shopButton);
        Unit *unit = (i < static_cast<int>(mShopUnits.size())) ? mShopUnits[i] : nullptr;
        if (!unit)
        {
            DrawRectangleRounded(buttonBounds, 0.2f, 8, BLACK);
            DrawRectangleRoundedLinesEx(buttonBounds, 0.2f, 8, 3.0f, DARKGRAY);
            continue;
        }

        DrawRectangleRoundedLinesEx(buttonBounds, 0.2f, 8, 4.0f, getTierBorderColor(unit->getTier()));
    }

    if (mPlayer) mPlayer->render();
    drawTopCenterResourceBar();
}

void Shop::shutdown()
{
    if (mUnitBuySoundLoaded)
    {
        if (IsSoundPlaying(mUnitBuySound)) StopSound(mUnitBuySound);
        UnloadSound(mUnitBuySound);
        mUnitBuySoundLoaded = false;
    }
    if (mUnitLevelUpSoundLoaded)
    {
        if (IsSoundPlaying(mUnitLevelUpSound)) StopSound(mUnitLevelUpSound);
        UnloadSound(mUnitLevelUpSound);
        mUnitLevelUpSoundLoaded = false;
    }

    GameScene::shutdown();
    mBoardView.shutdown();
    mShopUnits.clear();
    mShopSlotButtons.clear();
    mRerollButton = nullptr;
    mShopRefreshed = false;
    mObservedShopRevision = -1;
}

void Shop::updateShopButtonPortraits()
{
    for (int i = 0; i < static_cast<int>(mShopSlotButtons.size()); i++)
    {
        Button *shopButton = mShopSlotButtons[static_cast<size_t>(i)];
        if (!shopButton) continue;
        Unit *unit = (i < static_cast<int>(mShopUnits.size())) ? mShopUnits[i] : nullptr;
        const char *portraitFilepath = (unit && unit->getPortraitFilepath() && unit->getPortraitFilepath()[0] != '\0')
            ? unit->getPortraitFilepath()
            : EMPTY_SHOP_SLOT_TEXTURE;
        shopButton->setTexture(portraitFilepath);
    }
}

void Shop::syncHudFromLocalPlayer()
{
    LocalPlayerState *localPlayer = mLocalGameState ? mLocalGameState->getLocalPlayerState() : nullptr;
    if (!localPlayer) return;

    setGold(localPlayer->gold);
    setDarkGold(localPlayer->darkGold);
    setPlayerHealth(localPlayer->health);
}

void Shop::updateBoardSelectionInput()
{
    if (!mLocalGameState) return;

    const int benchSlotCount = static_cast<int>(mLocalGameState->getBenchUnits().size());
    const int boardSlotCount = static_cast<int>(mLocalGameState->getPlayerBoardUnits().size());

    if (benchSlotCount > 0 && mBenchSlotIndex >= benchSlotCount) mBenchSlotIndex = benchSlotCount - 1;
    if (boardSlotCount > 0 && mBoardSlotIndex >= boardSlotCount) mBoardSlotIndex = boardSlotCount - 1;

    if (IsKeyPressed(KEY_B))
    {
        mBenchSelected = !mBenchSelected;
    }

    if (IsKeyPressed(KEY_TAB))
    {
        if (mBenchSelected)
        {
            if (benchSlotCount > 0) mBenchSlotIndex = (mBenchSlotIndex + 1) % benchSlotCount;
        }
        else
        {
            if (boardSlotCount > 0) mBoardSlotIndex = (mBoardSlotIndex + 1) % boardSlotCount;
        }
    }

    if (IsKeyPressed(KEY_E))
    {
        const int selectedIndex = mBenchSelected ? mBenchSlotIndex : mBoardSlotIndex;
        if (mLocalGameState->trySellUnit(mBenchSelected, selectedIndex))
        {
            syncHudFromLocalPlayer();
        }
    }

    if (!IsKeyPressed(KEY_O)) return;

    std::vector<Unit *> &benchUnits = mLocalGameState->accessBenchUnits();
    std::vector<Unit *> &boardUnits = mLocalGameState->accessPlayerBoardUnits();
    std::vector<Unit *> &activeSlots = mBenchSelected ? benchUnits : boardUnits;
    int slotIndex = mBenchSelected ? mBenchSlotIndex : mBoardSlotIndex;
    if (slotIndex < 0 || slotIndex >= static_cast<int>(activeSlots.size())) return;

    Unit *&slot = activeSlots[static_cast<size_t>(slotIndex)];

    if (!mHeldUnit)
    {
        if (!slot) return;
        mHeldUnit = slot;
        slot = nullptr;
        return;
    }

    Unit *swap = slot;
    slot = mHeldUnit;
    mHeldUnit = swap;

    mLocalGameState->forceResolveUnitCombinations();
    if (mLocalGameState->consumePendingUnitLevelUpEvent() && mUnitLevelUpSoundLoaded)
    {
        if (IsSoundPlaying(mUnitLevelUpSound)) StopSound(mUnitLevelUpSound);
        PlaySound(mUnitLevelUpSound);
    }
    syncHudFromLocalPlayer();
}

void Shop::drawSelectionCursor() const
{
    const Rectangle selectedSlot = mBenchSelected
        ? mBoardView.getBenchSlotBounds(mBenchSlotIndex)
        : mBoardView.getPlayerFieldSlotBounds(mBoardSlotIndex);

    if (selectedSlot.width > 0.0f && selectedSlot.height > 0.0f)
    {
        DrawRectangleRoundedLinesEx(selectedSlot, 0.12f, 6, 4.0f, YELLOW);
    }

    const char *fieldLabel = mBenchSelected ? "Selected: Bench" : "Selected: Board";
    DrawText(fieldLabel, 30, 70, 24, YELLOW);

    const char *heldLabel = mHeldUnit ? "Held Unit: Yes (O to place/swap)" : "Held Unit: No (O to pick)";
    DrawText(heldLabel, 30, 96, 20, YELLOW);
    DrawText("B: Toggle Bench/Board | TAB: Next Slot | E: Sell | O: Pick/Place", 30, 120, 20, YELLOW);
}
