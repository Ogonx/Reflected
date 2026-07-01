#version 330 core
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
out vec4 FragColor;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform sampler2D texture1;
uniform int useTexture;

void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 baseColor;
    if (useTexture == 1)
        baseColor = vec3(texture(texture1, TexCoord));
    else
        baseColor = vec3(0.8, 0.8, 0.8);
    vec3 result = (ambient + diffuse) * baseColor;
    FragColor = vec4(result, 1.0);
}