#include "FactionSelection.h"
#include "Button.h"
#include <string>

namespace
{
    constexpr float PLAYER_Y_OFFSET = -190.0f;
    constexpr float BUTTON_WIDTH = 360.0f;
    constexpr float BUTTON_HEIGHT = 140.0f;
    constexpr float BUTTON_SPACING_X = 28.0f;
    constexpr float BUTTON_SPACING_Y = 20.0f;
    constexpr int GRID_COLUMNS = 2;
    constexpr float GRID_TOP_Y = 145.0f;
    constexpr int FACTION_HOTKEYS[] =
    {
        KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR, KEY_FIVE,
        KEY_SIX, KEY_SEVEN, KEY_EIGHT, KEY_NINE, KEY_ZERO
    };
}

FactionSelection::FactionSelection(Vector2 origin, const char *bgHexCode)
    : GameScene(origin, bgHexCode)
{
    setNextSceneID(SHOP_SCENE_ID);
}

void FactionSelection::initialise()
{
    GameScene::initialise();

    std::vector<std::string> factionNames =
        mLocalGameState ? mLocalGameState->getAvailableFactions() : std::vector<std::string>{};


    const int buttonCount = static_cast<int>(factionNames.size());
    const int totalButtonCount = buttonCount + 1;
    for (int i = 0; i < totalButtonCount; i++)
    {
        const int row = i / GRID_COLUMNS;
        const int column = i % GRID_COLUMNS;
        const int columnsInRow = std::min(GRID_COLUMNS, totalButtonCount - (row * GRID_COLUMNS));
        const float rowWidth = (columnsInRow * BUTTON_WIDTH) + ((columnsInRow - 1) * BUTTON_SPACING_X);
        const float x = mOrigin.x - (rowWidth / 2.0f) + (column * (BUTTON_WIDTH + BUTTON_SPACING_X));
        const float y = GRID_TOP_Y + (row * (BUTTON_HEIGHT + BUTTON_SPACING_Y));

        Button *button = nullptr;
        if (i == buttonCount)
        {
            button = new Button({ x, y, BUTTON_WIDTH, BUTTON_HEIGHT }, "assets/factions/Random.png");
            button->setKey(KEY_R);
        }
        else
        {
            const std::string factionName = factionNames[i];
            const std::string portraitFilepath = mLocalGameState
                ? mLocalGameState->getFactionPortraitFilepath(factionName)
                : "";
            button = portraitFilepath.empty()
                ? new Button({ x, y, BUTTON_WIDTH, BUTTON_HEIGHT }, factionName.c_str(), -1)
                : new Button({ x, y, BUTTON_WIDTH, BUTTON_HEIGHT }, portraitFilepath.c_str());
            const int hotkeyCount = static_cast<int>(sizeof(FACTION_HOTKEYS) / sizeof(FACTION_HOTKEYS[0]));
            if (i < hotkeyCount) button->setKey(FACTION_HOTKEYS[i]);
        }

        mButtons.push_back(button);
    }
}

void FactionSelection::update(float deltaTime)
{
    GameScene::update(deltaTime);

    for (Button *button : mButtons)
    {
        if (button->isActivated() && button->getKey() == KEY_R) mSelectedFaction = "Random";
        if (button->isActivated() && button->getKey() == KEY_THREE) mSelectedFaction = "SquirelGate";
        if (button->isActivated() && button->getKey() == KEY_TWO) mSelectedFaction = "LoonLake";
        if (button->isActivated() && button->getKey() == KEY_ONE) mSelectedFaction = "DinoDiner";
    }
}

void FactionSelection::render()
{
    Scene::render();

    DrawText("Choose Your Faction", 330, 36, 44, WHITE);
    
    const std::string selectedLabel = mSelectedFaction == "Random" ? "Selected: None" : "Selected: " + mSelectedFaction;
    DrawText(selectedLabel.c_str(), 20, 540, 26, mSelectedFaction == "Random" ? LIGHTGRAY : GREEN);

}

void FactionSelection::shutdown()
{
    if (mLocalGameState) mLocalGameState->setSelectedFaction(mSelectedFaction);
    GameScene::shutdown();
}
