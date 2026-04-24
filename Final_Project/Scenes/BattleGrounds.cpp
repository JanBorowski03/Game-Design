#include "BattleGrounds.h"
#include "Button.h"
#include "../Units/CommonUnit.h"
#include "../Units/EpicUnit.h"
#include "../Units/LegendaryUnit.h"
#include "../Units/RareUnit.h"
#include "../Units/Unit.h"
#include <algorithm>
#include <cmath>

namespace
{
    constexpr int ENCOUNTER_WINS_TO_WIN = 10;
    constexpr float ATTACK_RANGE_TILE_SCALE = 90.0f;
    constexpr float MOVE_ACTION_DURATION = 0.30f;
    constexpr float MOVE_DISTANCE_PER_ACTION = 14.0f;
    constexpr float REPLAN_DELAY = 0.08f;
    constexpr int MAX_ACTIONS_PER_FRAME = 8;
    constexpr const char *SWORD_CLASH_SOUND_FILEPATH = "assets/sounds/sword_clash_.wav";
    constexpr const char *DEATH_SOUND_FILEPATH = "assets/sounds/death.wav";
}

BattleGrounds::BattleGrounds(Vector2 origin, const char *bgHexCode)
    : GameScene(origin, bgHexCode)
{
    setNextSceneID(SHOP_SCENE_ID);
}

BattleGrounds::~BattleGrounds()
{
    if (mSwordClashSoundLoaded)
    {
        StopSound(mSwordClashSound);
        UnloadSound(mSwordClashSound);
        mSwordClashSoundLoaded = false;
    }

    if (mDeathSoundLoaded)
    {
        StopSound(mDeathSound);
        UnloadSound(mDeathSound);
        mDeathSoundLoaded = false;
    }
}

void BattleGrounds::initialise()
{
    GameScene::initialise();
    mEncounterResolved = false;
    if (!mSwordClashSoundLoaded)
    {
        mSwordClashSound = LoadSound(SWORD_CLASH_SOUND_FILEPATH);
        mSwordClashSoundLoaded = (mSwordClashSound.stream.buffer != nullptr);
        if (mSwordClashSoundLoaded) SetSoundVolume(mSwordClashSound, 0.55f);
    }
    if (!mDeathSoundLoaded)
    {
        mDeathSound = LoadSound(DEATH_SOUND_FILEPATH);
        mDeathSoundLoaded = (mDeathSound.stream.buffer != nullptr);
        if (mDeathSoundLoaded) SetSoundVolume(mDeathSound, 0.65f);
    }
    mBoardView.initialise();
    initialiseEnemyBoard();
    mPlayerBoard = mLocalGameState ? &mLocalGameState->getPlayerBoardUnits() : nullptr;
    mEnemyBoard = &mEnemyBoardStorage;
    initialiseUnitPositions();
    resetEncounterQueue();
    queueInitialActions();
    syncHudFromLocalPlayer();
}

void BattleGrounds::update(float deltaTime)
{
    GameScene::update(deltaTime);
    if (!mEncounterResolved) clearSceneChangeRequest();
    syncHudFromLocalPlayer();
    mPlayerBoard = mLocalGameState ? &mLocalGameState->getPlayerBoardUnits() : nullptr;
    if (mEncounterActive)
    {
        mEncounterTime += deltaTime;
        processEncounterQueue();
    }
    resolveEncounterOutcomeIfNeeded();
}

void BattleGrounds::render()
{
    drawBackground();
    mBoardView.renderBoard(mEnemyBoard, false);
    renderUnitsFromPointers();

    for (const Button *button : mButtons)
    {
        if (button) button->render();
    }
    if (mPlayer) mPlayer->render();

    DrawText("Enemy Encounter", 340, 72, 44, WHITE);
    DrawText("Encounter resolves automatically", 315, 122, 24, GREEN);
    drawTopCenterResourceBar();
}

void BattleGrounds::shutdown()
{
    if (mSwordClashSoundLoaded && IsSoundPlaying(mSwordClashSound)) StopSound(mSwordClashSound);
    if (mDeathSoundLoaded && IsSoundPlaying(mDeathSound)) StopSound(mDeathSound);
    resetEncounterQueue();
    mPlayerBoard = nullptr;
    mEnemyBoard = nullptr;
    mEnemyBoardStorage.clear();
    mEnemyOwnedUnits.clear();
    mBoardView.shutdown();
    GameScene::shutdown();
}

