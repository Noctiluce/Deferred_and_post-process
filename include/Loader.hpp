#ifndef LOADER_H
#define LOADER_H

#include "structures/Mesh.hpp"
#include "structures/Material.hpp"


class Loader 
{
public:
    std::vector<Mesh> loadedMeshes;
    std::vector<Material> loadedMaterials;
    
    Loader(){}
    virtual ~Loader()= default;
    virtual bool loadFromFile(std::string path) = 0;

    
};


#endif
