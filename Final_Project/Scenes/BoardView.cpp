#include "BoardView.h"
#include "../Units/Unit.h"
#include <array>

namespace
{
    constexpr float BENCH_BOARD_PANEL_TOP_OFFSET = 62.0f;
    constexpr float BAR_MARGIN_X = 40.0f;
    constexpr float BAR_MARGIN_Y = 18.0f;
    constexpr float BAR_HEIGHT = 110.0f;

    const std::array<std::pair<int, int>, 20> PLAYER_FIELD_COORDS =
    {{
        { 5, 0 }, { 5, 1 }, { 5, 2 }, { 5, 3 },
        { 4, 0 }, { 4, 1 }, { 4, 2 }, { 4, 3 },
        { 3, 0 }, { 3, 1 }, { 3, 2 }, { 3, 3 },
        { 2, 0 }, { 2, 1 }, { 2, 2 }, { 2, 3 },
        { 1, 0 }, { 1, 1 }, { 1, 2 }, { 1, 3 }
    }};
}

BoardView::~BoardView()
{
    shutdown();
}

void BoardView::loadTextures()
{
    if (mTexturesLoaded) return;
    mBenchTexture = LoadTexture("assets/battle_grounds/bench.png");
    mFogTexture = LoadTexture("assets/battle_grounds/fog.png");
    mTexturesLoaded = true;
}

void BoardView::unloadTextures()
{
    if (!mTexturesLoaded) return;
    if (mBenchTexture.id != 0) UnloadTexture(mBenchTexture);
    if (mFogTexture.id != 0) UnloadTexture(mFogTexture);
    mBenchTexture = {};
    mFogTexture = {};
    mTexturesLoaded = false;
}

void BoardView::initialise()
{
    shutdown();
    loadTextures();

    const float screenW = static_cast<float>(GetScreenWidth());
    const float screenH = static_cast<float>(GetScreenHeight());
    const float panelWidth = screenW - (BAR_MARGIN_X * 2.0f);
    const float bottomBarTopY = screenH - BAR_HEIGHT - BAR_MARGIN_Y;
    const float panelHeight = bottomBarTopY - BENCH_BOARD_PANEL_TOP_OFFSET;
    const float tileWidth = panelWidth / static_cast<float>(BOARD_COLUMNS);
    const float tileHeight = panelHeight / static_cast<float>(BOARD_ROWS);
    const Vector2 origin = { screenW / 2.0f, BENCH_BOARD_PANEL_TOP_OFFSET + (panelHeight / 2.0f) };

    mGroundTiles.assign(static_cast<size_t>(BOARD_ROWS * BOARD_COLUMNS), 1U);
    mGroundMap = new Map(
        BOARD_COLUMNS,
        BOARD_ROWS,
        mGroundTiles.data(),
        "assets/battle_grounds/earth.png",
        tileWidth,
        1,
        1,
        origin,
        tileHeight
    );
}

void BoardView::shutdown()
{
    if (mGroundMap)
    {
        delete mGroundMap;
        mGroundMap = nullptr;
    }
    mGroundTiles.clear();
    unloadTextures();
}

Rectangle BoardView::getTileBounds(int row, int col) const
{
    if (!mGroundMap) return {};
    return mGroundMap->getTileWorldBounds(row, col);
}

Rectangle BoardView::getBenchSlotBounds(int slotIndex) const
{
    if (slotIndex < 0 || slotIndex >= PLAYER_BOARD_SLOT_COUNT) return {};
    return getTileBounds(BOARD_ROWS - 1, slotIndex);
}

Rectangle BoardView::getPlayerFieldSlotBounds(int slotIndex) const
{
    if (slotIndex < 0 || slotIndex >= PLAYER_BOARD_SLOT_COUNT) return {};
    const int row = PLAYER_FIELD_COORDS[static_cast<size_t>(slotIndex)].first;
    const int col = PLAYER_FIELD_COORDS[static_cast<size_t>(slotIndex)].second;
    return getTileBounds(row, col);
}

Rectangle BoardView::getEnemyFieldSlotBounds(int slotIndex) const
{
    const int maxSlots = getEnemyFogSlotCount();
    if (slotIndex < 0 || slotIndex >= maxSlots) return {};
    const int row = slotIndex / FOG_COLUMNS;
    const int col = (BOARD_COLUMNS - FOG_COLUMNS) + (slotIndex % FOG_COLUMNS);
    return getTileBounds(row, col);
}

