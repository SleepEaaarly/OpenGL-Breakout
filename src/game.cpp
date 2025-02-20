#include "sprite_renderer.h"
#include "game.h"
#include "resource_manager.h"
#include "ball_object.h"
#include <iostream>

#define EPS 1e-6

const glm::vec2 PLAYER_SIZE(100, 20);
const GLfloat PLAYER_VELOCITY(500.);
const glm::vec2 INITIAL_BALL_VELOCITY(100.f, -350.f);

const GLfloat BALL_RADIUS = 12.5f;

SpriteRenderer *Renderer;
GameObject *Player;
BallObject *Ball;

Game::Game(GLuint width, GLuint height) : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{
    
}

Game::~Game() 
{
    delete Renderer;
    delete Player;
    delete Ball;
}

void Game::Init() 
{
    ResourceManager::GetInstance()->LoadShader("shaders/sprite_renderer.vs", "shaders/sprite_renderer.fs", nullptr, "sprite");
    
    // load textures
    ResourceManager::GetInstance()->LoadTexture("textures/background.jpg", "background");
    Texture2D ballTex = ResourceManager::GetInstance()->LoadTexture("textures/awesomeface.png", "face");
    ResourceManager::GetInstance()->LoadTexture("textures/block.png", "block");
    ResourceManager::GetInstance()->LoadTexture("textures/block_solid.png", "block_solid");
    Texture2D paddleTex = ResourceManager::GetInstance()->LoadTexture("textures/paddle.png", "paddle");
    // load levels
    GameLevel one;   one.Load("levels/one.lvl", Width, 0.5 * Height);
    GameLevel two;   two.Load("levels/two.lvl", Width, 0.5 * Height);
    GameLevel three; three.Load("levels/three.lvl", Width, 0.5 * Height);
    GameLevel four;  four.Load("levels/four.lvl", Width, 0.5 * Height);
    
    Levels.push_back(one);
    Levels.push_back(two);
    Levels.push_back(three);
    Levels.push_back(four);
    Level = 0;      // 第一关对应下标0

    Shader shader = ResourceManager::GetInstance()->GetShader("sprite");
    Renderer = new SpriteRenderer(shader);
    glm::vec2 playPos = glm::vec2(
        Width / 2 - PLAYER_SIZE.x / 2,
        Height - PLAYER_SIZE.y
    );
    Player = new GameObject(playPos, PLAYER_SIZE, paddleTex);
    glm::vec2 ballPos = playPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ballTex);
}

void Game::ProcessInput(GLfloat dt) 
{
    if (State == GAME_ACTIVE) {
        GLfloat dist = PLAYER_VELOCITY * dt;
        if (Keys[GLFW_KEY_A] && !Ball->Stuck) {
            if (Player->Position.x >= 0) {
                Player->Position.x -= dist;
            }
        }
        if (Keys[GLFW_KEY_D] && !Ball->Stuck) {
            if (Player->Position.x <= Width - Player->Scale.x) {
                Player->Position.x += dist;
            }
        }
        if (Keys[GLFW_KEY_SPACE]) {
            Ball->Stuck = false;
        }
    }
}

void Game::Update(GLfloat dt) 
{
    Ball->Move(dt, Width);

    DoCollisions(dt);

    if (Ball->Position.y >= Height) {
        ResetLevel();
        ResetPlayer();
    }
}

void Game::Render() 
{
    if (State == GAME_ACTIVE) {
        Texture2D background = ResourceManager::GetInstance()->GetTexture("background");
        Renderer->DrawSprite(background,
        glm::vec2(0, 0), glm::vec2(Width, Height));
        Levels[Level].Draw(*Renderer);
        Player->Draw(*Renderer);
        Ball->Draw(*Renderer);
    }
}

