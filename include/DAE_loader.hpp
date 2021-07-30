#ifndef DAE_LOADER_H
#define DAE_LOADER_H

#include "Loader.hpp"
#include <iostream>
#include <tinyxml.h>
#include <map>

class DAE_loader: public Loader
{
public:
    
    DAE_loader(){}
    ~DAE_loader(){}
    
    bool loadFromFile(std::string path) override
    {
        bool loadOkay = _daeFile.LoadFile(path.c_str());
        
        if (loadOkay)
        {     
            TiXmlElement* element = _daeFile.RootElement()->FirstChildElement("library_geometries");
            
            // parse geometry
            for(TiXmlElement* geometry = element->FirstChildElement("geometry"); geometry != 0; geometry = geometry->NextSiblingElement("geometry"))
            {
                std::cout << "geometry loaded : " << geometry->Attribute("id") << std::endl;
                for(TiXmlElement* mesh = geometry->FirstChildElement("mesh"); mesh != 0; mesh = mesh->NextSiblingElement("mesh"))
                {
                    for(TiXmlNode* triangles = mesh->FirstChild("triangles"); triangles != 0; triangles = triangles->NextSibling("triangles"))
                    {
                        loadedPositions.clear();
                        loadedNormals.clear();
                        Mesh tmpMesh = Mesh();

                        
                        int vertex_offset(-1), normal_offset(-1), map_offset(-1);
                        triangles->FirstChildElement("input")->QueryIntAttribute("offset", &vertex_offset);
                        triangles->FirstChildElement("input")->NextSiblingElement()->QueryIntAttribute("offset", &normal_offset);
                        triangles->FirstChildElement("input")->NextSiblingElement()->NextSiblingElement()->QueryIntAttribute("offset", &map_offset);
                        std::string indices_text = triangles->FirstChildElement("p")->GetText();
                        
                        TiXmlNode* source = mesh->FirstChild("source");
                        std::string positions_text = source->FirstChildElement("float_array")->GetText();
                        std::string normals_text = source->NextSibling("source")->FirstChildElement("float_array")->GetText();
                        std::string map_text;
                        if(map_offset != -1) map_text = source->NextSibling("source")->NextSibling("source")->FirstChildElement("float_array")->GetText();
                        
                        std::stringstream sst(positions_text), ssn(normals_text), ssm(map_text), ssi(indices_text);
                        std::string s;
                        int offset = 0;
                        
                        while (getline(sst, s, ' ') ) {loadedPositions.push_back(stof(s));}
                        while (getline(ssn, s, ' ') ) {loadedNormals.push_back(stof(s));}
                        if(map_offset != -1) while(getline(ssm, s, ' ') ) {loadedUVs.push_back(stof(s));}
                        
                       // std::cout << "  vertex offset " << vertex_offset << std::endl;
                       // std::cout << "  normal offset " << normal_offset << std::endl;
                       // std::cout << "  map offset " << map_offset << std::endl;

                        
                        while (getline(ssi, s, ' ') ) 
                        {
                            int position_index = stoi(s) * 3;
                            
                            if (offset == vertex_offset){
                                tmpMesh.positions.push_back(glm::vec3(  loadedPositions[position_index], 
                                                                        loadedPositions[position_index + 1], 
                                                                        loadedPositions[position_index + 2]) );
                            }
                            else if (offset == normal_offset){
                                tmpMesh.normals.push_back(glm::vec3(    loadedNormals[position_index], 
                                                                        loadedNormals[position_index + 1], 
                                                                        loadedNormals[position_index + 2]) );
                                if (map_offset == -1) 
                                {
                                    offset = 0;
                                    continue; 
                                }
                            }
                            else if (offset == map_offset)
                            {
                                position_index = stoi(s) * 2;
                                tmpMesh.uvs.push_back(glm::vec2(    loadedUVs[position_index], 
                                                                    loadedUVs[position_index + 1]));
                                offset = 0;
                                continue;
                            }
                            offset++;
                        }
                              
                        tmpMesh.material.Kd = glm::vec3(0.2, 0.5, 0.1);
                        tmpMesh.material.name = triangles->ToElement()->Attribute("material");
                        tmpMesh.material.name = tmpMesh.material.name.substr(0, tmpMesh.material.name.size() - 9);
                        loadedMeshes.push_back(tmpMesh);
                        //std::cout << "  vertices size " << tmpMesh.positions.size() << std::endl;
                    }
                   
                }
            }
            
            int meshesSize = loadedMeshes.size();
            if (meshesSize == 0) std::cout << "No meshes loaded. " << std::endl;
            else if (meshesSize == 1)  std::cout << "Loaded 1 mesh." << std::endl;
            else std::cout << "Loaded " << loadedMeshes.size() << " meshes. " << std::endl;
            
            element = _daeFile.RootElement()->FirstChildElement("library_effects");
            // parse effect
            for(TiXmlElement* effect = element->FirstChildElement("effect"); effect != 0; effect = effect->NextSiblingElement("effect"))
            {
                //std::cout << effect->Attribute("id") << std::endl;
                
                std::string effname = effect->Attribute("id");
                effname = effname.substr(0, effname.size() - 7);
                
                Material tmpMat = Material();
                tmpMat.name = effname;
                
                std::string diffcolor ;/*=*/ 
                TiXmlNode * diffuse = effect    ->FirstChildElement("profile_COMMON")
                                                ->FirstChildElement("technique")
                                                ->FirstChildElement("phong");
                if (diffuse == 0) 
                {
                    diffuse = effect    ->FirstChildElement("profile_COMMON")
                                        ->FirstChildElement("technique")
                                        ->FirstChildElement("lambert");
                }
                diffuse = diffuse->FirstChildElement("diffuse");
                
                

                TiXmlNode * diffuse_color = diffuse->FirstChildElement("color");
                
                if (diffuse_color == 0) 
                {

                    tmpMat.Kd = glm::vec3(0);
                    TiXmlNode * diffuse_texture = diffuse->FirstChildElement("texture");
                    if (diffuse_texture != 0)
                    {
                        std::string sampler =  diffuse_texture->ToElement()->Attribute("texture");
                        std::string surface_map;

                        TiXmlNode * common = effect->FirstChildElement("profile_COMMON");
                        for(TiXmlElement* newparam = common->FirstChildElement("newparam"); newparam != 0; newparam = newparam->NextSiblingElement("newparam"))
                        {
                            if (newparam->Attribute("sid") == sampler) {
                                surface_map = newparam->FirstChildElement("sampler2D")->FirstChildElement("source")->GetText();
                                break;
                            }
                        }
                        
                        for(TiXmlElement* newparam = common->FirstChildElement("newparam"); newparam != 0; newparam = newparam->NextSiblingElement("newparam"))
                        {
                            if (newparam->Attribute("sid") == surface_map) {
                                surface_map = newparam->FirstChildElement("surface")->FirstChildElement("init_from")->GetText();
                                break;
                            }
                        }
                        
                        TiXmlElement * image = _daeFile.RootElement()->FirstChildElement("library_images");

                        for(TiXmlElement* img = image->FirstChildElement("image"); img != 0; img = img->NextSiblingElement("image"))
                        {
                            if (img->Attribute("id") == surface_map) {
                                tmpMat.map_Kd = img->FirstChildElement("init_from")->GetText();
                                tmpMat.have_map_Kd = true;
                                break;
                            }
                        }
                    }
                }
                else {
                    diffcolor = diffuse_color->ToElement()->GetText();

                    std::stringstream sst(diffcolor);
                    std::string s; float colors [4]; int i = 0;
                    colors[0] = 0; colors[1] = 0; colors[2] = 0; colors[3] = 1;
                    while (getline(sst, s, ' ') ) {colors[i] = stof(s); ++i;}
                    
                    tmpMat.Kd = glm::vec3(colors[0], colors[1], colors[2]);   
                }
                parsedMat[tmpMat.name] = tmpMat;
            }
            

            for (int i = 0 ; i < loadedMeshes.size(); i++)
            {
                loadedMeshes.at(i).material = parsedMat[loadedMeshes.at(i).material.name];
            }
                        
            return true;
        }
        else
        {
            std::cout << path << " failed to load file" << std::endl;
            return false;
        }    
    }
    
private:
    TiXmlDocument _daeFile;
    
    std::vector<float> loadedPositions;
    std::vector<float> loadedNormals;
    std::vector<float> loadedUVs;
    
    std::map <std::string, Material> parsedMat;
    
    
};

#endif
