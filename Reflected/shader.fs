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
uniform float time;
uniform float grainAmount;

float rand(vec2 co)
{
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 baseColor;

    vec3 n = abs(normalize(Normal));
    vec2 worldUV;

    if(n.y > 0.5)
       worldUV = FragPos.xz;
    else if (n.x > 0.5)
       worldUV = FragPos.zy;
    else
       worldUV = FragPos.xy;

    worldUV /= 8.0;

    if (useTexture == 1)
        baseColor = vec3(texture(texture1,worldUV));
    else
        baseColor = vec3(0.8, 0.8, 0.8);

    vec3 result = (ambient + diffuse) * baseColor;

    float noise = rand(gl_FragCoord.xy + time);
    result += (noise - 0.5) * grainAmount;

    FragColor = vec4(result, 1.0);
}