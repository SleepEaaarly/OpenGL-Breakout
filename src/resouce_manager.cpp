#include "resource_manager.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Instantiate static variables
ResourceManager ResourceManager::instance;

Shader ResourceManager::LoadShader(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile, std::string name) {
    Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, name, gShaderFile);
    return Shaders[name];
}

Shader ResourceManager::GetShader(std::string name) {
    return Shaders[name];
}

Texture2D ResourceManager::LoadTexture(const GLchar *file, std::string name) {
    Textures[name] = loadTextureFromFile(file);
    return Textures[name];
}

Texture2D ResourceManager::GetTexture(std::string name) {
    return Textures[name];
}

void ResourceManager::Clear() {
    for (auto iter : Shaders) {
        glDeleteProgram(iter.second.ID);
    }
    for (auto iter : Textures) {
        glDeleteTextures(1, &iter.second.ID);
    }
}

Shader ResourceManager::loadShaderFromFile(const GLchar *vShaderPath, const GLchar *fShaderPath, std::string name, const GLchar *gShaderPath) {
    std::string vShaderCode, fShaderCode, gShaderCode;
    std::ifstream vShaderFile, fShaderFile;

    vShaderFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
    fShaderFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
    // gShaderFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);

    try
    {
        vShaderFile.open(vShaderPath);
        fShaderFile.open(fShaderPath);
        std::stringstream vStringStream, fStringStream;
        vStringStream << vShaderFile.rdbuf();
        fStringStream << fShaderFile.rdbuf();

        vShaderCode = vStringStream.str();
        fShaderCode = fStringStream.str();
        
        if (gShaderPath != nullptr) {
            std::ifstream gShaderFile;
            gShaderFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
            gShaderFile.open(gShaderPath);

            std::stringstream gStringStream;
            gStringStream << gShaderFile.rdbuf();
            gShaderCode = gStringStream.str();
        }

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    Shader shader;
    shader.SetName(name);
    shader.Compile(vShaderCode.c_str(), fShaderCode.c_str(), gShaderPath != nullptr ? gShaderCode.c_str() : nullptr);

    return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const GLchar *file) {
    Texture2D texture;

    int width, height, nrChannels;
    unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
    if (!data) {
        std::cerr << "Failed to load texture: " << stbi_failure_reason() << std::endl;
        exit(-1);
    }
    if (nrChannels == 1) {
        texture.Internal_format = GL_RED;
        texture.Image_format = GL_RED;
    } else if (nrChannels == 3) {
        texture.Internal_format = GL_RGB;
        texture.Image_format = GL_RGB;
    } else if (nrChannels == 4) {
        texture.Internal_format = GL_RGBA;
        texture.Image_format = GL_RGBA;
    } else {
        std::cerr << "Texture channel number error" << std::endl;
        exit(-1);
    }

    // std::cout << nrChannels << std::endl;

    texture.Generate(width, height, data);

    stbi_image_free(data);

    return texture;
}
