#ifndef LOSS_SCENE_H
#define LOSS_SCENE_H

#include "Scene.h"

class LossScene : public Scene
{
public:
    LossScene(Vector2 origin, const char *bgHexCode);
    virtual ~LossScene() = default;

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif
