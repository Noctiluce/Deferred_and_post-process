#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <string>

struct Material
{
    std::string name;               // name of loaded material
    glm::vec3 Ka;                   // ambient color
    glm::vec3 Kd;                   // diffuse color
    glm::vec3 Ks;                   // specular color
    glm::vec3 Ke;                   // eminence color
    float Ns;                       // specular exponent
    float Ni;                       // Optical density
    float d;                        // dissolve variable
    int illum;                      // illumination variable
    std::string map_Ka;             // ambient texture map name
    std::string map_Kd;             // diffuse texture map name
    std::string map_Ks;             // specular texture map name
    std::string map_Ns;             // specular highlight map
    std::string map_d;              // alpha texture map name
    std::string map_Bump;           // bump map name
    
    // texture ID
    unsigned int map_Ka_id = 0;     // ambient texture map name
    unsigned int map_Kd_id = 0;     // diffuse texture map name
    unsigned int map_Ks_id = 0;     // specular texture map name
    unsigned int map_Ns_id = 0;     // specular highlight map
    unsigned int map_d_id = 0;      // alpha texture map name
    unsigned int map_Bump_id = 0;   // bump map name
    
    // texture boolean
    bool have_map_Ka;
    bool have_map_Kd;
    bool have_map_Ks;
    bool have_map_Ns;
    bool have_map_d;
    bool have_map_Bump;
    
    // default constructor
    Material()
    {
        name = "";   
        Ka = glm::vec3(0.0);      
        Kd = glm::vec3(0.0);      
        Ks = glm::vec3(0.0);      
        Ke = glm::vec3(0.0);      
        Ns = 0.0f;
        Ni = 0.0f;
        d = 0.0f;
        illum = 0;
        have_map_Ka = false;
        have_map_Kd = false;
        have_map_Ks = false;
        have_map_Ns = false;
        have_map_d = false;
        have_map_Bump = false;
        map_Ka_id = 0; 
        map_Kd_id = 0; 
        map_Ks_id = 0; 
        map_Ns_id = 0; 
        map_d_id = 0;  
        map_Bump_id = 0; 
    }
};

#endif
