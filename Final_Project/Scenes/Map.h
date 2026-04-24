#include "../CS3113/cs3113.h"
#include <functional>

#ifndef MAP_H
#define MAP_H

class Map
{
private:
    int mMapColumns; // number of columns in map
    int mMapRows;    // number of rows in map

    unsigned int *mLevelData; // array of tile indices
    Texture2D mTextureAtlas;  // texture atlas

    float mTileWidth;  // width of each tile in pixels
    float mTileHeight; // height of each tile in pixels

    int mTextureColumns; // number of columns in texture atlas
    int mTextureRows;    // number of rows in texture atlas

    std::vector<Rectangle> mTextureAreas; // texture areas for each tile
    Vector2 mOrigin; // center of the map in world coordinates

    float mLeftBoundary;  // left boundary of the map in world coordinates
    float mRightBoundary; // right boundary of the map in world coordinates
    float mTopBoundary;   // top boundary of the map in world coordinates
    float mBottomBoundary;// bottom boundary of the map in world coordinates

public:
    Map(int mapColumns, int mapRows, unsigned int *levelData,
        const char *textureFilePath, float tileWidth, int textureColumns,
        int textureRows, Vector2 origin, float tileHeight = -1.0f);
    ~Map();

    void build();
    void render(const std::function<void(int, int, unsigned int, const Rectangle &)> &tileRenderer = nullptr);
    bool isSolidTileAt(Vector2 position, float *xOverlap, float *yOverlap);
    Rectangle getTileWorldBounds(int row, int col) const;

    int           getMapColumns()     const { return mMapColumns;     };
    int           getMapRows()        const { return mMapRows;        };
    float         getTileWidth()      const { return mTileWidth;      };
    float         getTileHeight()     const { return mTileHeight;     };
    unsigned int* getLevelData()      const { return mLevelData;      };
    Texture2D     getTextureAtlas()   const { return mTextureAtlas;   };
    int           getTextureColumns() const { return mTextureColumns; };
    int           getTextureRows()    const { return mTextureRows;    };
    float         getLeftBoundary()   const { return mLeftBoundary;   };
    float         getRightBoundary()  const { return mRightBoundary;  };
    float         getTopBoundary()    const { return mTopBoundary;    };
    float         getBottomBoundary() const { return mBottomBoundary; };
};

#endif
