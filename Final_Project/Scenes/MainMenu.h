#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "Scene.h"

class MainMenu : public Scene
{
public:
    MainMenu(Vector2 origin, const char *bgHexCode);

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif
