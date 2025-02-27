#include "game_level.h"
#include "game_object.h"
#include "resource_manager.h"

void GameLevel::Load(const char *file, GLuint levelWidth, GLuint levelHeight) {
    // clear overdue data
    Bricks.clear();
    // load from file
    GLuint tileCode;
    GameLevel level;
    std::string line;
    std::ifstream fstream(file);
    std::vector< std::vector<GLuint> > tileData;

    if (fstream) 
    {
        while (std::getline(fstream, line))
        {
            std::istringstream sstream(line);
            std::vector<GLuint> row;
            while (sstream >> tileCode) 
                row.push_back(tileCode);
            tileData.push_back(row);
        }
        if (tileData.size() > 0)
            init(tileData, levelWidth, levelHeight);
    }

}

void GameLevel::init(std::vector<std::vector<GLuint> > tileData, GLuint levelWidth, GLuint levelHeight) {
    GLuint height = tileData.size();
    GLuint width = tileData[0].size();

    GLfloat unit_width = levelWidth / width;
    GLfloat unit_height = levelHeight / height;

    for (GLuint y = 0; y < height; y++) {
        for (GLuint x = 0; x < width; x++) {
            if (tileData[y][x] == 1) {
                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                GameObject obj(pos, size, 
                    ResourceManager::GetInstance()->GetTexture("block_solid"), 
                    glm::vec3(0.8f, 0.8f, 0.7f)
                );
                obj.IsSolid = GL_TRUE;
                this->Bricks.push_back(obj);  
            } else {
                glm::vec3 color;
                if (tileData[y][x] == 2)
                    color = glm::vec3(0.2f, 0.6f, 1.0f);
                else if (tileData[y][x] == 3)
                    color = glm::vec3(0.0f, 0.7f, 0.0f);
                else if (tileData[y][x] == 4)
                    color = glm::vec3(0.8f, 0.8f, 0.4f);
                else if (tileData[y][x] == 5)
                    color = glm::vec3(1.0f, 0.5f, 0.0f);
                
                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                Bricks.push_back(
                    GameObject(pos, size, ResourceManager::GetInstance()->GetTexture("block"), color)
                );
            }
        }
    }
}

void GameLevel::Draw(SpriteRenderer &renderer) {
    for (GameObject gobj : Bricks) {
        if (!gobj.Destroyed)
            gobj.Draw(renderer);
    }
}

GLboolean GameLevel::IsCompleted() {
    for (GameObject gobj : Bricks) 
        if (!gobj.Destroyed && !gobj.IsSolid)
            return GL_FALSE;
    return GL_TRUE;
}

/*
    // load level from file
    void Load(const char *file, GLuint levelWidth, GLuint levelHeight);
    // render levels
    void Draw(SpriteRenderer &renderer);
    // check if the level completed
    GLboolean IsCompleted();
*/