int BoardView::getEnemyFogSlotCount() const
{
    return (BOARD_ROWS - BENCH_ROWS) * FOG_COLUMNS;
}

void BoardView::renderBoard(const std::vector<Unit *> *enemyBoard, bool showEnemyFog)
{
    if (!mGroundMap) return;

    mGroundMap->render();

    for (int row = 0; row < BOARD_ROWS; row++)
    {
        for (int col = 0; col < BOARD_COLUMNS; col++)
        {
            const bool isBenchTile = row == (BOARD_ROWS - BENCH_ROWS);
            const bool isEnemyFogTile = !isBenchTile && col >= (BOARD_COLUMNS - FOG_COLUMNS);
            const Rectangle tile = getTileBounds(row, col);

            if (isBenchTile && mBenchTexture.id != 0)
            {
                DrawTexturePro(
                    mBenchTexture,
                    { 0.0f, 0.0f, static_cast<float>(mBenchTexture.width), static_cast<float>(mBenchTexture.height) },
                    tile,
                    { 0.0f, 0.0f },
                    0.0f,
                    WHITE
                );
            }

            if (isEnemyFogTile && showEnemyFog)
            {
                const int fogSlot = row * FOG_COLUMNS + (col - (BOARD_COLUMNS - FOG_COLUMNS));
                const bool shouldFog =
                    !enemyBoard ||
                    fogSlot < 0 ||
                    fogSlot >= static_cast<int>(enemyBoard->size()) ||
                    !(*enemyBoard)[static_cast<size_t>(fogSlot)];

                if (shouldFog && mFogTexture.id != 0)
                {
                    DrawTexturePro(
                        mFogTexture,
                        { 0.0f, 0.0f, static_cast<float>(mFogTexture.width), static_cast<float>(mFogTexture.height) },
                        tile,
                        { 0.0f, 0.0f },
                        0.0f,
                        WHITE
                    );
                }
            }

            const Color border = isEnemyFogTile ? Fade(RED, 0.75f) : Fade(SKYBLUE, 0.65f);
            DrawRectangleRoundedLinesEx(tile, 0.12f, 6, 2.0f, border);
        }
    }
}

void BoardView::renderPlayerBenchUnits(const std::vector<Unit *> &playerBoard)
{
    for (int i = 0; i < PLAYER_BOARD_SLOT_COUNT && i < static_cast<int>(playerBoard.size()); i++)
    {
        Unit *unit = playerBoard[static_cast<size_t>(i)];
        if (!unit) continue;

        const Rectangle tile = getTileBounds(BOARD_ROWS - 1, i);
        if (tile.width <= 0.0f || tile.height <= 0.0f) continue;

        unit->setPosition({ tile.x + (tile.width / 2.0f), tile.y + (tile.height / 2.0f) });
        unit->render();
    }
}

void BoardView::renderPlayerFieldedUnits(const std::vector<Unit *> &playerBoard)
{
    for (int i = 0; i < PLAYER_BOARD_SLOT_COUNT && i < static_cast<int>(playerBoard.size()); i++)
    {
        Unit *unit = playerBoard[static_cast<size_t>(i)];
        if (!unit) continue;

        const int row = PLAYER_FIELD_COORDS[static_cast<size_t>(i)].first;
        const int col = PLAYER_FIELD_COORDS[static_cast<size_t>(i)].second;
        const Rectangle tile = getTileBounds(row, col);
        if (tile.width <= 0.0f || tile.height <= 0.0f) continue;

        unit->setPosition({ tile.x + (tile.width / 2.0f), tile.y + (tile.height / 2.0f) });
        unit->render();
    }
}

void BoardView::renderEnemyUnits(const std::vector<Unit *> *enemyBoard)
{
    if (!enemyBoard) return;

    for (int slot = 0; slot < static_cast<int>(enemyBoard->size()); slot++)
    {
        Unit *unit = (*enemyBoard)[static_cast<size_t>(slot)];
        if (!unit) continue;

        const int row = slot / FOG_COLUMNS;
        const int col = (BOARD_COLUMNS - FOG_COLUMNS) + (slot % FOG_COLUMNS);
        const Rectangle tile = getTileBounds(row, col);
        if (tile.width <= 0.0f || tile.height <= 0.0f) continue;

        unit->setPosition({ tile.x + (tile.width / 2.0f), tile.y + (tile.height / 2.0f) });
        unit->render();
    }
}
