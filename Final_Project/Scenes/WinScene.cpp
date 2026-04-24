#include "WinScene.h"
#include "Button.h"

namespace
{
    constexpr float BUTTON_WIDTH = 240.0f;
    constexpr float BUTTON_HEIGHT = 70.0f;
    constexpr float BUTTON_SPACING = 36.0f;
    constexpr float BUTTON_Y = 360.0f;
}

WinScene::WinScene(Vector2 origin, const char *bgHexCode)
    : Scene(origin, bgHexCode)
{
}

void WinScene::initialise()
{
    Scene::initialise();

    const float totalWidth = (BUTTON_WIDTH * 2.0f) + BUTTON_SPACING;
    const float leftX = mOrigin.x - (totalWidth / 2.0f);

    mButtons.push_back(new Button(
        { leftX, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT },
        "Main Menu",
        MAIN_MENU_SCENE_ID,
        KEY_M
    ));

    mButtons.push_back(new Button(
        { leftX + BUTTON_WIDTH + BUTTON_SPACING, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT },
        "Play Again",
        FACTION_SELECTION_SCENE_ID,
        KEY_P
    ));
}

void WinScene::update(float deltaTime)
{
    Scene::update(deltaTime);

    bool shouldResetRun = false;
    for (Button *button : mButtons)
    {
        if (button && button->isActivated())
        {
            shouldResetRun = true;
            break;
        }
    }

    if (shouldResetRun && mLocalGameState) mLocalGameState->resetRunProgress();
}

void WinScene::render()
{
    Scene::render();
    DrawText("Victory!", 390, 130, 70, GREEN);
}

void WinScene::shutdown()
{
    Scene::shutdown();
}
