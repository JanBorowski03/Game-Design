#include "Map.h"

Map::Map(int mapColumns, int mapRows, unsigned int *levelData,
         const char *textureFilePath, float tileWidth, int textureColumns,
         int textureRows, Vector2 origin, float tileHeight) :
         mMapColumns {mapColumns}, mMapRows {mapRows},
         mLevelData {levelData},
         mTextureAtlas { LoadTexture(textureFilePath) },
         mTileWidth {tileWidth},
         mTileHeight { tileHeight > 0.0f ? tileHeight : tileWidth },
         mTextureColumns {textureColumns}, mTextureRows {textureRows},
         mOrigin {origin} { build(); }

Map::~Map() { UnloadTexture(mTextureAtlas); }

void Map::build()
{
    mLeftBoundary   = mOrigin.x - (mMapColumns * mTileWidth) / 2.0f;
    mRightBoundary  = mOrigin.x + (mMapColumns * mTileWidth) / 2.0f;
    mTopBoundary    = mOrigin.y - (mMapRows * mTileHeight) / 2.0f;
    mBottomBoundary = mOrigin.y + (mMapRows * mTileHeight) / 2.0f;

    mTextureAreas.clear();
    for (int row = 0; row < mTextureRows; row++)
    {
        for (int col = 0; col < mTextureColumns; col++)
        {
            Rectangle textureArea = {
                static_cast<float>(col) * (mTextureAtlas.width / mTextureColumns),
                static_cast<float>(row) * (mTextureAtlas.height / mTextureRows),
                static_cast<float>(mTextureAtlas.width / mTextureColumns),
                static_cast<float>(mTextureAtlas.height / mTextureRows)
            };

            mTextureAreas.push_back(textureArea);
        }
    }
}

void Map::render(const std::function<void(int, int, unsigned int, const Rectangle &)> &tileRenderer)
{
    for (int row = 0; row < mMapRows; row++)
    {
        for (int col = 0; col < mMapColumns; col++)
        {
            const int tile = static_cast<int>(mLevelData[row * mMapColumns + col]);
            if (tile == 0) continue;

            const Rectangle destinationArea = {
                mLeftBoundary + col * mTileWidth,
                mTopBoundary  + row * mTileHeight,
                mTileWidth,
                mTileHeight
            };

            if (tileRenderer)
            {
                tileRenderer(row, col, static_cast<unsigned int>(tile), destinationArea);
                continue;
            }

            DrawTexturePro(
                mTextureAtlas,
                mTextureAreas[static_cast<size_t>(tile - 1)],
                destinationArea,
                { 0.0f, 0.0f },
                0.0f,
                WHITE
            );
        }
    }
}

Rectangle Map::getTileWorldBounds(int row, int col) const
{
    if (row < 0 || row >= mMapRows) return {};
    if (col < 0 || col >= mMapColumns) return {};
    return
    {
        mLeftBoundary + col * mTileWidth,
        mTopBoundary + row * mTileHeight,
        mTileWidth,
        mTileHeight
    };
}

bool Map::isSolidTileAt(Vector2 position, float *xOverlap, float *yOverlap)
{
    *xOverlap = 0.0f;
    *yOverlap = 0.0f;

    if (position.x < mLeftBoundary || position.x > mRightBoundary ||
        position.y < mTopBoundary  || position.y > mBottomBoundary)
        return false;

    const int tileXIndex = static_cast<int>(floor((position.x - mLeftBoundary) / mTileWidth));
    const int tileYIndex = static_cast<int>(floor((position.y - mTopBoundary) / mTileHeight));

    if (tileXIndex < 0 || tileXIndex >= mMapColumns ||
        tileYIndex < 0 || tileYIndex >= mMapRows)
        return false;

    const int tile = static_cast<int>(mLevelData[tileYIndex * mMapColumns + tileXIndex]);
    if (tile == 0) return false;

    const float tileCentreX = mLeftBoundary + tileXIndex * mTileWidth + mTileWidth / 2.0f;
    const float tileCentreY = mTopBoundary + tileYIndex * mTileHeight + mTileHeight / 2.0f;

    *xOverlap = fmaxf(0.0f, (mTileWidth / 2.0f) - fabs(position.x - tileCentreX));
    *yOverlap = fmaxf(0.0f, (mTileHeight / 2.0f) - fabs(position.y - tileCentreY));

    return true;
}
