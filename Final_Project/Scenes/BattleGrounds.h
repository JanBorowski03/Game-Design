#ifndef BATTLE_GROUNDS_H
#define BATTLE_GROUNDS_H

#include "BoardView.h"
#include "GameScene.h"
#include <queue>
#include <memory>
#include <vector>

class Unit;

class BattleGrounds : public GameScene
{
private:
    enum EncounterActionType
    {
        ENCOUNTER_ACTION_MOVE = 0,
        ENCOUNTER_ACTION_ATTACK
    };

    struct QueuedEncounterAction
    {
        EncounterActionType type = ENCOUNTER_ACTION_MOVE;
        Unit *actor = nullptr;
        Unit *target = nullptr;
        Vector2 destination = {};
        float endTime = 0.0f;
        int sequenceID = 0;
    };

    struct QueuedEncounterActionComparator
    {
        bool operator()(const QueuedEncounterAction &lhs, const QueuedEncounterAction &rhs) const
        {
            if (lhs.endTime == rhs.endTime) return lhs.sequenceID > rhs.sequenceID;
            return lhs.endTime > rhs.endTime;
        }
    };

    BoardView mBoardView;
    const std::vector<Unit *> *mPlayerBoard = nullptr;
    const std::vector<Unit *> *mEnemyBoard = nullptr;
    std::priority_queue<
        QueuedEncounterAction,
        std::vector<QueuedEncounterAction>,
        QueuedEncounterActionComparator
    > mActionQueue;
    float mEncounterTime = 0.0f;
    int mNextActionSequenceID = 0;
    bool mEncounterActive = false;
    bool mEncounterResolved = false;
    Sound mSwordClashSound = {};
    bool mSwordClashSoundLoaded = false;
    Sound mDeathSound = {};
    bool mDeathSoundLoaded = false;

    std::vector<std::unique_ptr<Unit>> mEnemyOwnedUnits;
    std::vector<Unit *> mEnemyBoardStorage;

    void syncHudFromLocalPlayer();
    void initialiseEnemyBoard();
    void initialiseUnitPositions();
    void renderUnitsFromPointers() const;
    bool isEnemyUnit(const Unit *unit) const;
    bool isUnitAlive(const Unit *unit) const;
    int countLivingUnits(bool enemyUnits) const;
    Unit *findClosestLivingTarget(Unit *actor, bool actorIsEnemy, float *outDistance) const;
    Vector2 resolveCollisionFreeDestination(Unit *actor, Vector2 desiredDestination) const;
    void resetEncounterQueue();
    void queueInitialActions();
    void queueNextActionFor(Unit *actor, bool actorIsEnemy, float delaySeconds);
    void resolveQueuedAction(const QueuedEncounterAction &action);
    void processEncounterQueue();
    void resolveEncounterOutcomeIfNeeded();

public:
    BattleGrounds(Vector2 origin, const char *bgHexCode);
    virtual ~BattleGrounds() override;

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif
