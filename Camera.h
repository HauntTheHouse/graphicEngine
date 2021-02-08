//
// Created by durda on 08.12.20.
//

#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Определяет несколько возможных вариантов движения камеры. Используется в качестве абстракции, чтобы держаться подальше от специфичных для оконной системы методов ввода
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Параметры камеры по умолчанию
const double YAW         = -90.0f;
const double PITCH       =  -10.0f;
const double SPEED       =  10.5f;
const double SENSITIVITY =  0.1f;
const double ZOOM        =  45.0f;


// Абстрактный класс камеры, который обрабатывает входные данные и вычисляет соответствующие Эйлеровы углы, векторы и матрицы для использования в OpenGL
class Camera
{
public:
    // Атрибуты камеры
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // углы Эйлера
    double Yaw;
    double Pitch;
    // Настройки камеры
    float MovementSpeed;
    double MouseSensitivity;
    double Zoom;

    // Конструктор, использующий векторы
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), double yaw = YAW, double pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // Конструктор, использующие скаляры
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // Возвращает матрицу вида, вычисленную с использованием углов Эйлера и LookAt-матрицы
    glm::mat4 GetViewMatrix() const
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    //Обрабатываем входные данные, полученные от любой клавиатуроподобной системы ввода. Принимаем входной параметр в виде определенного камерой перечисления (для абстрагирования его от оконных систем)
    void ProcessKeyboard(Camera_Movement direction, double deltaTime)
    {
        float velocity = MovementSpeed * static_cast<float>(deltaTime);
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    //Обрабатываем входные данные, полученные от системы ввода с помощью мыши. Ожидаем в качестве параметров значения смещения как в направлении X, так и в направлении Y.
    void ProcessMouseMovement(double xoffset, double yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // Убеждаемся, что когда тангаж выходит за пределы обзора, экран не переворачивается
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // Обновляем значения вектора-прямо, вектора-вправо и вектора-вверх, используя обновленные значения углов Эйлера
        updateCameraVectors();
    }

    // Обрабатывает входные данные, полученные от события колеса прокрутки мыши. Интересуют только входные данные на вертикальную ось колесика
    void ProcessMouseScroll(double yoffset)
    {
        if (Zoom >= 1.0f && Zoom <= 90.0f)
            Zoom -= yoffset;
        if (Zoom <= 1.0f)
            Zoom = 1.0f;
        if (Zoom >= 90.0f)
            Zoom = 90.0f;
    }

private:
    // Вычисляет вектор-прямо по (обновленным) углам Эйлера камеры
    void updateCameraVectors()
    {
        // Вычисляем новый вектор-прямо
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // Также пересчитываем вектор-вправо и вектор-вверх
        Right = glm::normalize(glm::cross(Front, WorldUp));  // Нормализуем векторы, потому что их длина становится стремится к 0 тем больше, чем больше вы смотрите вверх или вниз, что приводит к более медленному движению.
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};
#endif
