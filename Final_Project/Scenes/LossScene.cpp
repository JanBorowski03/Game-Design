#include "LossScene.h"
#include "Button.h"

namespace
{
    constexpr float BUTTON_WIDTH = 240.0f;
    constexpr float BUTTON_HEIGHT = 70.0f;
    constexpr float BUTTON_SPACING = 36.0f;
    constexpr float BUTTON_Y = 360.0f;
}

LossScene::LossScene(Vector2 origin, const char *bgHexCode)
    : Scene(origin, bgHexCode)
{
}

void LossScene::initialise()
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
        "Try Again",
        FACTION_SELECTION_SCENE_ID,
        KEY_T
    ));
}

void LossScene::update(float deltaTime)
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

void LossScene::render()
{
    Scene::render();
    DrawText("Defeat", 410, 130, 70, RED);
}

void LossScene::shutdown()
{
    Scene::shutdown();
}
