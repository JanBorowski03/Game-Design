#ifndef FACTION_SELECTION_H
#define FACTION_SELECTION_H

#include "GameScene.h"
#include <string>
#include <vector>

class FactionSelection : public GameScene
{
private:
    std::string mSelectedFaction = "Random";

public:
    FactionSelection(Vector2 origin, const char *bgHexCode);

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif
