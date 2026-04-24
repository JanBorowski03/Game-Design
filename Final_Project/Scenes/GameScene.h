#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include "Scene.h"
#include <string>

class GameScene : public Scene
{
private:
    static constexpr const char *DEFAULT_BACKGROUND_TEXTURE_FILEPATH = "assets/game_scene/Background.png";
    static constexpr int DEFAULT_NEXT_SCENE_ID = BATTLE_GROUNDS_SCENE_ID;

    int mGold = 0;
    int mDarkGold = 0;
    int mPlayerHealth = 100;
    int mNextSceneID = DEFAULT_NEXT_SCENE_ID;
    Texture2D mGoldIcon = {};
    Texture2D mDarkGoldIcon = {};
    Texture2D mHealthIcon = {};
    Texture2D mBackgroundTexture = {};
    std::string mBackgroundTextureFilepath = DEFAULT_BACKGROUND_TEXTURE_FILEPATH;
    bool mBackgroundLoaded = false;
    bool mIconsLoaded = false;

    void loadResourceIcons();
    void unloadResourceIcons();
    void loadBackgroundTexture(const char *textureFilepath);
    void unloadBackgroundTexture();

protected:
    void drawBackground();
    void drawTopCenterResourceBar();

public:
    GameScene(Vector2 origin, const char *bgHexCode);
    virtual ~GameScene() override;

    void initialise() override;
    void render() override;

    int getGold() const { return mGold; }
    int getDarkGold() const { return mDarkGold; }
    int getPlayerHealth() const { return mPlayerHealth; }
    int getNextSceneID() const { return mNextSceneID; }
    const std::string &getBackgroundTextureFilepath() const { return mBackgroundTextureFilepath; }

    void setGold(int value) { mGold = value; }
    void setDarkGold(int value) { mDarkGold = value; }
    void setPlayerHealth(int value) { mPlayerHealth = value; }
    void setNextSceneID(int sceneID) { mNextSceneID = sceneID; }
    void setBackgroundTextureFilepath(const char *textureFilepath);
};

#endif
