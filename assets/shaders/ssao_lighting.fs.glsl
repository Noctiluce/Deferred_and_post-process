#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightnessColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D ssao;

uniform bool u_useSSAO;
uniform float u_ambient;

struct Light {
    vec3 Position;
    vec3 Color;
    
    float Linear;
    float Quadratic;
    float seuil;
    float transition;
};
const int NR_LIGHTS = 32;
uniform Light u_lights[NR_LIGHTS];

float greyScale(in vec3 col) 
{
    return dot(col, vec3(0.3, 0.59, 0.11));
    //return dot(col, vec3(0.2126, 0.7152, 0.0722)); //sRGB
}

void main()
{             
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedo, TexCoords).rgb;
    float AmbientOcclusion = texture(ssao, TexCoords).r;
    
   

   
    
    vec3 ambient = vec3(u_ambient * Diffuse);
    if(u_useSSAO) ambient = ambient * AmbientOcclusion;
    vec3 lighting  = ambient; 
    for(int i = 0 ; i < NR_LIGHTS ; i++){
    float distance = length(u_lights[i].Position - FragPos);
        if(u_lights[i].seuil > distance)   {
            vec3 viewDir  = normalize(-FragPos); 
            // diffuse
            vec3 lightDir = normalize(u_lights[i].Position - FragPos);
            vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * u_lights[i].Color;
            // specular
            vec3 halfwayDir = normalize(lightDir + viewDir);  
            float spec = pow(max(dot(Normal, halfwayDir), 0.0), 8.0);
            vec3 specular = u_lights[i].Color * spec;
            // attenuation 

            float attenuation = 1.0 / (1.0 + u_lights[i].Linear * distance + u_lights[i].Quadratic * distance * distance);
            float doux  = 1.0;

            if(distance >= u_lights[i].seuil-u_lights[i].transition ){
                float mix_value = (1.0/u_lights[i].transition)*(u_lights[i].seuil-distance);
                attenuation = mix(attenuation, 0.0, 1.0- mix_value);
                if (attenuation < 0.0) attenuation = 0.0;
            }    

            diffuse *= attenuation;
            specular *= attenuation;
            lighting += diffuse + specular;
            
        }
    }
    FragColor = vec4(lighting, 1.0);
    if(greyScale(lighting) > 0.7)
        BrightnessColor = FragColor;
    else BrightnessColor = vec4(0.0);
    
}