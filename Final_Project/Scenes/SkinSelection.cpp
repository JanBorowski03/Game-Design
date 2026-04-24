#include "SkinSelection.h"
#include "Button.h"
#include <algorithm>
#include <string>
#include <vector>
#include <unordered_set>

namespace
{
constexpr float PLAYER_Y_OFFSET = -170.0f;
constexpr float BUTTON_SIZE = 96.0f;
constexpr float BUTTON_SPACING = 24.0f;
constexpr int GRID_COLUMNS = 4;
constexpr float GRID_Y_OFFSET = 20.0f;
constexpr float BACK_BUTTON_WIDTH = 600.0f;
constexpr float BACK_BUTTON_HEIGHT = 200.0f;
constexpr float BACK_BUTTON_Y = 24.0f;
const std::vector<int> SKIN_HOTKEY_ORDER =
{
    KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR, KEY_FIVE,
    KEY_SIX, KEY_SEVEN, KEY_EIGHT, KEY_NINE, KEY_ZERO
};
const std::unordered_set<int> SKIN_HOTKEYS(SKIN_HOTKEY_ORDER.begin(), SKIN_HOTKEY_ORDER.end());
}

SkinSelection::SkinSelection(Vector2 origin, const char *bgHexCode)
    : Scene(origin, bgHexCode)
{
}

void SkinSelection::initialise()
{
    Scene::initialise();
    const float playerStartX = mOrigin.x;
    const float playerStartY = mOrigin.y + PLAYER_Y_OFFSET;
    const float backButtonX = mOrigin.x - (BACK_BUTTON_WIDTH / 2.0f);

    FilePathList skins = LoadDirectoryFilesEx("assets/player_skins", ".png", false);
    std::vector<std::string> skinPaths;
    skinPaths.reserve(static_cast<size_t>(skins.count));

    for (unsigned int i = 0; i < skins.count; i++)
    {
        if (skins.paths[i]) skinPaths.emplace_back(skins.paths[i]);
    }

    std::sort(skinPaths.begin(), skinPaths.end());
    const int buttonCount = static_cast<int>(skinPaths.size());

    for (int i = 0; i < buttonCount; i++)
    {
        const int row = i / GRID_COLUMNS;
        const int column = i % GRID_COLUMNS;

        const int columnsInRow = std::min(GRID_COLUMNS, buttonCount - (row * GRID_COLUMNS));
        const float rowWidth = (columnsInRow * BUTTON_SIZE) + ((columnsInRow - 1) * BUTTON_SPACING);

        const float x = mOrigin.x - (rowWidth / 2.0f) + (column * (BUTTON_SIZE + BUTTON_SPACING));
        const float y = mOrigin.y + GRID_Y_OFFSET + (row * (BUTTON_SIZE + BUTTON_SPACING));

        const int hotkeyCount = static_cast<int>(SKIN_HOTKEY_ORDER.size());
        const int hotkey = i < hotkeyCount ? SKIN_HOTKEY_ORDER[i] : KEY_NULL;
        Button *skinButton = new Button({ x, y, BUTTON_SIZE, BUTTON_SIZE }, skinPaths[i].c_str());
        skinButton->setKey(hotkey);
        mButtons.push_back(skinButton);
    }


    Button * backButton = new Button(
                                        { backButtonX, BACK_BUTTON_Y, BACK_BUTTON_WIDTH, BACK_BUTTON_HEIGHT },
                                        "Back To Main Menu",
                                        MAIN_MENU_SCENE_ID
                                    );
    backButton->setKey(KEY_B);
    mButtons.push_back(backButton);

    UnloadDirectoryFiles(skins);
}

void SkinSelection::update(float deltaTime)
{
    Scene::update(deltaTime);

    for (Button *button : mButtons)
    {
        if (button->isActivated() && SKIN_HOTKEYS.find(button->getKey()) != SKIN_HOTKEYS.end())
        {
            mPlayer->setTexture(button->getSkinTextureFilepath().c_str());
            button->deactivate();
        }
    }
}
