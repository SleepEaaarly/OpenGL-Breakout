#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <string>

#include "shader.h"
#include "texture.h"

class ResourceManager
{   
public:
    std::map<std::string, Shader> Shaders;
    std::map<std::string, Texture2D> Textures;
    
    static ResourceManager* GetInstance() { return &instance; }

     // Loads (and generates) a shader program from file loading vertex, fragment (and geometry) shader's source code. If gShaderFile is not nullptr, it also loads a geometry shader
    Shader LoadShader(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile, std::string name);
    // Retrieves a stored shader
    Shader GetShader(std::string name);
    // Loads (and generates) a texture from file
    Texture2D LoadTexture(const GLchar *file, std::string name);
    // Retrieves a stored texture
    Texture2D GetTexture(std::string name);
    // Properly de-allocates all loaded resources
    void Clear();

private:
    static ResourceManager instance;

    ResourceManager() { }
    Shader loadShaderFromFile(const GLchar *vShaderFile, const GLchar *fShaderFile, std::string name, const GLchar *gShaderFile = nullptr);
    Texture2D loadTextureFromFile(const GLchar *file);
};



#endif