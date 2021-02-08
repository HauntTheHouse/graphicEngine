//
// Created by durda on 1/11/21.
//

#include "Engine.h"

#include <cmath>
#include <map>

Camera *(Engine::camera) = new Camera(glm::vec3(0.0f, 20.0f, 40.0f));
InputProcessing *(Engine::inputProcessing) = new InputProcessing({0.0f, 0.0f, true, 0.0f, 0.0f});

Engine::Engine() : Engine(800, 600, "Unnamed") {}
Engine::Engine(int w, int h, std::string t) :
        width(w), height(h), title(std::move(t))
{

    objects.push_back(new Area({MATERIAL, FLAT}));
    objects.push_back(new Sphere(1.0f, {MATERIAL, STENCIL}));
    objects.push_back(new Cube({MATERIAL, STENCIL}));

    objects.push_back(new Area({TEXTURE, TRANSPARENT, FLAT}));
    objects.push_back(new Area({TEXTURE, TRANSPARENT, FLAT}));
    objects.push_back(new Area({TEXTURE, TRANSPARENT, FLAT}));

    lights.push_back(new DirLight);
    lights[0]->setLightPower(0.7f, 0.4f, 0.2f);
    lights[0]->setDirLight(glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    VAO.resize(objects.size());
    VBO.resize(objects.size());
    EBO.resize(objects.size());
}

Engine::~Engine() {
    for (Object* &object : objects)
    {
        delete object;
    }
    for (Light* &light : lights)
    {
        delete light;
    }
}

void Engine::run()
{

    init();
    loadShaders();
    bindVertexAttributes();
//    createFramebuffers();
    loadTextures();
    initializeObjects();
    loop();
}

void Engine::init()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);

    if(glewInit())
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        exit(2);
    }

    glfwSetFramebufferSizeCallback(window, Engine::framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
}

void Engine::loadShaders()
{
    for(Object* & object : objects)
    {
        unsigned int type = object->getObjectType();
        if(type & TEXTURE)
            object->connectShader("../shaders/textureObject.vert", "../shaders/textureObject.frag");
        if(type & STENCIL)
            object->connectOutlineShader("../shaders/stencil.vert", "../shaders/stencil.frag");
        if(type & MATERIAL)
            object->connectShader("../shaders/object.vert", "../shaders/object.frag");

    }
}

void Engine::loadTextures() {
    unsigned int texture;
    glGenTextures(1, &texture);

    texture = loadImage("../images/glass.png");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

}

