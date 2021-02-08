//
// Created by durda on 1/12/21.
//

#include "Light.h"

void DirLight::setLightPower(float ambPow, float diffPow, float specPow) {
    ambientPow = ambPow;
    diffusePow = diffPow;
    specularPow = specPow;
}

void DirLight::setDirLight(glm::vec3 dir, glm::vec3 color) {
    direction = dir;
    ambient = ambientPow * color;
    diffuse = diffusePow * color;
    specular = specularPow * color;
}

glm::vec3 DirLight::getDirection() {
    return direction;
}

glm::vec3 Light::getAmbient() {
    return ambient;
}

glm::vec3 Light::getDiffuse() {
    return diffuse;
}

glm::vec3 Light::getSpecular() {
    return specular;
}

