#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

#include <string>
#include <vector>
using namespace std;

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;

    int m_BoneIDs[MAX_BONE_INFLUENCE];
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
    unsigned int id;
    string type;
    aiString path;
};

class Mesh {
    public:
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;
        unsigned int VAO;

        // constructor
        Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures) {
            this->vertices = vertices;
            this->indices = indices;
            this->textures = textures;

            setupMesh();
        }

        void Draw(Shader shader) {
            unsigned int diffuseNr = 1;
            unsigned int specularNr = 1;
            unsigned int normalNr = 1;
            unsigned int heightNr = 1;
            for (int i = 0; i < textures.size(); i++) {
                glActiveTexture(GL_TEXTURE0 + i);   // 绑定纹理对象前激活对应的纹理单元 g_obj[unit][object]
                // 获取纹理编号
                string number;
                string name = textures[i].type;
                if (name == "texture_diffuse") {
                    number = to_string(diffuseNr++);
                } else if (name == "texture_specular") {
                    number = to_string(specularNr++);
                } else if (name == "texture_normal") {
                    number = to_string(normalNr++);
                } else if (name == "texture_height") {
                    number = to_string(heightNr++);
                }

                shader.setInt((name + number).c_str(), i);
                // 将shader中的 sampler 绑定对应的纹理单元(texture_unit)
                glBindTexture(GL_TEXTURE_2D, textures[i].id);   // 绑定纹理对象 GL_TEXTURE_2D
            }

            // draw mesh
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
            
            // always good practice to set everything back to defaults once configured.
            glActiveTexture(GL_TEXTURE0);
        }
    private:
        unsigned int VBO, EBO;

        void setupMesh() {
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);

            // 这是因为结构体内存布局是连续的
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

            // 顶点位置
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
            // 顶点法线
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
            // 顶点纹理坐标
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

            glBindVertexArray(0);
        }
};

#endif