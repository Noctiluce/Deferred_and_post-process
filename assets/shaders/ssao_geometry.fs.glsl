#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main()
{    
    if(texture(texture_diffuse1, TexCoords).a <= 0.1) discard;
    gPosition = FragPos;
    gNormal = normalize(Normal);
    gAlbedo.rgb = texture(texture_diffuse1, TexCoords).rgb;

}