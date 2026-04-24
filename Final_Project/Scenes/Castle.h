#ifndef CASTLE_H
#define CASTLE_H

#include "GameScene.h"
#include <vector>

class Button;

class Castle : public GameScene
{
private:
    std::vector<Button *> mBuildingButtons;
    int mSelectedBuildingIndex = -1;
    Sound mBuildSound = {};
    bool mBuildSoundLoaded = false;

    void syncHudFromLocalPlayer();
    void renderBuildingNavbar() const;

public:
    Castle(Vector2 origin, const char *bgHexCode);
    virtual ~Castle() = default;

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif
