#include "Effects.h"
#include <cmath>

Effects::UnitShadowEffectOptions Effects::sUnitShadowState = {};

void Effects::renderUnitShadow(const UnitShadowEffectOptions &options)
{
    if (options.destination.width <= 0.0f || options.destination.height <= 0.0f) return;

    const float clampedSoftness = fmaxf(0.05f, fminf(options.softness, 0.95f));
    const float clampedAlpha = fmaxf(0.0f, fminf(options.alpha, 1.0f));
    const int layers = options.layerCount < 3 ? 3 : options.layerCount;

    const float centerX = options.destination.x;
    const float centerY = options.destination.y;
    const float baseRadiusX = options.destination.width * 0.5f;
    const float baseRadiusY = options.destination.height * 0.5f;

    for (int i = 0; i < layers; i++)
    {
        const float t = static_cast<float>(i) / static_cast<float>(layers - 1);
        const float shrink = t * (0.75f + (0.20f * clampedSoftness));
        const float radiusX = fmaxf(0.0f, baseRadiusX * (1.0f - shrink));
        const float radiusY = fmaxf(0.0f, baseRadiusY * (1.0f - (shrink * 0.85f)));
        const float layerAlpha = clampedAlpha * powf(1.0f - t, 1.6f) * (0.7f + 0.3f * (1.0f - clampedSoftness));

        if (radiusX <= 0.0f || radiusY <= 0.0f || layerAlpha <= 0.0f) continue;

        DrawEllipse(
            static_cast<int>(centerX),
            static_cast<int>(centerY),
            radiusX,
            radiusY,
            Fade(BLACK, layerAlpha)
        );
    }
}

void Effects::renderEffect(EffectType effectType, const Rectangle &destination, float softness, float alpha)
{
    switch (effectType)
    {
        case UNIT_SHADOW:
        {
            sUnitShadowState.destination = destination;
            sUnitShadowState.softness = softness;
            sUnitShadowState.alpha = alpha;
            sUnitShadowState.layerCount = static_cast<int>(6.0f + (14.0f * fmaxf(0.05f, fminf(softness, 0.95f))));
            renderUnitShadow(sUnitShadowState);
            break;
        }

        case NONE:
        case FADEIN:
        case FADEOUT:
        case SHRINK:
        case GROW:
        default:
            break;
    }
}

Effects::Effects(Vector2 origin, float windowWidth, float windowHeight) : mAlpha{SOLID}, 
                     mEffectSpeed{DEFAULT_SPEED},
                     mOverlayWidth{windowWidth}, 
                     mOverlayHeight{windowHeight}, mCurrentEffect{NONE},
                     mViewOffset{{}}, mOrigin{origin}, 
                     mMaxWindowDimensions{windowWidth, windowHeight}
{
}

void Effects::drawOverlay()
{
    float left = mViewOffset.x - mOverlayWidth  / 2.0f;
    float top  = mOrigin.y - mOverlayHeight / 2.0f; // Adjustment for some reason

    DrawRectangle(left,
                  top,
                  mOverlayWidth,
                  mOverlayHeight,
                  Fade(BLACK, mAlpha));
}

void Effects::update(float deltaTime, Vector2 *viewOffset)
{
    if (viewOffset != nullptr) mViewOffset = *viewOffset;

    float diagonalRatio = mMaxWindowDimensions.y / mMaxWindowDimensions.x;

    switch (mCurrentEffect)
    {
        case FADEIN:
            mAlpha -= mEffectSpeed * deltaTime;

            if (mAlpha <= TRANSPARENT)
            {
                mAlpha = TRANSPARENT;
                mCurrentEffect = NONE;
            }

            break;
        
        case FADEOUT:
            mAlpha += mEffectSpeed * deltaTime;

            if (mAlpha >= SOLID)
            {
                mAlpha = SOLID;
                mCurrentEffect = NONE;
            }

            break;

        case SHRINK:
            mOverlayHeight -= mEffectSpeed * SIZE_SPEED_MULTIPLIER * deltaTime * diagonalRatio;
            mOverlayWidth  -= mEffectSpeed * SIZE_SPEED_MULTIPLIER * deltaTime;

            if (mOverlayHeight <= 0.0f ||
                mOverlayWidth  <= 0.0f)
            {
                mOverlayHeight = 0.0f;
                mOverlayWidth  = 0.0f;
                mCurrentEffect = NONE;
            }
            break;

        case GROW:
            mOverlayHeight += mEffectSpeed * SIZE_SPEED_MULTIPLIER * deltaTime * diagonalRatio;
            mOverlayWidth  += mEffectSpeed * SIZE_SPEED_MULTIPLIER * deltaTime;

            if (mOverlayHeight >= mMaxWindowDimensions.y ||
                mOverlayWidth  >= mMaxWindowDimensions.x)
            {
                mOverlayHeight = mMaxWindowDimensions.y;
                mOverlayWidth  = mMaxWindowDimensions.x;
                mCurrentEffect = NONE;
            }
            break;

        case UNIT_SHADOW:
        case NONE:
        default:
            break;
    }
}

void Effects::start(EffectType effectType)
{
    mCurrentEffect = effectType;

    switch (mCurrentEffect)
    {
        case FADEIN:
            mAlpha = SOLID;
            break;

        case FADEOUT:
            mAlpha = TRANSPARENT;
            break;

        case SHRINK:
            mOverlayHeight = mMaxWindowDimensions.y;
            mOverlayWidth  = mMaxWindowDimensions.x;
            break;

        case GROW:
            mOverlayHeight = 0.0f;
            mOverlayWidth  = 0.0f;
            break;

        case UNIT_SHADOW:
        case NONE:
        default:
            break;
    }
}

void Effects::render()
{
    switch (mCurrentEffect)
    {
        case FADEIN:
        case FADEOUT:
        case SHRINK:
        case GROW:
            drawOverlay();
            break;

        case UNIT_SHADOW:
        case NONE:
    
    default:
        break;
    }
}
