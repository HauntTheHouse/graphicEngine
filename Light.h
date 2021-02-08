//
// Created by durda on 1/12/21.
//

#ifndef PHYSICS_LIGHT_H
#define PHYSICS_LIGHT_H

#include <glm/glm.hpp>


//struct DirLight {
//    glm::vec3 direction;
//
//    glm::vec3 ambient;
//    glm::vec3 diffuse;
//    glm::vec3 specular;
//};
//
//struct PointLight {
//    glm::vec3 position;
//    glm::vec3 color;
//
//    glm::vec3 ambient;
//    glm::vec3 diffuse;
//    glm::vec3 specular;
//
//    float constant;
//    float linear;
//    float quadratic;
//};
//
//struct FlashLight {
//    glm::vec3 position;
//    glm::vec3 direction;
//    glm::vec3 color;
//    float innerCutOff;
//    float outerCutOff;
//
//    glm::vec3 ambient;
//    glm::vec3 diffuse;
//    glm::vec3 specular;
//
//    float constant;
//    float linear;
//    float quadratic;
//};


class Light
{
protected:
    float ambientPow, diffusePow, specularPow;
    glm::vec3 ambient{};
    glm::vec3 diffuse{};
    glm::vec3 specular{};
public:
    virtual ~Light()= default;
    virtual void setLightPower(float ambPow, float diffPow, float specPow) = 0;
    virtual void setDirLight(glm::vec3 dir, glm::vec3 color) = 0;
    virtual glm::vec3 getDirection() = 0;
    virtual glm::vec3 getAmbient();
    virtual glm::vec3 getDiffuse();
    virtual glm::vec3 getSpecular();
};

class DirLight : public Light
{
private:
    glm::vec3 direction{};
public:
    DirLight() = default;
    void setLightPower(float ambPow, float diffPow, float specPow) override;
    void setDirLight(glm::vec3 dir, glm::vec3 color) override;
    glm::vec3 getDirection() override;
};


#endif //PHYSICS_LIGHT_H
