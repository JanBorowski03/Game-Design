#include "CS3113/ShaderProgram.h"
#include "Scenes/MainMenu.h"
#include "Scenes/SkinSelection.h"
#include "Scenes/FactionSelection.h"
#include "Scenes/Shop.h"
#include "Scenes/Castle.h"
#include "Scenes/BattleGrounds.h"
#include "Scenes/WinScene.h"
#include "Scenes/LossScene.h"
#include "LocalGameState.h"
#include <array>
#include <string>
#include <vector>

// Global Constants
constexpr int SCREEN_WIDTH     = 1000,
              SCREEN_HEIGHT    = 600,
              FPS              = 120;

constexpr Vector2 ORIGIN = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
            
constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

// Global Variables
AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f;

LocalGameState *gLocalGameState = nullptr;
Scene *gCurrentScene = nullptr;
std::vector<Scene *> gScenes = {};

Effects *gEffects = nullptr;

Camera2D gCamera = { 0 };
ShaderProgram gShader;
Music gSceneMusic = {};
bool gSceneMusicLoaded = false;
std::string gCurrentMusicPath = "";

const char *getMusicPathForScene(int sceneID)
{
    switch (sceneID)
    {
        case MAIN_MENU_SCENE_ID:
        case SKIN_SELECTION_SCENE_ID:
        case FACTION_SELECTION_SCENE_ID:
        case WIN_SCENE_ID:
        case LOSS_SCENE_ID:
            return "assets/sounds/menu_and_faction_selection_bg_sound.wav";

        case SHOP_SCENE_ID:
        case CASTLE_SCENE_ID:
            return "assets/sounds/shop_and_castle_bg_sound.wav";

        case BATTLE_GROUNDS_SCENE_ID:
            return "assets/sounds/battle_bg_sound.wav";

        default:
            return nullptr;
    }
}

void playSceneMusic(int sceneID)
{
    const char *nextMusicPath = getMusicPathForScene(sceneID);

    const std::string requestedPath(nextMusicPath);
    if (gSceneMusicLoaded && gCurrentMusicPath == requestedPath)
    {
        if (!IsMusicStreamPlaying(gSceneMusic)) PlayMusicStream(gSceneMusic);
        return;
    }

    if (gSceneMusicLoaded)
    {
        StopMusicStream(gSceneMusic);
        UnloadMusicStream(gSceneMusic);
        gSceneMusicLoaded = false;
    }

    gSceneMusic = LoadMusicStream(nextMusicPath);
    gSceneMusicLoaded = (gSceneMusic.stream.buffer != nullptr);
    if (!gSceneMusicLoaded)
    {
        gCurrentMusicPath.clear();
        return;
    }

    gCurrentMusicPath = requestedPath;
    SetMusicVolume(gSceneMusic, 0.55f);
    PlayMusicStream(gSceneMusic);
}

// Function Declarations
void switchToScene(int sceneID);
void initialise();
void processInput();
void update();
void render();
void shutdown();

void switchToScene(int sceneID)
{
    if (gCurrentScene) gCurrentScene->shutdown();

    gCurrentScene = gScenes[sceneID];


    gCurrentScene->setGameState(gLocalGameState);
    gCurrentScene->setPlayer(gLocalGameState->getLocalPlayerState()->player);


    gCurrentScene->initialise();
    playSceneMusic(sceneID);

    gCamera.target = ORIGIN;
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Jan Borowski - Final Project");
    InitAudioDevice();

    gCamera.offset   = ORIGIN;
    gCamera.rotation = 0.0f;
    gCamera.zoom     = 1.0f;

    gLocalGameState = new LocalGameState();

    gScenes.resize(LOSS_SCENE_ID + 1, nullptr);
    gScenes[MAIN_MENU_SCENE_ID] = new MainMenu(ORIGIN, "#355834");
    gScenes[SKIN_SELECTION_SCENE_ID] = new SkinSelection(ORIGIN, "#355834");
    gScenes[FACTION_SELECTION_SCENE_ID] = new FactionSelection(ORIGIN, "#355834");
    gScenes[SHOP_SCENE_ID] = new Shop(ORIGIN, "#355834");
    gScenes[CASTLE_SCENE_ID] = new Castle(ORIGIN, "#355834");
    gScenes[BATTLE_GROUNDS_SCENE_ID] = new BattleGrounds(ORIGIN, "#355834");
    gScenes[WIN_SCENE_ID] = new WinScene(ORIGIN, "#355834");
    gScenes[LOSS_SCENE_ID] = new LossScene(ORIGIN, "#355834");

    gEffects = new Effects(ORIGIN, (float) SCREEN_WIDTH * 1.5f, (float) SCREEN_HEIGHT * 1.5f);

    switchToScene(MAIN_MENU_SCENE_ID);

    SetTargetFPS(FPS);
}

void processInput() 
{
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;

    std::array<bool, 512> keyStates = {};
    for (int key = 0; key < gCurrentScene->getKeyInputArraySize(); key++)
    {
        keyStates[key] = IsKeyDown(key);
    }

    gCurrentScene->processInput(keyStates.data(), gCurrentScene->getKeyInputArraySize());
}

void update() 
{
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        gCurrentScene->update(0.0f);
        gEffects->update(0.0f, &gCamera.target);
        if (gSceneMusicLoaded)
        {
            UpdateMusicStream(gSceneMusic);
            if (!IsMusicStreamPlaying(gSceneMusic)) PlayMusicStream(gSceneMusic);
        }
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        gCurrentScene->update(FIXED_TIMESTEP);
        gEffects->update(FIXED_TIMESTEP, &gCamera.target);

        deltaTime -= FIXED_TIMESTEP;
    }

    gTimeAccumulator = deltaTime;
    if (gSceneMusicLoaded)
    {
        UpdateMusicStream(gSceneMusic);
        if (!IsMusicStreamPlaying(gSceneMusic)) PlayMusicStream(gSceneMusic);
    }
}

void render()
{
    BeginDrawing();
    BeginMode2D(gCamera);
    gShader.begin();

    // gShader.setVector2("lightPosition", gLightPosition);
    gCurrentScene->render();

    gShader.end();
    gEffects->render();
    EndMode2D();
    EndDrawing();
}

void shutdown() 
{
    if (gCurrentScene) gCurrentScene->shutdown();

    for (Scene *scene : gScenes) delete scene;
    gScenes.clear();
    gCurrentScene = nullptr;

    delete gEffects;
    gEffects = nullptr;

    delete gLocalGameState;
    gLocalGameState = nullptr;

    gShader.unload();

    if (gSceneMusicLoaded)
    {
        StopMusicStream(gSceneMusic);
        UnloadMusicStream(gSceneMusic);
        gSceneMusicLoaded = false;
    }

    CloseAudioDevice();
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();

        if (gLocalGameState->isSceneChangeRequested())
        {
            const int nextSceneID = gLocalGameState->getNextSceneID();
            gLocalGameState->clearSceneChangeRequest();
            switchToScene(nextSceneID);
        }

        render();
    }

    shutdown();

    return 0;
}