void BattleGrounds::syncHudFromLocalPlayer()
{
    LocalPlayerState *localPlayer = mLocalGameState ? mLocalGameState->getLocalPlayerState() : nullptr;
    if (!localPlayer) return;

    setGold(localPlayer->gold);
    setDarkGold(localPlayer->darkGold);
    setPlayerHealth(localPlayer->health);
}

void BattleGrounds::initialiseEnemyBoard()
{
    const int enemySlotCount = mBoardView.getEnemyFogSlotCount();
    mEnemyBoardStorage.assign(static_cast<size_t>(enemySlotCount), nullptr);
    mEnemyOwnedUnits.clear();

    auto pushEnemy = [&](std::unique_ptr<Unit> enemy, int slotIndex)
    {
        if (!enemy) return;
        if (slotIndex < 0 || slotIndex >= static_cast<int>(mEnemyBoardStorage.size())) return;
        Unit *raw = enemy.get();
        mEnemyOwnedUnits.push_back(std::move(enemy));
        mEnemyBoardStorage[static_cast<size_t>(slotIndex)] = raw;
    };

    pushEnemy(std::unique_ptr<Unit>(new CommonUnit({ 0.0f, 0.0f })), 1);
    pushEnemy(std::unique_ptr<Unit>(new RareUnit({ 0.0f, 0.0f })), 2);
    pushEnemy(std::unique_ptr<Unit>(new CommonUnit({ 0.0f, 0.0f })), 4);
    pushEnemy(std::unique_ptr<Unit>(new EpicUnit({ 0.0f, 0.0f })), 6);
    pushEnemy(std::unique_ptr<Unit>(new RareUnit({ 0.0f, 0.0f })), 9);
    pushEnemy(std::unique_ptr<Unit>(new CommonUnit({ 0.0f, 0.0f })), 11);
    pushEnemy(std::unique_ptr<Unit>(new RareUnit({ 0.0f, 0.0f })), 13);
    pushEnemy(std::unique_ptr<Unit>(new LegendaryUnit({ 0.0f, 0.0f })), 14);
}

void BattleGrounds::initialiseUnitPositions()
{
    if (mPlayerBoard)
    {
        for (int i = 0; i < static_cast<int>(mPlayerBoard->size()); i++)
        {
            Unit *unit = (*mPlayerBoard)[static_cast<size_t>(i)];
            if (!unit) continue;
            const Rectangle tile = mBoardView.getPlayerFieldSlotBounds(i);
            if (tile.width <= 0.0f || tile.height <= 0.0f) continue;
            unit->setPosition({ tile.x + (tile.width / 2.0f), tile.y + (tile.height / 2.0f) });
            unit->setDirection(RIGHT);
        }
    }

    if (mEnemyBoard)
    {
        for (int i = 0; i < static_cast<int>(mEnemyBoard->size()); i++)
        {
            Unit *unit = (*mEnemyBoard)[static_cast<size_t>(i)];
            if (!unit) continue;
            const Rectangle tile = mBoardView.getEnemyFieldSlotBounds(i);
            if (tile.width <= 0.0f || tile.height <= 0.0f) continue;
            unit->setPosition({ tile.x + (tile.width / 2.0f), tile.y + (tile.height / 2.0f) });
            unit->setDirection(LEFT);
        }
    }
}

void BattleGrounds::renderUnitsFromPointers() const
{
    if (mPlayerBoard)
    {
        for (int i = 0; i < static_cast<int>(mPlayerBoard->size()); i++)
        {
            Unit *unit = (*mPlayerBoard)[static_cast<size_t>(i)];
            if (!unit) continue;
            unit->render();
        }
    }

    if (mEnemyBoard)
    {
        for (int i = 0; i < static_cast<int>(mEnemyBoard->size()); i++)
        {
            Unit *unit = (*mEnemyBoard)[static_cast<size_t>(i)];
            if (!unit) continue;
            unit->render();
        }
    }
}

bool BattleGrounds::isEnemyUnit(const Unit *unit) const
{
    if (!unit || !mEnemyBoard) return false;
    for (Unit *enemy : *mEnemyBoard)
    {
        if (enemy == unit) return true;
    }
    return false;
}

bool BattleGrounds::isUnitAlive(const Unit *unit) const
{
    return unit && !unit->isDead();
}

