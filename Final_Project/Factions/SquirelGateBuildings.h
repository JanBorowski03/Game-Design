#ifndef SQUIREL_GATE_BUILDINGS_H
#define SQUIREL_GATE_BUILDINGS_H

#include "Building.h"
#include "../LocalGameState.h"

class GatehouseBuilding : public Building
{
public:
    GatehouseBuilding() : Building("Gatehouse", "Primary fortification that secures the trunk roads and enables all core construction.", 5) {}
};

class AcornVaultBuilding : public Building
{
public:
    AcornVaultBuilding() : Building("Acorn Vault", "Hardened storage chambers that improve food reserves and unlock economy growth.", 7)
    {
        addRequirement("Gatehouse");
    }

    void onShopRoundStart(LocalGameState &gameState) override
    {
        gameState.addGold(2);
    }
};

class BarkBarracksBuilding : public Building
{
public:
    BarkBarracksBuilding() : Building("Bark Barracks", "Training camp for frontline squirrel infantry and militia rotations.", 8)
    {
        addRequirement("Gatehouse");
    }

    void onUnitPurchased(LocalGameState &gameState, Unit &purchasedUnit) override
    {
        (void)gameState;
        purchasedUnit.setDamage(purchasedUnit.getDamage() + 2);
    }
};

class SapperBurrowBuilding : public Building
{
public:
    SapperBurrowBuilding() : Building("Sapper Burrow", "Engineering tunnels where sappers prepare traps and breach tools.", 9)
    {
        addRequirement("Bark Barracks");
    }

    void onUnitPurchased(LocalGameState &gameState, Unit &purchasedUnit) override
    {
        (void)gameState;
        purchasedUnit.setDefense(purchasedUnit.getDefense() + 1);
    }
};

class GliderRoostBuilding : public Building
{
public:
    GliderRoostBuilding() : Building("Glider Roost", "High perches for glider scouts that provide rapid aerial movement.", 10)
    {
        addRequirement("Bark Barracks");
    }

    void onUnitPurchased(LocalGameState &gameState, Unit &purchasedUnit) override
    {
        (void)gameState;
        purchasedUnit.setAttackSpeed(purchasedUnit.getAttackSpeed() * 1.10f);
    }
};

class ResinWorkshopBuilding : public Building
{
public:
    ResinWorkshopBuilding() : Building("Resin Workshop", "Craft hall that refines tree resin into armor sealant and adhesives.", 10)
    {
        addRequirement("Acorn Vault");
    }

    void onUnitPurchased(LocalGameState &gameState, Unit &purchasedUnit) override
    {
        gameState.boostPurchasedUnitHealth(purchasedUnit, 1.15f);
    }
};

class TwigArtilleryBuilding : public Building
{
public:
    TwigArtilleryBuilding() : Building("Twig Artillery", "Siege platform that launches sharpened twigs and resin payloads.", 11)
    {
        addRequirement("Resin Workshop");
    }

    void onUnitPurchased(LocalGameState &gameState, Unit &purchasedUnit) override
    {
        gameState.boostPurchasedUnitDamage(purchasedUnit, 1.20f);
    }
};

class NutMarketBuilding : public Building
{
public:
    NutMarketBuilding() : Building("Nut Market", "Trade quarter where merchants exchange nuts, herbs, and salvage.", 8)
    {
        addRequirement("Acorn Vault");
    }

    void onShopRoundStart(LocalGameState &gameState) override
    {
        gameState.addGold(2);
        gameState.addDarkGold(1);
    }
};

class ScoutPostBuilding : public Building
{
public:
    ScoutPostBuilding() : Building("Scout Post", "Forward post that coordinates patrol routes and enemy sightings.", 7)
    {
        addRequirement("Gatehouse");
    }

    void onUnitPurchased(LocalGameState &gameState, Unit &purchasedUnit) override
    {
        (void)gameState;
        purchasedUnit.setRange(purchasedUnit.getRange() + 0.5f);
    }
};

class CanopyLibraryBuilding : public Building
{
public:
    CanopyLibraryBuilding() : Building("Canopy Library", "Archive of maps and battle records used for tactical planning.", 11)
    {
        addRequirement("Scout Post");
    }

    void onShopRoundStart(LocalGameState &gameState) override
    {
        gameState.setPlayerShopTier(2);
    }
};

class HerbalDenBuilding : public Building
{
public:
    HerbalDenBuilding() : Building("Herbal Den", "Alchemical den producing poultices and restorative bark tonics.", 9)
    {
        addRequirement("Nut Market");
    }

    void onUnitPurchased(LocalGameState &gameState, Unit &purchasedUnit) override
    {
        gameState.boostPurchasedUnitHealth(purchasedUnit, 1.12f);
    }
};

class MoonwatchTowerBuilding : public Building
{
public:
    MoonwatchTowerBuilding() : Building("Moonwatch Tower", "Night watch tower with signal fires for long-range alerts.", 12)
    {
        addRequirement("Canopy Library");
    }

    void onShopRoundStart(LocalGameState &gameState) override
    {
        gameState.setPlayerShopTier(3);
    }
};

class RootForgeBuilding : public Building
{
public:
    RootForgeBuilding() : Building("Root Forge", "Deep forge that tempers metal scraps into weapons and fittings.", 12)
    {
        addRequirement("Resin Workshop");
    }

    void onUnitPurchased(LocalGameState &gameState, Unit &purchasedUnit) override
    {
        gameState.boostPurchasedUnitDamage(purchasedUnit, 1.25f);
    }
};

class ElderOaksCouncilBuilding : public Building
{
public:
    ElderOaksCouncilBuilding() : Building("Elder Oaks Council", "Leadership hall where elders approve advanced war directives.", 13)
    {
        addRequirement("Moonwatch Tower");
    }

    void onShopRoundStart(LocalGameState &gameState) override
    {
        gameState.addGold(3);
        gameState.setPlayerShopTier(3);
    }
};

class GreatNestCitadelBuilding : public Building
{
public:
    GreatNestCitadelBuilding() : Building("Great Nest Citadel", "Grand citadel and final stronghold of SquirelGate command.", 15)
    {
        addRequirement("Elder Oaks Council");
        addRequirement("Root Forge");
    }

    void onShopRoundStart(LocalGameState &gameState) override
    {
        gameState.addGold(4);
        gameState.addDarkGold(2);
        gameState.setPlayerShopTier(4);
    }

    void onUnitPurchased(LocalGameState &gameState, Unit &purchasedUnit) override
    {
        gameState.boostPurchasedUnitHealth(purchasedUnit, 1.10f);
        gameState.boostPurchasedUnitDamage(purchasedUnit, 1.10f);
    }
};

#endif
