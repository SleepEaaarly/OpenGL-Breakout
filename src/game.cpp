#include "sprite_renderer.h"
#include "game.h"
#include "resource_manager.h"
#include "ball_object.h"
#include "particle_generator.h"
#include "post_process.h"
#include <iostream>
#include <algorithm>

#define EPS 1e-6

const glm::vec2 PLAYER_SIZE(100, 20);
const GLfloat PLAYER_VELOCITY(500.);
const glm::vec2 INITIAL_BALL_VELOCITY(100.f, -350.f);

const GLfloat BALL_RADIUS = 12.5f;
GLfloat shakeTime = 0.f;

SpriteRenderer *Renderer;
GameObject *Player;
BallObject *Ball;
ParticleGenerator *Particles;
PostProcess *Effects;

Game::Game(GLuint width, GLuint height) : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{
    
}

Game::~Game() 
{
    delete Renderer;
    delete Player;
    delete Ball;
    delete Particles;
}

void ActivatePowerUp(PowerUp &powerUp) {
    powerUp.Destroyed = GL_TRUE;
    powerUp.Activated = GL_TRUE;
    if (powerUp.Type == "speed")
    {
        Ball->Velocity *= 1.2;
    }
    else if (powerUp.Type == "sticky")
    {
        Ball->Sticky = GL_TRUE;
        Player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
    }
    else if (powerUp.Type == "pass-through")
    {
        Ball->PassThrough = GL_TRUE;
        Ball->Color = glm::vec3(1.0f, 0.5f, 0.5f);
    }
    else if (powerUp.Type == "pad-size-increase")
    {
        Player->Scale.x += 50;
    }
    else if (powerUp.Type == "confuse")
    {
        if (!Effects->chaos)
            Effects->confuse = GL_TRUE; // 只在chaos未激活时生效，chaos同理
    }
    else if (powerUp.Type == "chaos")
    {
        if (!Effects->confuse)
            Effects->chaos = GL_TRUE;
    }
}

GLboolean isOtherPowerUpActive(std::vector <PowerUp> &powerUps, std::string type) {
    for (const PowerUp &powerUp : powerUps)
    {
        if (powerUp.Activated)
            if (powerUp.Type == type)
                return GL_TRUE;
    }
    return GL_FALSE;
}

void Game::Init() 
{
    Shader spriteShader = ResourceManager::GetInstance()->LoadShader("shaders/sprite_renderer.vs", "shaders/sprite_renderer.fs", nullptr, "sprite");
    Shader particleShader = ResourceManager::GetInstance()->LoadShader("shaders/particle.vs", "shaders/particle.fs", nullptr, "particle");
    Shader postProcessShader = ResourceManager::GetInstance()->LoadShader("shaders/post_process.vs", "shaders/post_process.fs", nullptr, "postprocess");

    // load textures
    ResourceManager::GetInstance()->LoadTexture("textures/background.jpg", "background");
    Texture2D ballTex = ResourceManager::GetInstance()->LoadTexture("textures/awesomeface.png", "face");
    ResourceManager::GetInstance()->LoadTexture("textures/block.png", "block");
    ResourceManager::GetInstance()->LoadTexture("textures/block_solid.png", "block_solid");
    Texture2D paddleTex = ResourceManager::GetInstance()->LoadTexture("textures/paddle.png", "paddle");
    Texture2D particleTex = ResourceManager::GetInstance()->LoadTexture("textures/particle.png", "particle");
    ResourceManager::GetInstance()->LoadTexture("textures/powerup_chaos.png", "powerup_chaos");
    ResourceManager::GetInstance()->LoadTexture("textures/powerup_confuse.png", "powerup_confuse");
    ResourceManager::GetInstance()->LoadTexture("textures/powerup_increase.png", "powerup_increase");
    ResourceManager::GetInstance()->LoadTexture("textures/powerup_passthrough.png", "powerup_passthrough");
    ResourceManager::GetInstance()->LoadTexture("textures/powerup_speed.png", "powerup_speed");
    ResourceManager::GetInstance()->LoadTexture("textures/powerup_sticky.png", "powerup_sticky");
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

    Renderer = new SpriteRenderer(spriteShader);
    glm::vec2 playPos = glm::vec2(
        Width / 2 - PLAYER_SIZE.x / 2,
        Height - PLAYER_SIZE.y
    );
    Player = new GameObject(playPos, PLAYER_SIZE, paddleTex);
    glm::vec2 ballPos = playPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ballTex);
    Particles = new ParticleGenerator(
        particleShader, 
        particleTex,
        500
    );
    Effects = new PostProcess(postProcessShader, Width, Height);
}

