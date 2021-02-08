//
// Created by durda on 1/12/21.
//

#include "Object.h"

//#include <utility>

Object::Object() : Object({MATERIAL}){}

Object::Object(const std::set<ObjectTypes>& types)
{
    model = glm::mat4(1.0f);
    objectType = 0;
    for(auto type : types)
    {
        objectType |= type;
    }
}

void Object::connectShader(const std::string& vertexPath, const std::string& fragmentPath)
{
    shaderObject = new Shader(vertexPath.c_str(), fragmentPath.c_str());
}

void Object::connectOutlineShader(const std::string &vertexPath, const std::string &fragmentPath) {
    shaderForOutline = new Shader(vertexPath.c_str(), fragmentPath.c_str());
}

std::vector<float> &Object::getVertices()
{
    return this->vertices;
}
std::vector<unsigned int> &Object::getIndices()
{
    return this->indices;
}

Shader Object::getShader()
{
    return *shaderObject;
}

Shader Object::getOutlineShader() {
    return *shaderForOutline;
}


void Object::setMaterial(Material m) {
    material = m;
}

void Object::setVariablesInShader(Camera *camera, const std::vector<Light*>& lights, float aspect)
{
    setViewProjectionMatrixTransform(shaderObject, camera, aspect);

    shaderObject->setVec3("dirLight.direction", lights[0]->getDirection());

    shaderObject->setVec3("dirLight.ambient", lights[0]->getAmbient());
    shaderObject->setVec3("dirLight.diffuse", lights[0]->getDiffuse());
    shaderObject->setVec3("dirLight.specular", lights[0]->getSpecular());

    shaderObject->setVec3("material.diffuse", material.diffuse);
    shaderObject->setVec3("material.specular", material.specular);
    shaderObject->setFloat("material.shininess", material.shininess);

    shaderObject->setVec3("camPos", camera->Position);
}

void Object::setViewProjectionMatrixTransform(Shader *shader, Camera *camera, float aspect)
{
    shader->use();

    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(static_cast<float>(camera->Zoom)), aspect, 0.01f, 400.0f);
    shader->setMat4("projection", projection);

    glm::mat4 view = glm::mat4(1.0f);
    view = camera->GetViewMatrix();
    shader->setMat4("view", view);

    shader->setMat4("model", model);
}

void Object::draw() {
    if(objectType & STENCIL)
    {
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
    }
    else
        glStencilMask(0x00);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
}

void Object::drawOutline(Camera *camera, float aspect) {
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);
    setViewProjectionMatrixTransform(shaderForOutline, camera, aspect);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
//    draw();

    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glEnable(GL_DEPTH_TEST);
}


Object::~Object() {
    delete shaderObject;
}

void Object::setModelMatrix(glm::mat4 modelMatrix) {
    this->model = modelMatrix;
}

glm::mat4 Object::getModelMatrix() {
    return model;
}

unsigned int Object::getVAO() const {
    return VAO;
}

void Object::setVAO(unsigned int vao) {
    VAO = vao;
}

const unsigned int &Object::getObjectType() const {
    return objectType;
}


Area::Area(const std::set<ObjectTypes>& types) : Object(types)
{
    Area::fillVertices();
    Area::fillIndices();
}

void Area::fillVertices() {
    vertices = {
            -1.0f, -1.0f, 0.0f,     0.0f, 0.0f,  1.0f,     0.0f,  0.0f,
            -1.0f,  1.0f, 0.0f,     0.0f, 0.0f,  1.0f,     0.0f,  1.0f,
             1.0f,  1.0f, 0.0f,     0.0f, 0.0f,  1.0f,     1.0f,  1.0f,
             1.0f, -1.0f, 0.0f,     0.0f, 0.0f,  1.0f,     1.0f,  0.0f,
    };
}

void Area::fillIndices() {
    indices = {
            0, 2, 1,
            0, 3, 2,
    };
}


Sphere::Sphere(const float &r, const std::set<ObjectTypes> &types, const unsigned int &sectorC, const unsigned int &stackC) : Object(types),
    radius(r), sectorCount(sectorC), stackCount(stackC)
{
    Sphere::fillVertices();
    Sphere::fillIndices();
}

void Sphere::fillVertices()
{
    float x, y, z, xy;
    float nx, ny, nz, lengthInv = 1.0f / this->radius;
    float s, t;

    float sectorStep = 2.0 * M_PI / (float)this->sectorCount;
    float stackStep = M_PI / (float)this->stackCount;
    float sectorAngle, stackAngle;

    for(int i = 0; i <= this->stackCount; ++i)
    {
        stackAngle = M_PI / 2.0 - (float)i * stackStep;
        xy = this->radius * cos(stackAngle);
        z = this->radius * sin(stackAngle);
        for(int j = 0; j <= this->sectorCount; ++j)
        {
            sectorAngle = (float)j * sectorStep;
            x = xy * cos(sectorAngle);
            y = xy * sin(sectorAngle);

            this->vertices.push_back(x);
            this->vertices.push_back(y);
            this->vertices.push_back(z);

//            std::cout << x << ' ' << y << ' ' << z << std::endl;

            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            this->vertices.push_back(nx);
            this->vertices.push_back(ny);
            this->vertices.push_back(nz);

            s = (float)j / sectorCount;
            t = (float)i / stackCount;
            this->vertices.push_back(s);
            this->vertices.push_back(t);
        }
    }
}

