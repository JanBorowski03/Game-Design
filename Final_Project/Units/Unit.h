#ifndef UNIT_H
#define UNIT_H

#include "../CS3113/Entity.h"
#include <cmath>
#include <string>

enum UnitTier
{
    UNIT_TIER_1 = 1,
    UNIT_TIER_2,
    UNIT_TIER_3,
    UNIT_TIER_4
};

class Unit : public Entity
{
private:
    int mTier = UNIT_TIER_1;
    int mStarLevel = 1;
    int mHealth = 100;
    int mMaxHealth = 100;
    int mDamage = 10;
    int mDefense = 0;
    float mAttackSpeed = 1.0f;
    float mRange = 1.0f;
    int mCost = 1;
    std::string mUnitName = "Unit";
    std::string mTextureFilepath = "";
    std::string mPortraitFilepath = "";

public:
    static constexpr Vector2 DEFAULT_SCALE = { 80.0f, 80.0f };

    Unit(
        Vector2 position = { 0.0f, 0.0f },
        Vector2 scale = DEFAULT_SCALE,
        const std::string &unitName = "Unit",
        const std::string &textureFilepath = "",
        const std::string &portraitFilepath = "",
        int tier = UNIT_TIER_1,
        int maxHealth = 100,
        int damage = 10,
        int defense = 0,
        float attackSpeed = 1.0f,
        float range = 1.0f,
        int cost = 1
    ) : Entity(
            position,
            scale,
            (textureFilepath.empty() ? ("assets/units/" + unitName + ".png") : textureFilepath).c_str()
        ),
        mUnitName(unitName),
        mTextureFilepath(textureFilepath.empty() ? ("assets/units/" + unitName + ".png") : textureFilepath),
        mPortraitFilepath(portraitFilepath.empty() ? ("assets/units/" + unitName + " - Copy.png") : portraitFilepath)
    {
        mTier = tier;
        mMaxHealth = maxHealth > 1 ? maxHealth : 1;
        mHealth = mMaxHealth;
        mDamage = damage >= 0 ? damage : 0;
        mDefense = defense >= 0 ? defense : 0;
        mAttackSpeed = attackSpeed > 0.0f ? attackSpeed : 0.1f;
        mRange = range >= 0.0f ? range : 0.0f;
        mCost = cost >= 0 ? cost : 0;
        setFlipOnLeftDirection(true);
        setGroundShadowEnabled(true);
    }

    int getTier() const { return mTier; }
    int getStarLevel() const { return mStarLevel; }
    const std::string &getUnitName() const { return mUnitName; }
    const char *getPortraitFilepath() const { return mPortraitFilepath.c_str(); }
    const char *getTextureFilepath() const { return mTextureFilepath.c_str(); }
    int getHealth() const { return mHealth; }
    int getMaxHealth() const { return mMaxHealth; }
    int getDamage() const { return mDamage; }
    int getDefense() const { return mDefense; }
    float getAttackSpeed() const { return mAttackSpeed; }
    float getRange() const { return mRange; }
    int getCost() const { return mCost; }
    bool isDead() const { return mHealth <= 0; }

    void setTier(int tier) { mTier = tier; }
    void setStarLevel(int starLevel) { mStarLevel = starLevel >= 1 ? starLevel : 1; }
    void setDamage(int damage) { mDamage = damage >= 0 ? damage : 0; }
    void setDefense(int defense) { mDefense = defense >= 0 ? defense : 0; }
    void setAttackSpeed(float attackSpeed) { mAttackSpeed = attackSpeed > 0.0f ? attackSpeed : 0.1f; }
    void setRange(float range) { mRange = range >= 0.0f ? range : 0.0f; }
    void setCost(int cost) { mCost = cost >= 0 ? cost : 0; }

    void setMaxHealth(int maxHealth)
    {
        mMaxHealth = maxHealth > 1 ? maxHealth : 1;
        if (mHealth > mMaxHealth) mHealth = mMaxHealth;
    }

    void setHealth(int health)
    {
        if (health < 0) health = 0;
        if (health > mMaxHealth) health = mMaxHealth;
        mHealth = health;
    }

    void takeDamage(int incomingDamage)
    {
        if (incomingDamage <= 0) return;

        int mitigatedDamage = incomingDamage - mDefense;
        if (mitigatedDamage < 1) mitigatedDamage = 1;

        setHealth(mHealth - mitigatedDamage);
    }

    void heal(int amount)
    {
        if (amount <= 0) return;
        setHealth(mHealth + amount);
    }

    void promoteStarLevel()
    {
        mStarLevel++;

        mMaxHealth = std::max(1, static_cast<int>(std::round(static_cast<float>(mMaxHealth) * 1.5f)));
        mHealth = mMaxHealth;
        mDamage = std::max(0, static_cast<int>(std::round(static_cast<float>(mDamage) * 1.5f)));

        Vector2 upgradedScale = getScale();
        upgradedScale.x *= 1.2f;
        upgradedScale.y *= 1.2f;
        setScale(upgradedScale);
        setColliderDimensions(upgradedScale);
    }

    void render() override;
};

#endif
