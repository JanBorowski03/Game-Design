#include "Castle.h"
#include "Button.h"
#include <algorithm>
#include <memory>
#include <string>
#include <vector>

namespace
{
    constexpr int MAX_COLUMNS = 4;
    constexpr float SLOT_WIDTH = 150.0f;
    constexpr float SLOT_HEIGHT = 74.0f;
    constexpr float SLOT_SPACING_X = 20.0f;
    constexpr float SLOT_SPACING_Y = 18.0f;
    constexpr float GRID_TOP_Y = 90.0f;
    constexpr float NAV_BUTTON_WIDTH = 180.0f;
    constexpr float NAV_BUTTON_HEIGHT = 44.0f;
    constexpr float NAV_BUTTON_MARGIN = 12.0f;
    constexpr float BOTTOM_PANEL_HEIGHT = 160.0f;
    constexpr float BOTTOM_PANEL_MARGIN = 12.0f;
    constexpr float BOTTOM_PANEL_IMAGE_SIZE = 120.0f;
    constexpr int BOTTOM_PANEL_TITLE_SIZE = 24;
    constexpr int BOTTOM_PANEL_TEXT_SIZE = 18;
    constexpr const char *BUILD_SOUND_FILEPATH = "assets/sounds/build_sound.wav";
    constexpr float STATUS_BORDER_THICKNESS = 4.0f;

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

    std::vector<std::string> wrapText(const std::string &text, int maxWidth, int fontSize)
    {
        std::vector<std::string> lines;
        if (text.empty() || maxWidth <= 0) return lines;

        std::string currentLine = "";
        std::string currentWord = "";

        const auto pushWord = [&](const std::string &word, std::string &line, std::vector<std::string> &output)
        {
            if (word.empty()) return;

            const std::string candidate = line.empty() ? word : (line + " " + word);
            if (MeasureText(candidate.c_str(), fontSize) <= maxWidth)
            {
                line = candidate;
            }
            else
            {
                if (!line.empty()) output.push_back(line);
                line = word;
            }
        };

        for (char ch : text)
        {
            if (ch == ' ')
            {
                pushWord(currentWord, currentLine, lines);
                currentWord.clear();
                continue;
            }

            if (ch == '\n')
            {
                pushWord(currentWord, currentLine, lines);
                currentWord.clear();
                if (!currentLine.empty()) lines.push_back(currentLine);
                currentLine.clear();
                continue;
            }

            currentWord.push_back(ch);
        }

        pushWord(currentWord, currentLine, lines);
        if (!currentLine.empty()) lines.push_back(currentLine);

        return lines;
    }
}

Castle::Castle(Vector2 origin, const char *bgHexCode)
    : GameScene(origin, bgHexCode)
{
}

void Castle::initialise()
{
    GameScene::initialise();
    if (!mBuildSoundLoaded)
    {
        mBuildSound = LoadSound(BUILD_SOUND_FILEPATH);
        mBuildSoundLoaded = (mBuildSound.stream.buffer != nullptr);
        if (mBuildSoundLoaded) SetSoundVolume(mBuildSound, 0.75f);
    }
    syncHudFromLocalPlayer();
    mBuildingButtons.clear();
    mSelectedBuildingIndex = -1;
    const LocalPlayerState *localPlayer = mLocalGameState ? mLocalGameState->getLocalPlayerState() : nullptr;
    Faction *faction = (localPlayer ? localPlayer->faction : nullptr);

    const float screenH = static_cast<float>(GetScreenHeight());
    const float backButtonX = NAV_BUTTON_MARGIN;
    const float backButtonY = NAV_BUTTON_MARGIN;

    const int buildingCount = faction ? faction->getNumberOfBuildings() : 0;
    for (int i = 0; i < buildingCount; i++)
    {
        const int row = i / MAX_COLUMNS;
        const int column = i % MAX_COLUMNS;
        const int remaining = buildingCount - (row * MAX_COLUMNS);
        const int columns = remaining > MAX_COLUMNS ? MAX_COLUMNS : remaining;
        const float rowWidth = (columns * SLOT_WIDTH) + ((columns - 1) * SLOT_SPACING_X);
        const float startX = (static_cast<float>(GetScreenWidth()) - rowWidth) / 2.0f;
        const float y = GRID_TOP_Y + (row * (SLOT_HEIGHT + SLOT_SPACING_Y));
        const float x = startX + (column * (SLOT_WIDTH + SLOT_SPACING_X));

        if (faction)
        {
            const std::string pictureFilepath = faction->getBuildingPictureFilepath(i);
            if (!pictureFilepath.empty() && FileExists(pictureFilepath.c_str()))
            {
                Button *buildingButton = new Button({ x, y, SLOT_WIDTH, SLOT_HEIGHT }, pictureFilepath.c_str());
                mButtons.push_back(buildingButton);
                mBuildingButtons.push_back(buildingButton);
                continue;
            }
        }

        std::string label = "Slot " + std::to_string(i + 1);
        if (faction && i < static_cast<int>(faction->getBuildings().size()))
        {
            const std::unique_ptr<Building> &building = faction->getBuildings()[i];
            if (building) label = building->getName();
        }
        Button *buildingButton = new Button({ x, y, SLOT_WIDTH, SLOT_HEIGHT }, label.c_str(), -1);
        mButtons.push_back(buildingButton);
        mBuildingButtons.push_back(buildingButton);
    }

    const std::string backButtonTexturePath = faction
        ? ("assets/factions/" + faction->getName() + "/Icon.png")
        : "";

    Button *backToShopButton = new Button(
        {
            backButtonX,
            backButtonY,
            NAV_BUTTON_WIDTH,
            NAV_BUTTON_HEIGHT
        },
        backButtonTexturePath.c_str(),
        SHOP_SCENE_ID,
        KEY_B,
        true
    );

    mButtons.push_back(backToShopButton);
}

