#ifndef SCENE_H
#define SCENE_H

#include "../CS3113/Entity.h"
#include "../LocalGameState.h"
#include "../Player.h"
#include "SceneIDs.h"
#include <vector>
#include <unordered_set>

class Button;

class Scene 
{
protected:
    static constexpr int KEY_INPUT_ARRAY_SIZE = 512;

    LocalGameState* mLocalGameState = nullptr;
    std::vector<Entity*> mEntities;
    std::vector<Button*> mButtons;
    Player *mPlayer = nullptr;
    bool mOwnsPlayer = false;
    std::unordered_set<int> mKeybindSet;
    Vector2 mOrigin;
    Vector2 mMinPosition = {};
    Vector2 mMaxPosition = {};
    const char *mBGColourHexCode = "#000000";
    
public:
    Scene();
    Scene(Vector2 origin, const char *bgHexCode);
    virtual ~Scene() = default;

    virtual void initialise();
    virtual void processInput(const bool keyStates[], int keyCount);
    virtual void update(float deltaTime);
    virtual void render();
    virtual void shutdown();
    
    LocalGameState *getGameState() const { return mLocalGameState; }
    void setGameState(LocalGameState *gameState) { mLocalGameState = gameState; }
    void setPlayer(Player *player) { mPlayer = player; mOwnsPlayer = false; }
    Entity* getPlayer()              const { return mPlayer; }
    int getKeyInputArraySize()       const { return KEY_INPUT_ARRAY_SIZE; }
    const std::vector<Entity*>& getEntities() const { return mEntities; }
    Vector2     getOrigin()          const { return mOrigin;    }
    const char* getBGColourHexCode() const { return mBGColourHexCode; }
    void requestSceneChange(int sceneID);
    void clearSceneChangeRequest();
};

#endif
