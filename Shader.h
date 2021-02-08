//
// Created by durda on 17.11.20.
//

#ifndef OPENGL_TEST_HELLOTRIANGLE_SHADER_H
#define OPENGL_TEST_HELLOTRIANGLE_SHADER_H

#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Shader {
private:
    unsigned int ID;

    static void checkCompilingErrors(unsigned int &shader) ;
    static void checkLinkingErrors(unsigned int &program) ;

public:
    Shader(const char* vertexPath, const char* fragmentPath);

    void use() const
    {
        glUseProgram(ID);
    }

    void setBool(const  std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    void setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    void setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    void setMat4(const std::string &name, glm::mat4 value) const
    {
        unsigned int transformLoc = glGetUniformLocation(ID, name.c_str());
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(value));
    }
    void setMat3(const std::string &name, glm::mat3 value) const
    {
        unsigned int transformLoc = glGetUniformLocation(ID, name.c_str());
        glUniformMatrix3fv(transformLoc, 1, GL_FALSE, glm::value_ptr(value));
    }
    void setVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    void setVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
};


#endif //OPENGL_TEST_HELLOTRIANGLE_SHADER_H
