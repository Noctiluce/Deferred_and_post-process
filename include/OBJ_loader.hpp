#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include "Loader.hpp"
#include <iostream>



class OBJ_loader : public Loader
{
public:
    
    OBJ_loader(){}
    ~OBJ_loader(){}
    
    bool loadFromFile(std::string path) override
    {
        if (path.substr(path.size() - 4, 4) != ".obj") {std::cout << "OBJ_loader:: " << path << " is not OBJ format" << std::endl; return false;}
        
        std::ifstream file(path);
        if (!file.is_open()){std::cout << "OBJ_loader:: Can't read file : " << path << std::endl; return false;}
        
        loadedMeshes.clear();
        loadedMaterials.clear();
        
        loadedPositions.clear();
        loadedNormals.clear();
        loadedUVs.clear();
        
        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> normals;
        
        
        std::vector<std::string> meshMaterialNames;
        
        bool listening = false;
        std::string meshname, currentLine;
        
        Mesh tmpMesh;
        
        while (std::getline(file, currentLine))
        {
            if (firstToken(currentLine) == "o" || firstToken(currentLine) == "g" || currentLine[0] == 'g')
            {
                if(!listening)
                {
                    listening = true;
                    
                    if (firstToken(currentLine) == "o" || firstToken(currentLine) == "g")
                        meshname = tail(currentLine);
                    else meshname = "unnamed";
                }
                else 
                {
                    if (!loadedPositions.empty())
                    {
                        tmpMesh = Mesh();
                        tmpMesh.positions = loadedPositions;
                        tmpMesh.normals = loadedNormals;
                        tmpMesh.uvs = loadedUVs;
                        tmpMesh.name = meshname;
                        
                        loadedMeshes.push_back(tmpMesh);
                        
                        loadedPositions.clear();
                        loadedNormals.clear();
                        loadedUVs.clear();
                        meshname.clear();
                        
                        meshname = tail(currentLine);
                    }
                    else 
                    {
                        if (firstToken(currentLine) == "o" || firstToken(currentLine) == "g") 
                            meshname = tail(currentLine);
                        else meshname = "unnamed";
                    }
                }
                
            }
            
            
            // vertex position
            if (firstToken(currentLine) == "v")
            {
                std::vector<std::string> spos;
                split(tail(currentLine), spos, " ");
                
                glm::vec3 vpos = glm::vec3(std::stof(spos[0]), std::stof(spos[1]), std::stof(spos[2]));
                positions.push_back(vpos);
            }
            
            
            // vertex texture coordinate
            if (firstToken(currentLine) == "vt")
            {
                std::vector<std::string> stex;
                split(tail(currentLine), stex, " ");
                
                glm::vec2 vtex = glm::vec2(std::stof(stex[0]), std::stof(stex[1]));
                uvs.push_back(vtex);
            }
            
            // vertex normal
            if (firstToken(currentLine) == "vn")
            {
                std::vector<std::string> snor;
                split(tail(currentLine), snor, " ");
                
                glm::vec3 vnor = glm::vec3(std::stof(snor[0]), std::stof(snor[1]), std::stof(snor[2]));
                normals.push_back(vnor);
            }
            
            // face
            if (firstToken(currentLine) == "f")
            {
               // Generate the vertices
                generateVerticesFromRawOBJ(positions, uvs, normals, currentLine);
            }
            
            // mesh material name
            if (firstToken(currentLine) == "usemtl")
            {
                meshMaterialNames.push_back(tail(currentLine));
                
                // create new mesh if material changes within a group
                if (!loadedPositions.empty())
                {
                    tmpMesh = Mesh();
                    tmpMesh.positions = loadedPositions;
                    tmpMesh.normals = loadedNormals;
                    tmpMesh.uvs = loadedUVs;
                    tmpMesh.name = meshname;
                    
                    /*int i = 2;
                    while(1) {
                        tmpMesh.name = meshname + "_" + std::to_string(i);

                        for (auto &m : loadedMeshes)
                            if (m.name == tmpMesh.name)
                                continue;
                        break;
                    }*/
                    int i = 2;
                    for (auto &m: loadedMeshes)
                    {
                        if (m.name == tmpMesh.name) 
                        {
                            tmpMesh.name = meshname + "_" + std::to_string(i);
                            break;
                        }
                    }
                    
                    loadedMeshes.push_back(tmpMesh);
                    
                    loadedPositions.clear();
                    loadedNormals.clear();
                    loadedUVs.clear();
                }
            }
            
            // load material
            if (firstToken(currentLine) == "mtllib")
            {
                std::vector <std::string> tmp;
                split(path, tmp, "/");
                
                std::string pathtomat = "";
                
                if (tmp.size() != 1)
                {
                    for(int i = 0; i < (int)tmp.size() -1; ++i) pathtomat += tmp[i]+"/";
                }
                
                pathtomat += tail(currentLine);
                
                loadMaterials(pathtomat);
            }
        }
            
        // last mesh
        if (!loadedPositions.empty())
        {
            tmpMesh = Mesh();
            tmpMesh.positions = loadedPositions;
            tmpMesh.normals = loadedNormals;
            tmpMesh.uvs = loadedUVs;
            tmpMesh.name = meshname;
            
            loadedMeshes.push_back(tmpMesh);
        }
        
        file.close();
        
        // set material for each mesh
        for (int i = 0 ; i < (int)meshMaterialNames.size() ; ++i)
        {
            std::string matname = meshMaterialNames[i];
            
            // Find corresponding material name in loaded materials
            // when found copy material variables into mesh material
            for (int j = 0; j < (int)loadedMaterials.size(); j++)
            {
                if (loadedMaterials[j].name == matname)
                {
                    loadedMeshes[i].material = loadedMaterials[j];
                    break;
                }
            }
        }
        
        
        if (loadedMeshes.empty()) return false;
        else return true;
    }
    
    
private :  
    std::vector<glm::vec3> loadedPositions;
    std::vector<glm::vec3> loadedNormals;
    std::vector<glm::vec2> loadedUVs;
    
