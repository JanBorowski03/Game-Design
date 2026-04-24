#include "Scene.h"
#include "Button.h"
#include "../Player.h"

Scene::Scene() : mOrigin{{}} {}

Scene::Scene(Vector2 origin, const char *bgHexCode) : mOrigin{origin}, mBGColourHexCode {bgHexCode} 
{
    ClearBackground(ColorFromHex(bgHexCode));
}

void Scene::initialise()
{
    mEntities.push_back(mPlayer);
    mKeybindSet =
    {
        KEY_W, KEY_A, KEY_S, KEY_D,
        KEY_UP, KEY_LEFT, KEY_DOWN, KEY_RIGHT
    };
    mMinPosition = { 0.0f, 0.0f };
    mMaxPosition = { static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight()) };
    clearSceneChangeRequest();
}

void Scene::processInput(const bool keyStates[], int keyCount)
{
    if (!mPlayer || !keyStates) return;

    const auto isBoundAndPressed = [&](int key)
    {
        return key >= 0 &&
               key < keyCount &&
               keyStates[key] &&
               mKeybindSet.find(key) != mKeybindSet.end();
    };

    mPlayer->resetMovement();

    if (isBoundAndPressed(KEY_W) || isBoundAndPressed(KEY_UP))    mPlayer->moveUp();
    if (isBoundAndPressed(KEY_S) || isBoundAndPressed(KEY_DOWN))  mPlayer->moveDown();
    if (isBoundAndPressed(KEY_A) || isBoundAndPressed(KEY_LEFT))  mPlayer->moveLeft();
    if (isBoundAndPressed(KEY_D) || isBoundAndPressed(KEY_RIGHT)) mPlayer->moveRight();

    for (Button *button : mButtons)
    {
        button->processInput(*this);
    }
}

void Scene::update(float deltaTime)
{
    if (!mPlayer) return;

    mPlayer->update(deltaTime, nullptr, nullptr, nullptr, 0);

    Vector2 position = mPlayer->getPosition();

    if (position.x < mMinPosition.x) position.x = mMinPosition.x;
    else if (position.x > mMaxPosition.x) position.x = mMaxPosition.x;

    if (position.y < mMinPosition.y) position.y = mMinPosition.y;
    else if (position.y > mMaxPosition.y) position.y = mMaxPosition.y;

    mPlayer->setPosition(position);

    for (Button *button : mButtons)
    {
        button->update(*this);
        if (button->isActivated() && button->getTargetSceneID() != -1)
        {
            requestSceneChange(button->getTargetSceneID());
        }
    }
}

void Scene::render()
{
    ClearBackground(ColorFromHex(getBGColourHexCode()));
    for (const Button *button : mButtons) button->render();
    mPlayer->render();
}

void Scene::requestSceneChange(int sceneID)
{
    if (!mLocalGameState) return;
    mLocalGameState->requestSceneChange(sceneID);
}

void Scene::clearSceneChangeRequest()
{
    if (!mLocalGameState) return;
    mLocalGameState->clearSceneChangeRequest();
}

void Scene::shutdown()
{
    for (Entity *entity : mEntities)
    {
        if (!entity) continue;
        if (entity == mPlayer && !mOwnsPlayer) continue;
        delete entity;
    }
    mEntities.clear();
    for (Button *button : mButtons) delete button;
    mButtons.clear();
    mPlayer = nullptr;
    mOwnsPlayer = false;
}
