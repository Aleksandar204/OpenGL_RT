#pragma once

#include <rtEngine/mesh.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model
{
public:
    std::vector<Mesh> meshes;
    std::string directory;
    void loadModel(std::string model_path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    /// @brief Loads textures for a .obj model using the data from the .mtl file. Other filetypes require manual loading of textures.
    /// @param mat 
    /// @param type 
    /// @param typeName 
    /// @return std::vector<Texture*> textures
    std::vector<Texture*> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
    /// @brief Loads a model using assimp located at the modelPath. Plz use obj files for comlpex objects, fbx has issues with files containing multiple meshes.
    /// @param modelPath Path of the model file
    Model(std::string model_path);
    /// @brief Loads a model using assimp located at the modelPath and gives every mesh the specified shader
    /// @param modelPath Path of the model file
    /// @param sh Default shader for meshes of this model
    Model(std::string model_path, Shader sh);
};