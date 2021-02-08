//
// Created by durda on 1/11/21.
//

#include "Shader.h"

void Shader::checkCompilingErrors(unsigned int &shader)
{
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cout << "ERROR: Shader compilation failed\n" << infoLog << std::endl;
    }
}

void Shader::checkLinkingErrors(unsigned int &program)
{
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cout << "ERROR: Shader linking failed\n" << infoLog << std::endl;
    }
}

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    {
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;

        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;

            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();

            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure &e)
        {
            std::cout << "Error: Shader file was not successfully read" << std::endl;
        }

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

//    створюємо об'єкт вершинного шейдера
        unsigned int vertexShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vShaderCode, nullptr);
        glCompileShader(vertexShader);
        checkCompilingErrors(vertexShader);

//    створюємо об'єкт фрагментного шейдера
        unsigned int fragmentShader;
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fShaderCode, nullptr);
        glCompileShader(fragmentShader);
        checkCompilingErrors(fragmentShader);

//    створюємо об'єкт шейдерної програми і зв'язуємо шейдери
        ID = glCreateProgram();
        glAttachShader(ID, vertexShader);
        glAttachShader(ID, fragmentShader);
        glLinkProgram(ID);
        checkLinkingErrors(ID);

//    після зв'язки видаляємо непотрібні шейдери
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

    }
}