    // load materials from .mtl file
    bool loadMaterials(std::string path)
    {
        if (path.substr(path.size() - 4, 4) != ".mtl") 
        {std::cout << "OBJ_loader:: Have not mtl file" << std::endl; return false;}
        
        std::ifstream file(path);
        if (!file.is_open())
        {std::cout << "OBJ_loader:: Can't read file : " << path << std::endl; return false;}
        
        Material tmpMat;
        bool listening = false;
        std::string currentLine;
        
        while (std::getline(file, currentLine))
        {
            // new material and material name
            if (firstToken(currentLine) == "newmtl")
            {
                if (!listening)
                {
                    listening = true;

                    if (currentLine.size() > 7) tmpMat.name = tail(currentLine);
                    else tmpMat.name = "none";
                }
                else
                {
                    // Push Back loaded Material
                    loadedMaterials.push_back(tmpMat);
                    // Clear Loaded Material
                    tmpMat = Material();

                    if (currentLine.size() > 7) tmpMat.name = tail(currentLine);
                    else tmpMat.name = "none";
                }
            }
            
            // ambient color
            if (firstToken(currentLine) == "Ka")
            {
                std::vector<std::string> tmp;
                split(tail(currentLine), tmp, " ");
                
                if (tmp.size() != 3) continue;
                
                tmpMat.Ka = glm::vec3(std::stof(tmp[0]), std::stof(tmp[1]), std::stof(tmp[2]));
            }
            
            // diffuse color
            if (firstToken(currentLine) == "Kd")
            {
                std::vector<std::string> tmp;
                split(tail(currentLine), tmp, " ");
                
                if (tmp.size() != 3) continue;
                
                tmpMat.Kd = glm::vec3(std::stof(tmp[0]), std::stof(tmp[1]), std::stof(tmp[2]));
            }
            
            // specular color
            if (firstToken(currentLine) == "Ks")
            {
                std::vector<std::string> tmp;
                split(tail(currentLine), tmp, " ");
                
                if (tmp.size() != 3) continue;
                
                tmpMat.Ks = glm::vec3(std::stof(tmp[0]), std::stof(tmp[1]), std::stof(tmp[2]));
            }
            
            // eminence color
            if (firstToken(currentLine) == "Ke")
            {
                std::vector<std::string> tmp;
                split(tail(currentLine), tmp, " ");
                
                if (tmp.size() != 3) continue;
                
                tmpMat.Ke = glm::vec3(std::stof(tmp[0]), std::stof(tmp[1]), std::stof(tmp[2]));
            }
            
            // specular exponent
            if (firstToken(currentLine) == "Ns")
                tmpMat.Ns = std::stof(tail(currentLine));
            
            // optical density
            if (firstToken(currentLine) == "Ni")
                tmpMat.Ni = std::stof(tail(currentLine));
            
            // dissolve
            if (firstToken(currentLine) == "d")
                tmpMat.d = std::stof(tail(currentLine));
            
            // illumination
            if (firstToken(currentLine) == "illum")
                tmpMat.illum = std::stof(tail(currentLine));
            
            // ambient texture map
            if (firstToken(currentLine) == "map_Ka"){
                tmpMat.map_Ka = tail(currentLine);
                tmpMat.have_map_Ka = true;}
            
            // diffuse texture map
            if (firstToken(currentLine) == "map_Kd"){
                tmpMat.map_Kd = tail(currentLine);
                tmpMat.have_map_Kd = true;}
            
            // specular texture map
            if (firstToken(currentLine) == "map_Ks"){
                tmpMat.map_Ks = tail(currentLine);
                tmpMat.have_map_Ks = true;}
            
            // specular highlight map
            if (firstToken(currentLine) == "map_Ns"){
                tmpMat.map_Ns = tail(currentLine);
                tmpMat.have_map_Ns = true;}
            
            // alpha texture map
            if (firstToken(currentLine) == "map_d"){
                tmpMat.map_d = tail(currentLine);
                tmpMat.have_map_d = true;}
            
            // bump map
            if (firstToken(currentLine) == "map_Bump" 
                || firstToken(currentLine) == "map_bump" 
                || firstToken(currentLine) == "bump" 
                || firstToken(currentLine) == "Bump" 
            ){
                tmpMat.map_Bump = tail(currentLine);
                tmpMat.have_map_Bump = true;}
        }
        
        loadedMaterials.push_back(tmpMat);
        
        if(loadedMaterials.empty()) return false;
        else return true;
    }
    
    
    // Generate vertices from a list of positions, 
    //	tcoords, normals and a face line
    void generateVerticesFromRawOBJ(
        const std::vector<glm::vec3>& iPositions,
        const std::vector<glm::vec2>& iTCoords,
        const std::vector<glm::vec3>& iNormals,
        std::string icurline)
    {
        std::vector<std::string> sface, svert;
        split(tail(icurline), sface, " ");

        bool noNormal = false;

        // For every given vertex do this
        for (int i = 0; i < int(sface.size()); i++)
        {
            // See What type the vertex is.
            int vtype;

            split(sface[i], svert, "/");

            // Check for just position - v1
            if (svert.size() == 1)
            {
                // Only position
                vtype = 1;
            }

            // Check for position & texture - v1/vt1
            if (svert.size() == 2)
            {
                // Position & Texture
                vtype = 2;
            }

            // Check for Position, Texture and Normal - v1/vt1/vn1
            // or if Position and Normal - v1//vn1
            if (svert.size() == 3)
            {
                if (svert[1] != "")
                {
                    // Position, Texture, and Normal
                    vtype = 4;
                }
                else
                {
                    // Position & Normal
                    vtype = 3;
                }
            }

            // Calculate and store the vertex
            switch (vtype)
            {
            case 1: // P
            {
                noNormal = true;
                loadedPositions.push_back(getElement(iPositions, svert[0]));
                loadedUVs.push_back(glm::vec2(0, 0));
                
                break;
            }
            case 2: // P/T
            {
                noNormal = true;
                loadedPositions.push_back(getElement(iPositions, svert[0]));
                loadedUVs.push_back(getElement(iTCoords, svert[1]));
                break;
            }
            case 3: // P//N
            {
                loadedPositions.push_back(getElement(iPositions, svert[0]));
                loadedUVs.push_back(glm::vec2(0, 0));
                loadedNormals.push_back(getElement(iNormals, svert[2]));
                break;
            }
            case 4: // P/T/N
            {
                loadedPositions.push_back(getElement(iPositions, svert[0]));
                loadedUVs.push_back(getElement(iTCoords, svert[1]));
                loadedNormals.push_back(getElement(iNormals, svert[2]));
                break;
            }
            default:
            {
                break;
            }
            }
        }

        // take care of missing normals
        if (noNormal)
        {
            glm::vec3 A = loadedPositions[0] - loadedPositions[1];
            glm::vec3 B = loadedPositions[2] - loadedPositions[1];

            glm::vec3 normal = glm::cross(A, B);

            for (int i = 0; i < int(loadedPositions.size()); i++)
            {
                loadedNormals[i] = normal;
            }
        }
    }

    
    // ---------
    