void Game::ProcessInput(GLfloat dt) 
{
    if (State == GAME_ACTIVE) {
        GLfloat dist = PLAYER_VELOCITY * dt;
        if (Keys[GLFW_KEY_A]) {
            if (Player->Position.x >= 0) {
                Player->Position.x -= dist;
                if (Ball->Stuck) {
                    Ball->Position.x -= dist;
                }
            }
        }
        if (Keys[GLFW_KEY_D]) {
            if (Player->Position.x <= Width - Player->Scale.x) {
                Player->Position.x += dist;
                if (Ball->Stuck) {
                    Ball->Position.x += dist;
                }
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

    Particles->Update(dt, *Ball);
    if (shakeTime > 0.f) {
        shakeTime -= dt;
    } else {
        Effects->shake = GL_FALSE;
    }

    UpdatePowerUps(dt);
}

void Game::Render() 
{
    if (State == GAME_ACTIVE) {
        Effects->BeginRender();
        Texture2D background = ResourceManager::GetInstance()->GetTexture("background");
        Renderer->DrawSprite(background,
        glm::vec2(0, 0), glm::vec2(Width, Height));
        Levels[Level].Draw(*Renderer);
        Player->Draw(*Renderer);
        // Particle before ball
        Particles->Draw();
        Ball->Draw(*Renderer);
        for (PowerUp &powerUp : this->PowerUps) {
            if (!powerUp.Destroyed) {
                powerUp.Draw(*Renderer);
            }
        }

        Effects->EndRender();
        Effects->Render(glfwGetTime());
    }
}

void Game::DoCollisions(GLfloat dt) {
    for (GameObject &brick : this->Levels[this->Level].Bricks) {        // 注意迭代器必须加引用&才能改变被遍历对象本身
        if (!brick.Destroyed) {
            Collision collision = Ball->GetCollision(brick);
            if (std::get<0>(collision)) {
                if (!brick.IsSolid) {
                    brick.Destroyed = GL_TRUE;
                    SpawnPowerUps(brick);
                } else {
                    shakeTime = 0.05f;
                    Effects->shake = GL_TRUE;
                }
                glm::vec2 nearestPoint = std::get<1>(collision);
                float dist = std::get<2>(collision);
                if (!Ball->PassThrough || brick.IsSolid) {
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
                        Ball->Position.y = brick.Position.y - 2 * Ball->Radius;
                        Ball->Velocity.y = -Ball->Velocity.y;
                        std::cout << "UP" << std::endl;
                    } else if (down) {
                        Ball->Position.y = brick.Position.y + brick.Scale.y;
                        Ball->Velocity.y = -Ball->Velocity.y;
                        std::cout << "DOWN" << std::endl;
                    }
                    if (left) {
                        Ball->Position.x = brick.Position.x - 2 * Ball->Radius;
                        Ball->Velocity.x = -Ball->Velocity.x;
                        std::cout << "LEFT" << std::endl;
                    } else if (right) {
                        Ball->Position.x = brick.Position.x + brick.Scale.x;
                        Ball->Velocity.x = -Ball->Velocity.x;
                        std::cout << "RIGHT" << std::endl;
                    }
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
        Ball->Stuck = Ball->Sticky;
    }

    for (PowerUp &powerUp : this->PowerUps)
    {
        if (!powerUp.Destroyed)
        {
            if (powerUp.Position.y >= this->Height)
                powerUp.Destroyed = GL_TRUE;
            if (powerUp.CheckCollision(*Player))
            {   // 道具与挡板接触，激活它！
                ActivatePowerUp(powerUp);
            }
        }
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

GLboolean ShouldSpawn(GLuint chance) {
    GLuint random = rand() % chance;
    return random == 0;
}

const int pos_chance = 75;
const int neg_chance = 25;

void Game::SpawnPowerUps(GameObject &block) {
    if (ShouldSpawn(pos_chance)) {
        Texture2D tex_speed = ResourceManager::GetInstance()->GetTexture("powerup_speed");
        this->PowerUps.push_back(
            PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.Position, tex_speed
        ));
    }
    if (ShouldSpawn(pos_chance)) {
        Texture2D tex_sticky = ResourceManager::GetInstance()->GetTexture("powerup_sticky");
        this->PowerUps.push_back(PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.Position, tex_sticky));
    }
    if (ShouldSpawn(pos_chance)) {
        Texture2D tex_passthrough = ResourceManager::GetInstance()->GetTexture("powerup_passthrough");
        this->PowerUps.push_back(PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.Position, tex_passthrough));
    }
    if (ShouldSpawn(pos_chance)) {
        Texture2D tex_increase = ResourceManager::GetInstance()->GetTexture("powerup_increase");
        this->PowerUps.push_back(PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f, block.Position, tex_increase));
    }
    // Negative powerups should spawn more often
    if (ShouldSpawn(neg_chance)) {
        Texture2D tex_confuse = ResourceManager::GetInstance()->GetTexture("powerup_increase");
        this->PowerUps.push_back(PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.Position, tex_confuse));
    }
    if (ShouldSpawn(neg_chance)) {
        Texture2D tex_chaos = ResourceManager::GetInstance()->GetTexture("powerup_chaos");
        this->PowerUps.push_back(PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.Position, tex_chaos));
    }
}



void Game::UpdatePowerUps(GLfloat dt) {
    for (PowerUp &powerUp : this->PowerUps)
    {
        powerUp.Position += powerUp.Velocity * dt;
        if (powerUp.Position.y > Height) 
            powerUp.Destroyed = true;
        if (powerUp.Activated)
        {
            powerUp.Duration -= dt;

            if (powerUp.Duration <= 0.0f)
            {
                // 之后会将这个道具移除
                powerUp.Activated = GL_FALSE;
                // 停用效果
                if (powerUp.Type == "sticky")
                {
                    if (!isOtherPowerUpActive(this->PowerUps, "sticky"))
                    {   // 仅当没有其他sticky效果处于激活状态时重置，以下同理
                        Ball->Sticky = GL_FALSE;
                        Player->Color = glm::vec3(1.0f);
                    }
                }
                else if (powerUp.Type == "pass-through")
                {
                    if (!isOtherPowerUpActive(this->PowerUps, "pass-through"))
                    {
                        Ball->PassThrough = GL_FALSE;
                        Ball->Color = glm::vec3(1.0f);
                    }
                }
                else if (powerUp.Type == "confuse")
                {
                    if (!isOtherPowerUpActive(this->PowerUps, "confuse"))
                    {
                        Effects->confuse = GL_FALSE;
                    }
                }
                else if (powerUp.Type == "chaos")
                {
                    if (!isOtherPowerUpActive(this->PowerUps, "chaos"))
                    {
                        Effects->chaos = GL_FALSE;
                    }
                }                
            }
        }
    }
    this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
        [](const PowerUp &powerUp) { return powerUp.Destroyed && !powerUp.Activated; }
    ), this->PowerUps.end());
}
