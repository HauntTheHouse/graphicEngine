#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
//in vec2 TexCoords;

struct Material {
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform DirLight dirLight;
uniform vec3 camPos;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 fragPos, vec3 camPos);



void main()
{
    vec3 color = vec3(0.0f, 0.0f, 0.0f);
    vec3 norm = normalize(Normal);

        color += calcDirLight(dirLight, norm, FragPos, camPos);
//    color += calcPointLight(pointLight, norm, FragPos);
    //    color += calcFlashLight(flashLight, norm, FragPos);

    FragColor = vec4(color, 1.0f);

//    float near = 0.01f;
//    float far = 400.0f;
//
//    float depth = gl_FragCoord.z;
//    float z = 2.0 * depth - 1.0f;
//    depth = (2.0 * near * far) / (far + near - z * (far - near)) / far;
//    FragColor = vec4(vec3(depth), 1.0f);
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 fragPos, vec3 camPos)
{
    vec3 ambient = light.ambient * material.diffuse;

    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = light.diffuse * diff * material.diffuse;

    vec3 viewDir = normalize(camPos-fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * material.specular;

    return (ambient + diffuse + specular);
}