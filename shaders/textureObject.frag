#version 330 core

in vec2 TexCoords;

uniform sampler2D objectTexture;

void main() {
    vec4 texColor = texture(objectTexture, TexCoords);
    if(texColor.a < 0.1)
        discard;
    gl_FragColor = texColor;
}
