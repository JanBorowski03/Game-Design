# Source and target
SRCS = main.cpp \
       CS3113/cs3113.cpp \
       Scenes/Button.cpp \
       CS3113/Effects.cpp \
       LocalGameState.cpp \
       Units/Unit.cpp \
       Units/UnitPool.cpp \
       CS3113/Entity.cpp \
       Scenes/Map.cpp \
       Scenes/BoardView.cpp \
       Scenes/Scene.cpp \
       Scenes/GameScene.cpp \
       Scenes/MainMenu.cpp \
       Scenes/SkinSelection.cpp \
       Scenes/FactionSelection.cpp \
       Scenes/Shop.cpp \
       Scenes/Castle.cpp \
       Scenes/BattleGrounds.cpp \
       Scenes/WinScene.cpp \
       Scenes/LossScene.cpp \
       CS3113/ShaderProgram.cpp
TARGET = raylib_app

# OS detection
# On native Windows shells, `uname` may be unavailable. Prefer `OS` first.
UNAME_S := $(shell uname -s 2>/dev/null)

# Default values
CXX = g++
CXXFLAGS = -std=c++11

# Raylib configuration using pkg-config
RAYLIB_CFLAGS = $(shell pkg-config --cflags raylib)
RAYLIB_LIBS = $(shell pkg-config --libs raylib)

ifeq ($(OS),Windows_NT)
    # Windows configuration (assumes raylib in C:/raylib)
    CXXFLAGS += -IC:/raylib/include
    LIBS = -LC:/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm
    TARGET := $(TARGET).exe
    EXEC = ./$(TARGET)
else ifeq ($(UNAME_S), Darwin)
    # macOS configuration
    CXXFLAGS += -arch arm64 $(RAYLIB_CFLAGS)
    LIBS = $(RAYLIB_LIBS) -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
    EXEC = ./$(TARGET)
else ifneq (,$(findstring MINGW,$(UNAME_S)))
    # Windows configuration (assumes raylib in C:/raylib)
    CXXFLAGS += -IC:/raylib/include
    LIBS = -LC:/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm
    TARGET := $(TARGET).exe
    EXEC = ./$(TARGET)
else
    # Linux/WSL fallback
    CXXFLAGS +=
    LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    EXEC = ./$(TARGET)
endif

# Build rule
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LIBS)

# Clean rule
clean:
	@if [ -f "$(TARGET)" ]; then rm -f $(TARGET); fi
	@if [ -f "$(TARGET).exe" ]; then rm -f $(TARGET).exe; fi

# Run rule
run: $(TARGET)
	$(EXEC)
