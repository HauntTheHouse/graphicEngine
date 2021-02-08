#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
//    gl_Position = projection * view * model * vec4(aPos + aNormal*0.05, 1.0);
    gl_Position = projection * view * model * vec4(aPos*1.05, 1.0);
}
