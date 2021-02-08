//
// Created by durda on 1/11/21.
//

#ifndef PHYSICS_ENGINE_H
#define PHYSICS_ENGINE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <cmath>
#include <algorithm>
//#include <exception>
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Shader.h"
#include "Camera.h"
#include "Object.h"
#include "Light.h"

struct InputProcessing
{
    double deltaTime;
    double lastFrame;

    bool firstMouse;
    double lastX;
    double lastY;
};

class Engine {
private:
    int width;
    int height;
    std::string title;

    GLFWwindow *window {};
    static Camera *camera;
    static InputProcessing *inputProcessing;

    std::vector<Object*> objects;
//    std::vector<Object*> transparentObjects;
    std::vector<Light*> lights;

//    Shader stencil;
//    Area *area;
//    Sphere *sphere;

    std::vector<unsigned int> VAO, VBO, EBO;

    void init();
    void loadShaders();
    void loadTextures();
    unsigned int loadImage(const std::string &path);
    void bindVertexAttributes();
    void createFramebuffers();
    void initializeObjects();
    void loop();
//    void drawArea() const;
//    void drawSphere() const;

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void processCameraInput(GLFWwindow* window);
    void processInput(GLFWwindow *window);

public:
    Engine();
    ~Engine();
    Engine(int w, int h, std::string t);
    void run();

};


#endif //PHYSICS_ENGINE_H
