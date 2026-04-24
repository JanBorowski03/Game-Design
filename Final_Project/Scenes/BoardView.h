#ifndef BOARD_VIEW_H
#define BOARD_VIEW_H

#include "Map.h"
#include <memory>
#include <vector>

class Unit;

class BoardView
{
private:
    static constexpr int BOARD_COLUMNS = 8;
    static constexpr int BOARD_ROWS = 7;
    static constexpr int FOG_COLUMNS = 4;
    static constexpr int BENCH_ROWS = 1;
    static constexpr int PLAYER_BOARD_SLOT_COUNT = 20;

    Map *mGroundMap = nullptr;
    std::vector<unsigned int> mGroundTiles;
    Texture2D mBenchTexture = {};
    Texture2D mFogTexture = {};
    bool mTexturesLoaded = false;

    void loadTextures();
    void unloadTextures();

public:
    BoardView() = default;
    ~BoardView();

    void initialise();
    void shutdown();
    void renderBoard(const std::vector<Unit *> *enemyBoard = nullptr, bool showEnemyFog = true);
    void renderPlayerBenchUnits(const std::vector<Unit *> &playerBoard);
    void renderPlayerFieldedUnits(const std::vector<Unit *> &playerBoard);
    void renderEnemyUnits(const std::vector<Unit *> *enemyBoard);
    Rectangle getTileBounds(int row, int col) const;
    Rectangle getBenchSlotBounds(int slotIndex) const;
    Rectangle getPlayerFieldSlotBounds(int slotIndex) const;
    Rectangle getEnemyFieldSlotBounds(int slotIndex) const;
    int getEnemyFogSlotCount() const;
};

#endif