void Castle::update(float deltaTime)
{
    GameScene::update(deltaTime);
    syncHudFromLocalPlayer();

    for (int i = 0; i < static_cast<int>(mBuildingButtons.size()); i++)
    {
        Button *button = mBuildingButtons[i];
        if (!button || !button->isActivated()) continue;

        if (mSelectedBuildingIndex != i)
        {
            mSelectedBuildingIndex = i;
        }
        else if (mLocalGameState)
        {
            const bool purchased = mLocalGameState->tryPurchaseBuilding(i);
            if (purchased && mBuildSoundLoaded)
            {
                if (IsSoundPlaying(mBuildSound)) StopSound(mBuildSound);
                PlaySound(mBuildSound);
            }
        }
    }
}

void Castle::render()
{
    GameScene::render();

    const LocalPlayerState *localPlayer = mLocalGameState ? mLocalGameState->getLocalPlayerState() : nullptr;
    const Faction *faction = (localPlayer ? localPlayer->faction : nullptr);
    if (faction)
    {
        const std::vector<std::unique_ptr<Building>> &buildings = faction->getBuildings();
        for (int i = 0; i < static_cast<int>(mBuildingButtons.size()) && i < static_cast<int>(buildings.size()); i++)
        {
            const Button *button = mBuildingButtons[static_cast<size_t>(i)];
            const std::unique_ptr<Building> &building = buildings[static_cast<size_t>(i)];
            if (!button || !building) continue;

            const Rectangle bounds = getEntityBounds(button);
            if (building->isBuilt())
            {
                DrawRectangleRoundedLinesEx(bounds, 0.2f, 8, STATUS_BORDER_THICKNESS, LIME);
                continue;
            }

            bool requirementsMet = true;
            const std::vector<std::string> &requirements = building->getRequirements();
            for (const std::string &requiredName : requirements)
            {
                const auto it = std::find_if(
                    buildings.begin(),
                    buildings.end(),
                    [&](const std::unique_ptr<Building> &candidate)
                    {
                        return candidate && candidate->getName() == requiredName;
                    }
                );

                if (it == buildings.end() || !(*it) || !(*it)->isBuilt())
                {
                    requirementsMet = false;
                    break;
                }
            }

            const bool canAfford = localPlayer && (localPlayer->darkGold >= building->getCost());
            if (!requirementsMet || !canAfford)
            {
                DrawRectangleRoundedLinesEx(bounds, 0.2f, 8, STATUS_BORDER_THICKNESS, RED);
            }
        }
    }

    DrawText("ENTER: Select Building | ENTER Again: Build | Cost Uses Dark Gold", 120, 70, 22, YELLOW);
    renderBuildingNavbar();
}

