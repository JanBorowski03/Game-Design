#ifndef PLAYER_H
#define PLAYER_H

#include "CS3113/Entity.h"

class Player : public Entity
{
public:
    static constexpr Vector2 DEFAULT_SCALE = { 80.0f, 80.0f };
    static constexpr int DEFAULT_SPEED = 100;
    static constexpr float DEFAULT_JUMPING_POWER = 0.0f;
    static constexpr const char *DEFAULT_TEXTURE_FILEPATH = "assets/player_skins/biker.png";
    static constexpr int MOVING_FRAME_COUNT = 6;
    static constexpr int IDLE_FRAME_COUNT = 4;
    static constexpr const char *FOOTSTEP_SOUND_FILEPATH = "assets/sounds/footsteps.wav";
    int playerID = 0;
    Sound mFootstepSound = {};
    bool mFootstepSoundLoaded = false;

    Player(
        Vector2 position,
        Vector2 scale = DEFAULT_SCALE,
        const char *textureFilepath = DEFAULT_TEXTURE_FILEPATH
    ) : Entity(
            position,
            scale,
            textureFilepath,
            ATLAS,
            { 2.0f, 6.0f },
            {
                { LEFT,  { 0, 1, 2, 3, 4, 5 } },
                { UP,    { 0, 1, 2, 3, 4, 5 } },
                { RIGHT, { 0, 1, 2, 3, 4, 5 } },
                { DOWN,  { 0, 1, 2, 3, 4, 5 } }
            }
        )
    {
        setSpeed(DEFAULT_SPEED);
        setJumpingPower(DEFAULT_JUMPING_POWER);
        setAnimateWhenIdle(true);
        setFlipOnLeftDirection(true);
        setGroundShadowEnabled(true);


        mFootstepSound = LoadSound(FOOTSTEP_SOUND_FILEPATH);
        mFootstepSoundLoaded = (mFootstepSound.stream.buffer != nullptr);
        if (mFootstepSoundLoaded) SetSoundVolume(mFootstepSound, 0.30f);
    }

    ~Player() override
    {
        if (!mFootstepSoundLoaded) return;
        StopSound(mFootstepSound);
        UnloadSound(mFootstepSound);
        mFootstepSoundLoaded = false;
    }

    void update(float deltaTime, Entity *player, Map *map, Entity *collidableEntities, int collisionCheckCount)
    {
        const bool isMoving = GetLength(getMovement()) != 0;

        if (isMoving)
        {
            setAnimationIndices({ 0, 1, 2, 3, 4, 5 });
        }
        else
        {
            setAnimationIndices({ 6, 7, 8, 7 });
        }

        if (mFootstepSoundLoaded)
        {
            if (isMoving)
            {
                if (!IsSoundPlaying(mFootstepSound)) PlaySound(mFootstepSound);
            }
            else if (IsSoundPlaying(mFootstepSound))
            {
                StopSound(mFootstepSound);
            }
        }

        Entity::update(deltaTime, player, map, collidableEntities, collisionCheckCount);
    }

    void setPlayerID(int id) { playerID = id; }
    int getPlayerID() const { return playerID; }
};

#endif
