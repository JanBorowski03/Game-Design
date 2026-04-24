#ifndef SKIN_SELECTION_H
#define SKIN_SELECTION_H

#include "Scene.h"
#include <string>

class SkinSelection : public Scene
{
    public:
        SkinSelection(Vector2 origin, const char *bgHexCode);

        void initialise() override;
        void update(float deltaTime) override;
};

#endif
