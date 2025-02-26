#ifndef GAME_H
#define GAME_H

#include <GLFW/glfw3.h>
#include <vector>
#include "game_level.h"
#include "power_up.h"

const GLuint SCR_WIDTH = 800;
const GLuint SCR_HEIGHT = 600;

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

class Game
{
public:
    GameState State;
    GLboolean Keys[1024];
    GLuint Width, Height;
    std::vector<GameLevel> Levels;
    GLuint Level;
    std::vector<PowerUp> PowerUps;

    Game(GLuint width, GLuint height);
    ~Game();

    void Init();
    // game loop
    void ProcessInput(GLfloat dt);
    void Update(GLfloat dt);
    void Render();
    void DoCollisions(GLfloat dt);
    void ResetLevel();
    void ResetPlayer();
    void SpawnPowerUps(GameObject &block);
    void UpdatePowerUps(GLfloat dt);
};

#endif
