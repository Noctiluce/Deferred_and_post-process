#version 460 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;

uniform sampler2D u_albedo_texture;

// material
struct Material
{
    vec3 Ka;              // ambient color
    vec3 Kd;              // diffuse color
    vec3 Ks;              // specular color
    vec3 Ke;              // eminence color
    float Ns;             // specular exponent
    float Ni;             // Optical density
    float d;              // dissolve variable
    int illum;            // illumination variable
    sampler2D map_Ka;     // ambient texture map name
    sampler2D map_Kd;     // diffuse texture map name
    sampler2D Map_Ks;     // specular texture map name
    sampler2D map_Ns;     // specular highlight map
    sampler2D map_d;      // alpha texture map name
    sampler2D map_Bump;   // bump map name
};

struct MapTest // true if map_-- attached
{
    bool map_Ka;          
    bool map_Kd;
    bool map_Ks;
    bool map_Ns;
    bool map_d;
    bool map_Bump;
};

uniform Material u_material;
uniform MapTest u_have;


void main()
{
	if(u_have.map_Kd) { 
        FragColor = texture(u_material.map_Kd, TexCoord);
        if (FragColor.a < 0.5) discard;
	}
	else FragColor = vec4(u_material.Kd, 1.0);
}
