#include "../Scenes/Scene.h"

#ifndef EFFECTS_H
#define EFFECTS_H

enum EffectType { NONE, FADEIN, FADEOUT, SHRINK, GROW, UNIT_SHADOW };

class Effects
{
private:
    struct UnitShadowEffectOptions
    {
        Rectangle destination = {};
        float softness = 0.35f;
        float alpha = 0.40f;
        int layerCount = 10;
    };

    static UnitShadowEffectOptions sUnitShadowState;
    static void renderUnitShadow(const UnitShadowEffectOptions &options);

    float mAlpha;
    float mEffectSpeed;
    float mOverlayWidth;
    float mOverlayHeight;
    EffectType mCurrentEffect;
    Vector2 mViewOffset;
    Vector2 mOrigin;
    Vector2 mMaxWindowDimensions;

    void drawOverlay();
public:
    static constexpr float SOLID       = 1.0f,
                           TRANSPARENT = 0.0f,
                           DEFAULT_SPEED = 0.25f,
                           OVERLAY_MAX_SIZE = 1000.0f,
                           SIZE_SPEED_MULTIPLIER = 100.0f;

    Effects(Vector2 origin, float windowWidth, float windowHeight);

    void start(EffectType effectType);
    void update(float deltaTime, Vector2 *viewOffset);
    void render();
    static void renderEffect(EffectType effectType, const Rectangle &destination, float softness, float alpha);

    float      getAlpha()         const { return mAlpha;         }
    float      getEffectSpeed()   const { return mEffectSpeed;   }
    float      getOverlayWidth()  const { return mOverlayWidth;  }
    float      getOverlayHeight() const { return mOverlayHeight; }
    EffectType getCurrentEffect() const { return mCurrentEffect; }
    Vector2    getViewOffset()    const { return mViewOffset;    }
    Vector2    getOrigin()        const { return mOrigin;        }
    Vector2    getMaxWindowDimensions() const { return mMaxWindowDimensions; }

    void setAlpha(float alpha)                 { mAlpha = alpha;                 }
    void setEffectSpeed(float effectSpeed)     { mEffectSpeed = effectSpeed;     }
    void setOverlayWidth(float overlayWidth)   { mOverlayWidth = overlayWidth;   }
    void setOverlayHeight(float overlayHeight) { mOverlayHeight = overlayHeight; }
    void setCurrentEffect(EffectType effect)   { mCurrentEffect = effect;        }
    void setViewOffset(Vector2 offset)         { mViewOffset = offset;           }
    void setOrigin(Vector2 origin)             { mOrigin = origin;               }
};

#endif // EFFECTS_H