void Sphere::fillIndices()
{
    int k1, k2;

    for(int i = 0; i < this->stackCount; ++i)
    {
        k1 = i * (this->sectorCount + 1);
        k2 = k1 + this->sectorCount + 1;

        for (int j = 0; j < this->sectorCount; ++j, ++k1, ++k2) {
            if(i != 0)
            {
                this->indices.push_back(k1);
                this->indices.push_back(k2);
                this->indices.push_back(k1 + 1);
//                std::cout << "k1: " << k1 << ",  k2: " << k2 << ",  k1+1: " << k1+1 << std::endl;

            }
            if(i != this->stackCount - 1)
            {
                this->indices.push_back(k1 + 1);
                this->indices.push_back(k2);
                this->indices.push_back(k2 + 1);
//                std::cout << "k1+1: " << k1+1 << ",  k2: " << k2 << ",  k2+1: " << k2+1 << std::endl;

            }

        }
    }
}


Cube::Cube(const std::set<ObjectTypes> &types) : Object(types) {
    Cube::fillVertices();
    Cube::fillIndices();
}

void Cube::fillVertices() {
    vertices = {
            // Передняя грань
            -1.0f, -1.0f,  1.0f,    0.0f,  0.0f,  1.0f,     0.0f, 0.0f, // нижняя-левая
            -1.0f,  1.0f,  1.0f,    0.0f,  0.0f,  1.0f,     1.0f, 1.0f, // верхняя-правая
             1.0f,  1.0f,  1.0f,    0.0f,  0.0f,  1.0f,     1.0f, 0.0f, // нижняя-правая
             1.0f, -1.0f,  1.0f,    0.0f,  0.0f,  1.0f,     1.0f, 1.0f, // верхняя-правая

            // Грань слева
            -1.0f, -1.0f, -1.0f,   -1.0f,  0.0f,  0.0f,     1.0f, 0.0f, // верхняя-правая
            -1.0f,  1.0f, -1.0f,   -1.0f,  0.0f,  0.0f,     0.0f, 1.0f, // нижняя-левая
            -1.0f,  1.0f,  1.0f,   -1.0f,  0.0f,  0.0f,     1.0f, 1.0f, // верхняя-левая
            -1.0f, -1.0f,  1.0f,   -1.0f,  0.0f,  0.0f,     0.0f, 1.0f, // нижняя-левая

             1.0f, -1.0f, -1.0f,    0.0f,  0.0f, -1.0f,     0.0f, 0.0f, // нижняя-левая
             1.0f,  1.0f, -1.0f,    0.0f,  0.0f, -1.0f,     1.0f, 0.0f, // нижняя-правая
            -1.0f,  1.0f, -1.0f,    0.0f,  0.0f, -1.0f,     1.0f, 1.0f, // верхняя-правая
            -1.0f, -1.0f, -1.0f,    0.0f,  0.0f, -1.0f,     1.0f, 1.0f, // верхняя-правая

             1.0f, -1.0f,  1.0f,    1.0f,  0.0f,  0.0f,     1.0f, 0.0f, // верхняя-левая
             1.0f,  1.0f,  1.0f,    1.0f,  0.0f,  0.0f,     1.0f, 1.0f, // верхняя-правая
             1.0f,  1.0f, -1.0f,    1.0f,  0.0f,  0.0f,     0.0f, 1.0f, // нижняя-правая
             1.0f, -1.0f, -1.0f,    1.0f,  0.0f,  0.0f,     0.0f, 1.0f, // нижняя-правая

            -1.0f,  1.0f,  1.0f,    0.0f,  1.0f,  0.0f,     0.0f, 1.0f, // верхняя-правая
            -1.0f,  1.0f, -1.0f,    0.0f,  1.0f,  0.0f,     1.0f, 0.0f, // нижняя-левая
             1.0f,  1.0f, -1.0f,    0.0f,  1.0f,  0.0f,     1.0f, 1.0f, // верхняя-левая
             1.0f,  1.0f,  1.0f,    0.0f,  1.0f,  0.0f,     1.0f, 0.0f, // нижняя-левая

            -1.0f, -1.0f, -1.0f,    0.0f, -1.0f,  0.0f,     0.0f, 1.0f, // верхняя-левая
            -1.0f, -1.0f,  1.0f,    0.0f, -1.0f,  0.0f,     1.0f, 1.0f, // верхняя-правая
             1.0f, -1.0f,  1.0f,    0.0f, -1.0f,  0.0f,     1.0f, 0.0f, // нижняя-правая
             1.0f, -1.0f, -1.0f,    0.0f, -1.0f,  0.0f,     1.0f, 0.0f, // нижняя-правая
    };
}

void Cube::fillIndices() {
    indices = {
            0,  2,  1,
            0,  3,  2,

            4,  6,  5,
            4,  7,  6,

            8,  10, 9,
            8,  11, 10,

            12, 14, 13,
            12, 15, 14,

            16, 18, 17,
            16, 19, 18,

            20, 22, 21,
            20, 23, 22
    };
}