unsigned int Engine::loadImage(std::string const &path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    stbi_set_flip_vertically_on_load(true);
    int imageWidth, imageHeight, nrComponents;
    unsigned char *data = stbi_load(path.c_str(), &imageWidth, &imageHeight, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, imageWidth, imageHeight, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void Engine::bindVertexAttributes()
{
    glGenVertexArrays(VAO.size(), &VAO[0]);
    glGenBuffers(VBO.size(), &VBO[0]);
    glGenBuffers(EBO.size(), &EBO[0]);

    for(int i = 0; i < objects.size(); ++i)
    {
        objects[i]->setVAO(VAO[i]);
        glBindVertexArray(VAO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
        glBufferData(GL_ARRAY_BUFFER, objects[i]->getVertices().size() * sizeof(float), &objects[i]->getVertices()[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, objects[i]->getIndices().size() * sizeof(unsigned int), &objects[i]->getIndices()[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0 * sizeof(float)));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }

}

//void Engine::createFramebuffers() {
//    unsigned int framebuffer;
//    glGenFramebuffers(1, &framebuffer);
//    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
//
//    unsigned int texColorBuffer;
//    glGenTextures(1, &texColorBuffer);
////    glActiveTexture(GL_TEXTURE1);
//    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glBindTexture(GL_TEXTURE_2D, 0);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);
//
//    unsigned int renderbuffer;
//    glGenRenderbuffers(1, &renderbuffer);
//    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
//    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
//    glBindRenderbuffer(GL_RENDERBUFFER, 0);
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);
//
//    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE);
//        std::cerr << "Error: framebuffer is not complete!" << std::endl;
//
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    glDeleteFramebuffers(1, &framebuffer);
//}


void Engine::initializeObjects()
{
    glm::mat4 areaModel = glm::mat4(1.0f);
    areaModel = glm::scale(areaModel, glm::vec3(50.0f));
    areaModel = glm::rotate(areaModel, glm::radians((float)-90.0), glm::vec3(1.0f, 0.0f, 0.0f));
    objects[0]->setModelMatrix(areaModel);
    objects[0]->setMaterial({glm::vec3(0.45f, 0.65f, 0.45f), glm::vec3(1.0f), 16});

    glm::mat4 sphereModel = glm::mat4(1.0f);
    sphereModel = glm::translate(sphereModel, glm::vec3(4.0f, 4.01f, 0.0f));
    sphereModel = glm::scale(sphereModel, glm::vec3(4.0f));
    objects[1]->setModelMatrix(sphereModel);
    objects[1]->setMaterial({glm::vec3(0.65f, 0.45f, 0.45f), glm::vec3(1.0f), 16});

    glm::mat4 cubeModel = glm::mat4(1.0f);
    cubeModel = glm::translate(cubeModel, glm::vec3(10.0f, 4.01f, 8.5f));
    cubeModel = glm::scale(cubeModel, glm::vec3(3.0f));
    cubeModel = glm::rotate(cubeModel, glm::radians((float)40.0), glm::vec3(0.0f, 1.0f, 0.0f));
    objects[2]->setModelMatrix(cubeModel);
    objects[2]->setMaterial({glm::vec3(0.45f, 0.45f, 0.65f), glm::vec3(1.0f), 16});

    glm::mat4 textureObject1 = glm::mat4(1.0f);
    textureObject1 = glm::scale(textureObject1, glm::vec3(2.5f));
    textureObject1 = glm::translate(textureObject1, glm::vec3(-10.0f, 2.5f/2.0f, 0.0f));
    textureObject1 = glm::rotate(textureObject1, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    objects[3]->setModelMatrix(textureObject1);
    objects[3]->getShader().setInt("objectTexture", 0);

    glm::mat4 textureObject2 = glm::mat4(1.0f);
    textureObject2 = glm::scale(textureObject2, glm::vec3(3.0f));
    textureObject2 = glm::translate(textureObject2, glm::vec3(-5.0f, 3.0f/2.0f, 1.0f));
    textureObject2 = glm::rotate(textureObject2, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    objects[4]->setModelMatrix(textureObject2);
    objects[4]->getShader().setInt("objectTexture", 0);

    glm::mat4 textureObject3 = glm::mat4(1.0f);
    textureObject3 = glm::scale(textureObject3, glm::vec3(2.0f));
    textureObject3 = glm::translate(textureObject3, glm::vec3(-2.0f, 2.0/2.0f, 2.0f));
    textureObject3 = glm::rotate(textureObject3, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    objects[5]->setModelMatrix(textureObject3);
    objects[5]->getShader().setInt("objectTexture", 0);

}


void Engine::loop() {
    while(!glfwWindowShouldClose(window))
    {
        auto currentFrame = glfwGetTime();
        inputProcessing->deltaTime = currentFrame - inputProcessing->lastFrame;
        inputProcessing->lastFrame = currentFrame;
        processCameraInput(window);

        glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendEquation(GL_FUNC_ADD);
//        glFrontFace(GL_CW);
//        glCullFace(GL_FRONT);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        std::map<float, Object*> sortedObjects;
        for (auto & object : objects)
        {
            unsigned int type = object->getObjectType();
            if(type & TRANSPARENT)
            {
                glm::vec3 objectPosition = object->getModelMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                float distance = glm::length(camera->Position - objectPosition);
                sortedObjects[distance] = object;
            }
            else
            {
                if(type & FLAT)
                    glDisable(GL_CULL_FACE);
                object->setVariablesInShader(camera, lights, (float)width / (float)height);
                object->draw();
                glEnable(GL_CULL_FACE);
            }
        }

        for (auto it = sortedObjects.rbegin(); it != sortedObjects.rend(); ++it)
        {
            unsigned int type = it->second->getObjectType();
            if(type & FLAT)
                glDisable(GL_CULL_FACE);
            it->second->setVariablesInShader(camera, lights, (float)width / (float)height);
            it->second->draw();
            glEnable(GL_CULL_FACE);
        }


        processInput(window);

        glfwSwapBuffers(window);
        glfwPollEvents();

    }
}


void Engine::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
void Engine::mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    if(inputProcessing->firstMouse)
    {
        inputProcessing->lastX = static_cast<float>(xpos);
        inputProcessing->lastY = static_cast<float>(ypos);
        inputProcessing->firstMouse = false;
    }
    double xoffset = xpos - inputProcessing->lastX;
    double yoffset = inputProcessing->lastY - ypos;
    inputProcessing->lastX = xpos;
    inputProcessing->lastY = ypos;

    camera->ProcessMouseMovement(xoffset, yoffset);
}

void Engine::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera->ProcessMouseScroll(yoffset);
}

void Engine::processCameraInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            camera->ProcessKeyboard(FORWARD, inputProcessing->deltaTime * 10);
        camera->ProcessKeyboard(FORWARD, inputProcessing->deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            camera->ProcessKeyboard(BACKWARD, inputProcessing->deltaTime * 10);
        camera->ProcessKeyboard(BACKWARD, inputProcessing->deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            camera->ProcessKeyboard(LEFT, inputProcessing->deltaTime * 10);
        camera->ProcessKeyboard(LEFT, inputProcessing->deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            camera->ProcessKeyboard(RIGHT, inputProcessing->deltaTime * 10);
        camera->ProcessKeyboard(RIGHT, inputProcessing->deltaTime);
    }
}

void Engine::processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        for (Object *object : objects)
        {
            unsigned int type = object->getObjectType();
            if(type & STENCIL)
            {
                object->drawOutline(camera, (float)width / (float)height);
            }
        }
    }
}