int BattleGrounds::countLivingUnits(bool enemyUnits) const
{
    const std::vector<Unit *> *units = enemyUnits ? mEnemyBoard : mPlayerBoard;
    if (!units) return 0;

    int living = 0;
    for (Unit *unit : *units)
    {
        if (isUnitAlive(unit)) living++;
    }
    return living;
}

Unit *BattleGrounds::findClosestLivingTarget(Unit *actor, bool actorIsEnemy, float *outDistance) const
{
    if (outDistance) *outDistance = 0.0f;
    if (!actor) return nullptr;

    const std::vector<Unit *> *candidates = actorIsEnemy ? mPlayerBoard : mEnemyBoard;
    if (!candidates) return nullptr;

    Unit *bestTarget = nullptr;
    float bestDistance = 0.0f;
    for (Unit *candidate : *candidates)
    {
        if (!isUnitAlive(candidate)) continue;

        const float distance = Vector2Distance(actor->getPosition(), candidate->getPosition());
        if (!bestTarget || distance < bestDistance)
        {
            bestTarget = candidate;
            bestDistance = distance;
        }
    }

    if (bestTarget && outDistance) *outDistance = bestDistance;
    return bestTarget;
}

Vector2 BattleGrounds::resolveCollisionFreeDestination(Unit *actor, Vector2 desiredDestination) const
{
    if (!actor) return desiredDestination;

    const float actorRadius = std::max(actor->getColliderDimensions().x, actor->getColliderDimensions().y) * 0.45f;
    if (actorRadius <= 0.0f) return desiredDestination;

    Vector2 resolved = desiredDestination;
    constexpr int MAX_PASSES = 3;

    auto pushOutOf = [&](Unit *other)
    {
        if (!other || other == actor || !isUnitAlive(other)) return;

        const float otherRadius = std::max(other->getColliderDimensions().x, other->getColliderDimensions().y) * 0.45f;
        if (otherRadius <= 0.0f) return;

        Vector2 delta = Vector2Subtract(resolved, other->getPosition());
        float distance = Vector2Length(delta);
        const float minDistance = actorRadius + otherRadius;

        if (distance >= minDistance) return;

        if (distance <= 0.001f)
        {
            delta = Vector2Subtract(actor->getPosition(), other->getPosition());
            distance = Vector2Length(delta);
            if (distance <= 0.001f) delta = { 1.0f, 0.0f };
        }

        const Vector2 direction = Vector2Normalize(delta);
        resolved = Vector2Add(other->getPosition(), Vector2Scale(direction, minDistance));
    };

    for (int pass = 0; pass < MAX_PASSES; pass++)
    {
        if (mPlayerBoard)
        {
            for (Unit *other : *mPlayerBoard) pushOutOf(other);
        }
        if (mEnemyBoard)
        {
            for (Unit *other : *mEnemyBoard) pushOutOf(other);
        }
    }

    return resolved;
}

void BattleGrounds::resetEncounterQueue()
{
    while (!mActionQueue.empty()) mActionQueue.pop();
    mEncounterTime = 0.0f;
    mNextActionSequenceID = 0;
    mEncounterActive = false;
    mEncounterResolved = false;
}

void BattleGrounds::queueInitialActions()
{
    if (!mPlayerBoard || !mEnemyBoard) return;

    for (Unit *unit : *mPlayerBoard)
    {
        if (!isUnitAlive(unit)) continue;
        queueNextActionFor(unit, false, 0.0f);
    }

    for (Unit *unit : *mEnemyBoard)
    {
        if (!isUnitAlive(unit)) continue;
        queueNextActionFor(unit, true, 0.0f);
    }

    mEncounterActive = !mActionQueue.empty();
}