    // Get first token of string
    inline std::string firstToken(const std::string &in)
    {
        if (!in.empty())
        {
            size_t token_start = in.find_first_not_of(" \t");
            size_t token_end = in.find_first_of(" \t", token_start);
            if (token_start != std::string::npos && token_end != std::string::npos)
            {
                return in.substr(token_start, token_end - token_start);
            }
            else if (token_start != std::string::npos)
            {
                return in.substr(token_start);
            }
        }
        return "";
    }

    // Get tail of string after first token and possibly following spaces
    inline std::string tail(const std::string &in)
    {
        size_t token_start = in.find_first_not_of(" \t");
        size_t space_start = in.find_first_of(" \t", token_start);
        size_t tail_start = in.find_first_not_of(" \t", space_start);
        size_t tail_end = in.find_last_not_of(" \t");
        if (tail_start != std::string::npos && tail_end != std::string::npos)
        {
            return in.substr(tail_start, tail_end - tail_start + 1);
        }
        else if (tail_start != std::string::npos)
        {
            return in.substr(tail_start);
        }
        return "";
    }
    
    // Split a String into a string array at a given token
    inline void split(const std::string &in,
        std::vector<std::string> &out,
        std::string token)
    {
        out.clear();

        std::string temp;

        for (int i = 0; i < int(in.size()); i++)
        {
            std::string test = in.substr(i, token.size());

            if (test == token)
            {
                if (!temp.empty())
                {
                    out.push_back(temp);
                    temp.clear();
                    i += (int)token.size() - 1;
                }
                else
                {
                    out.push_back("");
                }
            }
            else if (i + token.size() >= in.size())
            {
                temp += in.substr(i, token.size());
                out.push_back(temp);
                break;
            }
            else
            {
                temp += in[i];
            }
        }
    }
    
    // Get element at given index position
    template <class T>
    inline const T & getElement(const std::vector<T> &elements, std::string &index)
    {
        int idx = std::stoi(index);
        if (idx < 0)
            idx = int(elements.size()) + idx;
        else
            idx--;
        return elements[idx];
    }
    
};


#endif
