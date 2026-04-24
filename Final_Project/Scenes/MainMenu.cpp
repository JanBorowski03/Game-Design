#include "MainMenu.h"
#include "Button.h"

namespace
{
    constexpr float BUTTON_WIDTH = 220.0f;
    constexpr float BUTTON_HEIGHT = 90.0f;
    constexpr float PLAYER_Y_OFFSET = -150.0f;
    constexpr float BUTTON_Y_OFFSET = 150.0f;
    constexpr float BUTTON_GAP = 60.0f;
}

MainMenu::MainMenu(Vector2 origin, const char *bgHexCode) : Scene(origin, bgHexCode) {}

void MainMenu::initialise()
{
    Scene::initialise();
    mPlayer->setPosition({ mOrigin.x, mOrigin.y + PLAYER_Y_OFFSET });

    const float totalWidth = (BUTTON_WIDTH * 2.0f) + BUTTON_GAP;
    const float leftStartX = mOrigin.x - (totalWidth / 2.0f);

    mButtons.push_back(new Button(
        { leftStartX, mOrigin.y + BUTTON_Y_OFFSET, BUTTON_WIDTH, BUTTON_HEIGHT },
        "Select Skins",
        SKIN_SELECTION_SCENE_ID,
        KEY_ONE
    ));
    mButtons.push_back(new Button(
        { leftStartX + BUTTON_WIDTH + BUTTON_GAP, mOrigin.y + BUTTON_Y_OFFSET, BUTTON_WIDTH, BUTTON_HEIGHT },
        "Start Game",
        FACTION_SELECTION_SCENE_ID,
        KEY_TWO
    ));
}

void MainMenu::update(float deltaTime)
{
    Scene::update(deltaTime);
}

void MainMenu::render()
{
    Scene::render();
    DrawText("Move with WASD or Arrow Keys. ", 80, 36, 26, SKYBLUE);
    DrawText("Press ENTER while touching a button or use the button hotkey - (X)", 80, 72, 24, SKYBLUE);
}

void MainMenu::shutdown()
{
    Scene::shutdown();
}