void Castle::renderBuildingNavbar() const
{
    if (mSelectedBuildingIndex < 0) return;

    const LocalPlayerState *localPlayer = mLocalGameState ? mLocalGameState->getLocalPlayerState() : nullptr;
    const Faction *faction = (localPlayer ? localPlayer->faction : nullptr);
    if (!faction) return;

    const std::vector<std::unique_ptr<Building>> &buildings = faction->getBuildings();
    if (mSelectedBuildingIndex >= static_cast<int>(buildings.size())) return;
    if (!buildings[mSelectedBuildingIndex]) return;

    const Building &selectedBuilding = *buildings[mSelectedBuildingIndex];
    const float screenWidth = static_cast<float>(GetScreenWidth());
    const float screenHeight = static_cast<float>(GetScreenHeight());
    const Rectangle panelBounds =
    {
        BOTTOM_PANEL_MARGIN,
        screenHeight - BOTTOM_PANEL_HEIGHT - BOTTOM_PANEL_MARGIN,
        screenWidth - (BOTTOM_PANEL_MARGIN * 2.0f),
        BOTTOM_PANEL_HEIGHT
    };

    DrawRectangleRounded(panelBounds, 0.1f, 8, Fade(BLACK, 0.82f));
    DrawRectangleRoundedLinesEx(panelBounds, 0.1f, 8, 2.0f, WHITE);

    const Rectangle imageBounds =
    {
        panelBounds.x + 14.0f,
        panelBounds.y + ((panelBounds.height - BOTTOM_PANEL_IMAGE_SIZE) / 2.0f),
        BOTTOM_PANEL_IMAGE_SIZE,
        BOTTOM_PANEL_IMAGE_SIZE
    };

    if (mSelectedBuildingIndex < static_cast<int>(mBuildingButtons.size()))
    {
        const Button *selectedButton = mBuildingButtons[mSelectedBuildingIndex];
        if (selectedButton && !selectedButton->getSkinTextureFilepath().empty())
        {
            const Texture2D image = selectedButton->getTexture();
            if (image.id != 0)
            {
                DrawTexturePro(
                    image,
                    { 0.0f, 0.0f, static_cast<float>(image.width), static_cast<float>(image.height) },
                    imageBounds,
                    { 0.0f, 0.0f },
                    0.0f,
                    WHITE
                );
            }
        }
    }
    DrawRectangleRoundedLinesEx(imageBounds, 0.1f, 8, 2.0f, LIGHTGRAY);

    const float textLeftX = imageBounds.x + imageBounds.width + 14.0f;
    const int titleX = static_cast<int>(textLeftX);
    int textY = static_cast<int>(panelBounds.y + 12.0f);
    const int textAreaWidth = static_cast<int>(panelBounds.width - (textLeftX - panelBounds.x) - 12.0f);

    DrawText(selectedBuilding.getName().c_str(), titleX, textY, BOTTOM_PANEL_TITLE_SIZE, GOLD);
    textY += BOTTOM_PANEL_TITLE_SIZE + 6;

    const std::string metaLine = "Cost: " + std::to_string(selectedBuilding.getCost()) + " Dark Gold   Status: " + (selectedBuilding.isBuilt() ? "Built" : "Not Built");
    DrawText(metaLine.c_str(), titleX, textY, BOTTOM_PANEL_TEXT_SIZE, SKYBLUE);
    textY += BOTTOM_PANEL_TEXT_SIZE + 4;

    std::string requirementsLine = "Requirements: ";
    const std::vector<std::string> &requirements = selectedBuilding.getRequirements();
    if (requirements.empty()) requirementsLine += "None";
    else
    {
        for (int i = 0; i < static_cast<int>(requirements.size()); i++)
        {
            requirementsLine += requirements[i];
            if (i < static_cast<int>(requirements.size()) - 1) requirementsLine += ", ";
        }
    }

    const std::vector<std::string> requirementLines = wrapText(requirementsLine, textAreaWidth, BOTTOM_PANEL_TEXT_SIZE);
    for (const std::string &line : requirementLines)
    {
        DrawText(line.c_str(), titleX, textY, BOTTOM_PANEL_TEXT_SIZE, GREEN);
        textY += BOTTOM_PANEL_TEXT_SIZE + 2;
    }

    textY += 2;
    const std::vector<std::string> descriptionLines = wrapText(selectedBuilding.getDescription(), textAreaWidth, BOTTOM_PANEL_TEXT_SIZE);
    for (const std::string &line : descriptionLines)
    {
        DrawText(line.c_str(), titleX, textY, BOTTOM_PANEL_TEXT_SIZE, WHITE);
        textY += BOTTOM_PANEL_TEXT_SIZE + 2;
    }
}

void Castle::shutdown()
{
    if (mBuildSoundLoaded)
    {
        if (IsSoundPlaying(mBuildSound)) StopSound(mBuildSound);
        UnloadSound(mBuildSound);
        mBuildSoundLoaded = false;
    }

    mBuildingButtons.clear();
    mSelectedBuildingIndex = -1;
    GameScene::shutdown();
}

void Castle::syncHudFromLocalPlayer()
{
    LocalPlayerState *localPlayer = mLocalGameState ? mLocalGameState->getLocalPlayerState() : nullptr;
    if (!localPlayer) return;

    setGold(localPlayer->gold);
    setDarkGold(localPlayer->darkGold);
    setPlayerHealth(localPlayer->health);
}