void BattleGrounds::queueNextActionFor(Unit *actor, bool actorIsEnemy, float delaySeconds)
{
    if (!isUnitAlive(actor)) return;

    float distanceToTarget = 0.0f;
    Unit *target = findClosestLivingTarget(actor, actorIsEnemy, &distanceToTarget);
    if (!target) return;

    const float attackRange = std::max(24.0f, actor->getRange() * ATTACK_RANGE_TILE_SCALE);
    const bool canAttack = distanceToTarget <= attackRange;

    QueuedEncounterAction action = {};
    action.actor = actor;
    action.target = target;
    action.sequenceID = mNextActionSequenceID++;

    if (canAttack)
    {
        const float attackSpeed = std::max(0.1f, actor->getAttackSpeed());
        const float attackDuration = std::max(0.20f, 1.0f / attackSpeed);
        action.type = ENCOUNTER_ACTION_ATTACK;
        action.destination = actor->getPosition();
        action.endTime = mEncounterTime + delaySeconds + attackDuration;
    }
    else
    {
        const Vector2 actorPos = actor->getPosition();
        const Vector2 targetPos = target->getPosition();
        Vector2 direction = Vector2Subtract(targetPos, actorPos);
        const float magnitude = Vector2Length(direction);
        if (magnitude > 0.001f) direction = Vector2Scale(direction, 1.0f / magnitude);
        const float moveDistance = std::min(MOVE_DISTANCE_PER_ACTION, std::max(0.0f, distanceToTarget - attackRange));

        action.type = ENCOUNTER_ACTION_MOVE;
        const Vector2 desiredDestination = Vector2Add(actorPos, Vector2Scale(direction, moveDistance));
        action.destination = resolveCollisionFreeDestination(actor, desiredDestination);
        action.endTime = mEncounterTime + delaySeconds + MOVE_ACTION_DURATION;
    }

    mActionQueue.push(action);
}

void BattleGrounds::resolveQueuedAction(const QueuedEncounterAction &action)
{
    Unit *actor = action.actor;
    if (!isUnitAlive(actor)) return;

    const bool actorIsEnemy = isEnemyUnit(actor);
    if (!action.target || !isUnitAlive(action.target))
    {
        queueNextActionFor(actor, actorIsEnemy, REPLAN_DELAY);
        return;
    }

    if (action.type == ENCOUNTER_ACTION_MOVE)
    {
        actor->setPosition(resolveCollisionFreeDestination(actor, action.destination));
    }
    else
    {
        if (mSwordClashSoundLoaded)
        {
            if (IsSoundPlaying(mSwordClashSound)) StopSound(mSwordClashSound);
            PlaySound(mSwordClashSound);
        }
        action.target->takeDamage(actor->getDamage());
        if (action.target->isDead())
        {
            if (mDeathSoundLoaded)
            {
                if (IsSoundPlaying(mDeathSound)) StopSound(mDeathSound);
                PlaySound(mDeathSound);
            }
            action.target->deactivate();
        }
    }

    if (countLivingUnits(false) == 0 || countLivingUnits(true) == 0)
    {
        mEncounterActive = false;
        return;
    }

    queueNextActionFor(actor, actorIsEnemy, REPLAN_DELAY);
}

void BattleGrounds::processEncounterQueue()
{
    int processed = 0;
    while (!mActionQueue.empty() && processed < MAX_ACTIONS_PER_FRAME)
    {
        const QueuedEncounterAction next = mActionQueue.top();
        if (next.endTime > mEncounterTime) break;

        mActionQueue.pop();
        resolveQueuedAction(next);
        processed++;
    }

    if (mActionQueue.empty()) mEncounterActive = false;
}

void BattleGrounds::resolveEncounterOutcomeIfNeeded()
{
    if (mEncounterResolved || !mLocalGameState) return;

    const int playerUnitsLeft = countLivingUnits(false);
    const int enemyUnitsLeft = countLivingUnits(true);
    if (playerUnitsLeft > 0 && enemyUnitsLeft > 0) return;

    mEncounterResolved = true;

    LocalPlayerState *localPlayer = mLocalGameState->getLocalPlayerState();
    if (!localPlayer) return;

    mLocalGameState->restoreUnitsAfterEncounter();

    if (enemyUnitsLeft <= 0)
    {
        mLocalGameState->markEncounterWin();
        if (mLocalGameState->getEncounterNumber() > ENCOUNTER_WINS_TO_WIN)
        {
            requestSceneChange(WIN_SCENE_ID);
            return;
        }
        if (getNextSceneID() == SHOP_SCENE_ID) mLocalGameState->applyRoundStartBonuses();
        requestSceneChange(getNextSceneID());
        return;
    }

    mLocalGameState->applyEncounterLossDamage(enemyUnitsLeft);
    syncHudFromLocalPlayer();

    if (localPlayer->health <= 0)
    {
        requestSceneChange(LOSS_SCENE_ID);
        return;
    }

    if (getNextSceneID() == SHOP_SCENE_ID) mLocalGameState->applyRoundStartBonuses();
    requestSceneChange(getNextSceneID());
}
