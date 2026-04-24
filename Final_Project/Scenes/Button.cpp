#include "Button.h"
#include "Scene.h"

namespace
{
    const char *getKeyLabel(int key)
    {
        switch (key)
        {
            case KEY_ONE: return "1";
            case KEY_TWO: return "2";
            case KEY_THREE: return "3";
            case KEY_FOUR: return "4";
            case KEY_FIVE: return "5";
            case KEY_SIX: return "6";
            case KEY_SEVEN: return "7";
            case KEY_EIGHT: return "8";
            case KEY_NINE: return "9";
            case KEY_ZERO: return "0";
            case KEY_Q: return "Q";
            case KEY_W: return "W";
            case KEY_E: return "E";
            case KEY_R: return "R";
            case KEY_T: return "T";
            case KEY_Y: return "Y";
            case KEY_U: return "U";
            case KEY_I: return "I";
            case KEY_O: return "O";
            case KEY_P: return "P";
            case KEY_A: return "A";
            case KEY_S: return "S";
            case KEY_D: return "D";
            case KEY_F: return "F";
            case KEY_G: return "G";
            case KEY_H: return "H";
            case KEY_J: return "J";
            case KEY_K: return "K";
            case KEY_L: return "L";
            case KEY_Z: return "Z";
            case KEY_X: return "X";
            case KEY_C: return "C";
            case KEY_V: return "V";
            case KEY_B: return "B";
            case KEY_N: return "N";
            case KEY_M: return "M";
            default: return nullptr;
        }
    }

    Rectangle getPlayerBounds(Entity *player)
    {
        const Vector2 playerPosition = player->getPosition();
        const Vector2 playerCollider = player->getColliderDimensions();
        return
        {
            playerPosition.x - (playerCollider.x / 2.0f),
            playerPosition.y - (playerCollider.y / 2.0f),
            playerCollider.x,
            playerCollider.y
        };
    }

    Rectangle getEntityBounds(const Entity *entity)
    {
        const Vector2 position = entity->getPosition();
        const Vector2 size = entity->getScale();
        return
        {
            position.x - (size.x / 2.0f),
            position.y - (size.y / 2.0f),
            size.x,
            size.y
        };
    }
}

Button::Button(Rectangle bounds, const char *label, int targetSceneID, int key)
    : Entity(),
      mLabel(label ? label : ""),
      mTargetSceneID(targetSceneID),
      mKey(key)
{
    const Vector2 center = {
        bounds.x + (bounds.width / 2.0f),
        bounds.y + (bounds.height / 2.0f)
    };
    const Vector2 size = { bounds.width, bounds.height };
    setPosition(center);
    setScale(size);
    setColliderDimensions(size);
}

Button::Button(Rectangle bounds, const char *skinTextureFilepath)
    : Entity(
        { bounds.x + (bounds.width / 2.0f), bounds.y + (bounds.height / 2.0f) },
        { bounds.width, bounds.height },
        skinTextureFilepath
    ),
      mSkinTextureFilepath(skinTextureFilepath ? skinTextureFilepath : "")
{
}

Button::Button(Rectangle bounds, const char *skinTextureFilepath, int targetSceneID, int key, bool isTextureButton)
    : Entity(
        { bounds.x + (bounds.width / 2.0f), bounds.y + (bounds.height / 2.0f) },
        { bounds.width, bounds.height },
        skinTextureFilepath
    ),
      mSkinTextureFilepath(skinTextureFilepath ? skinTextureFilepath : ""),
      mTargetSceneID(targetSceneID),
      mKey(key)
{
    (void)isTextureButton;
}

void Button::setTextButtonColors(Color fillInactive, Color fillActive, Color textInactive, Color textActive)
{
    mTextButtonFillInactive = fillInactive;
    mTextButtonFillActive = fillActive;
    mTextButtonTextInactive = textInactive;
    mTextButtonTextActive = textActive;
}

void Button::processInput(Scene &scene)
{
    mIsActivated = false;
    mIsActive = CheckCollisionRecs(getPlayerBounds(scene.getPlayer()), getEntityBounds(this));
    if ((mIsActive && IsKeyPressed(KEY_ENTER)) || (mKey != KEY_NULL && IsKeyPressed(mKey))) mIsActivated = true;
}

void Button::update(Scene &scene)
{
    mIsActive = CheckCollisionRecs(getPlayerBounds(scene.getPlayer()), getEntityBounds(this));
}

void Button::render() const
{
    std::string keyLabel = getKeyLabel(mKey) ? std::string("(") + getKeyLabel(mKey) + ")" : "";


    if (!mSkinTextureFilepath.empty())
    {
        const Rectangle buttonBounds = getEntityBounds(this);
        const float borderThickness = mIsActive ? 5.0f : 2.0f;
        const Color borderColour = mIsActive ? SKYBLUE : WHITE;
        const Color backgroundColour = mIsActive ? Fade(SKYBLUE, 0.25f) : Fade(BLACK, 0.45f);
        const float padding = 8.0f;
        const Texture2D texture = getTexture();

        const Rectangle destination = {
            buttonBounds.x + padding,
            buttonBounds.y + padding,
            buttonBounds.width - (padding * 2.0f),
            buttonBounds.height - (padding * 2.0f)
        };

        const Rectangle source = {
            0.0f,
            0.0f,
            texture.width > texture.height
                ? static_cast<float>(texture.height)
                : static_cast<float>(texture.width),
            static_cast<float>(texture.height)
        };

        DrawRectangleRounded(buttonBounds, 0.2f, 8, backgroundColour);
        DrawTexturePro(texture, source, destination, { 0.0f, 0.0f }, 0.0f, WHITE);
        DrawRectangleRoundedLinesEx(buttonBounds, 0.2f, 8, borderThickness, borderColour);

        if (!keyLabel.empty())
        {
            const int keyFontSize = 16;
            const int keyTextWidth = MeasureText(keyLabel.c_str(), keyFontSize);
            const int keyX = static_cast<int>(buttonBounds.x + buttonBounds.width - keyTextWidth - 8.0f);
            const int keyY = static_cast<int>(buttonBounds.y + buttonBounds.height - keyFontSize - 6.0f);
            DrawText(keyLabel.c_str(), keyX, keyY, keyFontSize, WHITE);
        }
        return;
    }

    const Rectangle buttonBounds = getEntityBounds(this);
    const Color fill = mIsActive ? mTextButtonFillActive : mTextButtonFillInactive;
    const Color textColour = mIsActive ? mTextButtonTextActive : mTextButtonTextInactive;
    const int fontSize = mTextFontSize;
    const int textWidth = MeasureText(mLabel.c_str(), fontSize);
    const int textX = static_cast<int>(buttonBounds.x + (buttonBounds.width - textWidth) / 2.0f);
    const int textY = static_cast<int>(buttonBounds.y + 18.0f);

    DrawRectangleRounded(buttonBounds, 0.2f, 8, fill);
    DrawRectangleRoundedLinesEx(buttonBounds, 0.2f, 8, 3.0f, WHITE);
    DrawText(mLabel.c_str(), textX, textY, fontSize, textColour);

    if (!keyLabel.empty())
    {
        const int keyFontSize = 16;
        const int keyTextWidth = MeasureText(keyLabel.c_str(), keyFontSize);
        const int keyX = static_cast<int>(buttonBounds.x + buttonBounds.width - keyTextWidth - 8.0f);
        const int keyY = static_cast<int>(buttonBounds.y + buttonBounds.height - keyFontSize - 6.0f);
        DrawText(keyLabel.c_str(), keyX, keyY, keyFontSize, textColour);
    }
}