void Game::DoCollisions(GLfloat dt) {
    for (GameObject &brick : this->Levels[this->Level].Bricks) {        // 注意迭代器必须加引用&才能改变被遍历对象本身
        if (!brick.Destroyed) {
            Collision collision = Ball->GetCollision(brick);
            if (std::get<0>(collision)) {
                if (!brick.IsSolid) {
                    brick.Destroyed = GL_TRUE;
                }
                glm::vec2 nearestPoint = std::get<1>(collision);
                float dist = std::get<2>(collision);
                bool up, down, left, right;   // 四个方向指brick受击的方位
                if (dist > EPS) {
                    up = std::abs(nearestPoint.y - brick.Position.y) < EPS; 
                    down = std::abs(nearestPoint.y - brick.Position.y - brick.Scale.y) < EPS;
                    left = std::abs(nearestPoint.x - brick.Position.x) < EPS;
                    right = std::abs(nearestPoint.x - brick.Position.x - brick.Scale.x) < EPS;
                } else {
                    up = (nearestPoint.y - brick.Position.y) > EPS && (nearestPoint.y - brick.Position.y) < Ball->Velocity.y * dt - Ball->Radius;
                    down = (brick.Position.y + brick.Scale.y - nearestPoint.y) > EPS && (brick.Position.y + brick.Scale.y - nearestPoint.y) < Ball->Velocity.y * dt - Ball->Radius;
                    left = (nearestPoint.x - brick.Position.x) > EPS && (nearestPoint.x - brick.Position.x) < Ball->Velocity.x * dt - Ball->Radius;
                    right = (brick.Position.x + brick.Scale.x - nearestPoint.x) > EPS && (brick.Position.x + brick.Scale.x - nearestPoint.x) < Ball->Velocity.x * dt - Ball->Radius;
                }

                if (up) {
                    Ball->Position.y = brick.Position.y - Ball->Radius;
                    Ball->Velocity.y = -Ball->Velocity.y;
                    // std::cout << "UP" << std::endl;
                } else if (down) {
                    Ball->Position.y = brick.Position.y + brick.Scale.y + Ball->Radius;
                    Ball->Velocity.y = -Ball->Velocity.y;
                    // std::cout << "DOWN" << std::endl;
                }
                if (left) {
                    Ball->Position.x = brick.Position.x - Ball->Radius;
                    Ball->Velocity.x = -Ball->Velocity.x;
                    // std::cout << "LEFT" << std::endl;
                } else if (right) {
                    Ball->Position.x = brick.Position.x + brick.Scale.x + Ball->Radius;
                    Ball->Velocity.x = -Ball->Velocity.x;
                    // std::cout << "RIGHT" << std::endl;
                }
                
            }
        }
    }

    Collision result = Ball->GetCollision(*Player);
    if (!Ball->Stuck && std::get<0>(result)) {
        float dist = std::get<1>(result).x - Player->Position.x - Player->Scale.x / 2;
        float percentage = dist / (Player->Scale.x / 2);

        float strength = 2.f;
        glm::vec2 oldVelocity = Ball->Velocity;
        Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
        Ball->Velocity.y = -std::abs(Ball->Velocity.y);
        Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
    }
}

void Game::ResetLevel() {
    if(Level == 0) {
        Levels[Level].Load("levels/one.lvl", Width, 0.5 * Height);
    } else if (Level == 1) {
        Levels[Level].Load("levels/two.lvl", Width, 0.5 * Height);
    } else if (Level == 2) {
        Levels[Level].Load("levels/three.lvl", Width, 0.5 * Height);
    } else if (Level == 3) {
        Levels[Level].Load("levels/four.lvl", Width, 0.5 * Height);
    }
}

void Game::ResetPlayer() {
    glm::vec2 playPos = glm::vec2(
        Width / 2 - PLAYER_SIZE.x / 2,
        Height - PLAYER_SIZE.y
    );
    glm::vec2 ballPos = playPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
    
    Player->PlayerReset(playPos, PLAYER_SIZE);
    Ball->BallReset(ballPos, INITIAL_BALL_VELOCITY, BALL_RADIUS);
}
