#ifndef BUTTON_H
#define BUTTON_H

#include "../CS3113/Entity.h"
#include <functional>
#include <string>

class Scene;

class Button : public Entity
{
private:
    std::string mLabel = "";
    std::string mSkinTextureFilepath = "";
    int mTargetSceneID = -1;
    int mKey = KEY_NULL;
    bool mIsActive = false;
    bool mIsActivated = false;
    int mTextFontSize = 28;
    Color mTextButtonFillInactive = DARKBLUE;
    Color mTextButtonFillActive = SKYBLUE;
    Color mTextButtonTextInactive = WHITE;
    Color mTextButtonTextActive = BLACK;

public:
    Button(Rectangle bounds, const char *label, int targetSceneID, int key = KEY_NULL);
    Button(Rectangle bounds, const char *skinTextureFilepath, int targetSceneID, int key, bool isTextureButton);
    Button(Rectangle bounds, const char *skinTextureFilepath);
    virtual ~Button() = default;

    virtual void processInput(Scene &scene);
    virtual void update(Scene &scene);
    virtual void render() const;
    bool isActive() const { return mIsActive; }
    bool isActivated() const { return mIsActivated; }
    void deactivate() { mIsActivated = false; }
    int getKey() const { return mKey; }
    int getTargetSceneID() const { return mTargetSceneID; }
    const std::string &getSkinTextureFilepath() const { return mSkinTextureFilepath; }
    void setTextFontSize(int fontSize) { if (fontSize > 0) mTextFontSize = fontSize; }
    void setTextButtonColors(Color fillInactive, Color fillActive, Color textInactive, Color textActive);
    void setKey(int key) { mKey = key; }
};

#endif
