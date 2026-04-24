#include "GameScene.h"
#include "Button.h"
#include <string>

namespace
{
    constexpr float RESOURCE_BAR_WIDTH = 560.0f;
    constexpr float RESOURCE_BAR_HEIGHT = 44.0f;
    constexpr float RESOURCE_BAR_TOP_MARGIN = 10.0f;

    constexpr float NEXT_SCENE_BOX_WIDTH = 170.0f;
    constexpr float TIMER_BOX_HEIGHT = 44.0f;
    constexpr float TIMER_BOX_MARGIN_X = 14.0f;
    constexpr float TIMER_BOX_MARGIN_Y = 10.0f;

    constexpr float ICON_SIZE = 24.0f;
    constexpr float ICON_PADDING_X = 6.0f;
}

GameScene::GameScene(Vector2 origin, const char *bgHexCode)
    : Scene(origin, bgHexCode)
{
    loadResourceIcons();
    loadBackgroundTexture(DEFAULT_BACKGROUND_TEXTURE_FILEPATH);
}

GameScene::~GameScene()
{
    unloadBackgroundTexture();
    unloadResourceIcons();
}

void GameScene::loadBackgroundTexture(const char *textureFilepath)
{
    if (!textureFilepath || textureFilepath[0] == '\0') return;

    mBackgroundTexture = LoadTexture(textureFilepath);
    mBackgroundLoaded = mBackgroundTexture.id != 0;
    if (mBackgroundLoaded) mBackgroundTextureFilepath = textureFilepath;
}

void GameScene::unloadBackgroundTexture()
{
    if (!mBackgroundLoaded) return;

    if (mBackgroundTexture.id != 0) UnloadTexture(mBackgroundTexture);
    mBackgroundTexture = {};
    mBackgroundLoaded = false;
}

void GameScene::loadResourceIcons()
{
    if (mIconsLoaded) return;

    mGoldIcon = LoadTexture("assets/game_scene/Gold.png");
    mDarkGoldIcon = LoadTexture("assets/game_scene/Dark Gold.png");
    mHealthIcon = LoadTexture("assets/game_scene/Health.png");
    mIconsLoaded = true;
}

void GameScene::unloadResourceIcons()
{
    if (!mIconsLoaded) return;

    if (mGoldIcon.id != 0) UnloadTexture(mGoldIcon);
    if (mDarkGoldIcon.id != 0) UnloadTexture(mDarkGoldIcon);
    if (mHealthIcon.id != 0) UnloadTexture(mHealthIcon);
    mIconsLoaded = false;
}

void GameScene::initialise()
{
    Scene::initialise();

    Button *nextSceneButton = new Button(
        {
            static_cast<float>(GetScreenWidth()) - NEXT_SCENE_BOX_WIDTH - TIMER_BOX_MARGIN_X,
            TIMER_BOX_MARGIN_Y,
            NEXT_SCENE_BOX_WIDTH,
            TIMER_BOX_HEIGHT
        },
        "Next Scene",
        mNextSceneID,
        KEY_P
    );
    nextSceneButton->setTextFontSize(20);
    nextSceneButton->setTextButtonColors(BLACK, BLACK, GREEN, GREEN);
    mButtons.push_back(nextSceneButton);
}

void GameScene::drawTopCenterResourceBar()
{
    const float screenWidth = static_cast<float>(GetScreenWidth());
    const Rectangle barBounds =
    {
        (screenWidth - RESOURCE_BAR_WIDTH) / 2.0f,
        RESOURCE_BAR_TOP_MARGIN,
        RESOURCE_BAR_WIDTH,
        RESOURCE_BAR_HEIGHT
    };

    DrawRectangleRounded(barBounds, 0.18f, 8, Fade(BLACK, 0.7f));
    DrawRectangleRoundedLinesEx(barBounds, 0.18f, 8, 2.0f, WHITE);

    const std::string leftText = "Gold: " + std::to_string(mGold);
    const std::string centerText = "Dark Gold: " + std::to_string(mDarkGold);
    const std::string rightText = "HP: " + std::to_string(mPlayerHealth);
    const int fontSize = 18;
    const int textY = static_cast<int>(barBounds.y + 12.0f);
    const float iconY = barBounds.y + ((barBounds.height - ICON_SIZE) / 2.0f);

    const float sectionGoldX = barBounds.x + 10.0f;
    const float sectionDarkGoldX = barBounds.x + (barBounds.width * 0.37f);
    const float sectionHealthX = barBounds.x + (barBounds.width * 0.72f);

    if (mGoldIcon.id != 0) DrawTexturePro(mGoldIcon, { 0.0f, 0.0f, static_cast<float>(mGoldIcon.width), static_cast<float>(mGoldIcon.height) }, { sectionGoldX, iconY, ICON_SIZE, ICON_SIZE }, { 0.0f, 0.0f }, 0.0f, WHITE);
    DrawText(leftText.c_str(), static_cast<int>(sectionGoldX + ICON_SIZE + ICON_PADDING_X), textY, fontSize, GOLD);

    if (mDarkGoldIcon.id != 0) DrawTexturePro(mDarkGoldIcon, { 0.0f, 0.0f, static_cast<float>(mDarkGoldIcon.width), static_cast<float>(mDarkGoldIcon.height) }, { sectionDarkGoldX, iconY, ICON_SIZE, ICON_SIZE }, { 0.0f, 0.0f }, 0.0f, WHITE);
    DrawText(centerText.c_str(), static_cast<int>(sectionDarkGoldX + ICON_SIZE + ICON_PADDING_X), textY, fontSize, SKYBLUE);

    if (mHealthIcon.id != 0) DrawTexturePro(mHealthIcon, { 0.0f, 0.0f, static_cast<float>(mHealthIcon.width), static_cast<float>(mHealthIcon.height) }, { sectionHealthX, iconY, ICON_SIZE, ICON_SIZE }, { 0.0f, 0.0f }, 0.0f, WHITE);
    DrawText(rightText.c_str(), static_cast<int>(sectionHealthX + ICON_SIZE + ICON_PADDING_X), textY, fontSize, RED);
}

void GameScene::drawBackground()
{
    ClearBackground(ColorFromHex(getBGColourHexCode()));

    if (mBackgroundLoaded)
    {
        const Rectangle source = {
            0.0f,
            0.0f,
            static_cast<float>(mBackgroundTexture.width),
            static_cast<float>(mBackgroundTexture.height)
        };
        const Rectangle destination = {
            0.0f,
            0.0f,
            static_cast<float>(GetScreenWidth()),
            static_cast<float>(GetScreenHeight())
        };
        DrawTexturePro(mBackgroundTexture, source, destination, { 0.0f, 0.0f }, 0.0f, WHITE);
    }
}

void GameScene::render()
{
    drawBackground();

    for (const Button *button : mButtons)
    {
        if (button) button->render();
    }
    if (mPlayer) mPlayer->render();

    drawTopCenterResourceBar();
}

void GameScene::setBackgroundTextureFilepath(const char *textureFilepath)
{
    if (!textureFilepath || textureFilepath[0] == '\0') return;
    if (mBackgroundTextureFilepath == textureFilepath && mBackgroundLoaded) return;

    unloadBackgroundTexture();
    loadBackgroundTexture(textureFilepath);
}
