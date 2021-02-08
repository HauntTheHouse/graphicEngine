//
// Created by durda on 1/12/21.
//

#ifndef PHYSICS_OBJECT_H
#define PHYSICS_OBJECT_H

#include <utility>
#include <vector>
#include <set>
#include <string>
#include <glm/glm.hpp>
#include "Camera.h"
#include "Shader.h"
#include "Light.h"

struct Material {
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

enum ObjectTypes
{
    MATERIAL =      0x00000001,
    TEXTURE =       0x00000010,
    TRANSPARENT =   0x00000100,
    FLAT =          0x00001000,
    STENCIL =       0x00010000
};

class Object {
protected:
    Material material;
//    std::vector<ObjectTypes> objectTypes;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    unsigned int VAO;
    glm::mat4 model;

    unsigned int objectType;

    Shader *shaderObject{};
    Shader *shaderForOutline{};

    void setViewProjectionMatrixTransform(Shader *shader, Camera *camera, float aspect);
    virtual void fillVertices() = 0;
    virtual void fillIndices() = 0;
public:
    Object();
    explicit Object(const std::set<ObjectTypes>& types = {MATERIAL});
    virtual ~Object();

    unsigned int getVAO() const;
    void setVAO(unsigned int vao);

    void setModelMatrix(glm::mat4 modelMatrix);
    glm::mat4 getModelMatrix();

    void connectShader(const std::string &vertexPath, const std::string &fragmentPath);
    void connectOutlineShader(const std::string &vertexPath, const std::string &fragmentPath);
    Shader getShader();
    Shader getOutlineShader();

    void setMaterial(Material material);
    virtual void setVariablesInShader(Camera *camera, const std::vector<Light*>& lights, float aspect);
    std::vector<float> &getVertices();
    std::vector<unsigned int> &getIndices();

    const unsigned int &getObjectType() const;
    virtual void draw();
    virtual void drawOutline(Camera *camera, float aspect);

};

class Area : public Object {
private:
    void fillVertices() override;
    void fillIndices() override;

public:
    explicit Area(const std::set<ObjectTypes> &types = {MATERIAL});
};

class Sphere : public Object {
private:
    float radius{};
    unsigned int sectorCount{};
    unsigned int stackCount{};

    void fillVertices() override;
    void fillIndices() override;

public:
    explicit Sphere(const float &radius, const std::set<ObjectTypes> &types = {MATERIAL}, const unsigned int &sectorCount = 70, const unsigned int &stackCount = 70);
};

class Cube : public Object {
private:
    void fillVertices() override;
    void fillIndices() override;

public:
    explicit Cube(const std::set<ObjectTypes> &types = {MATERIAL});

};

#endif //PHYSICS_OBJECT_H