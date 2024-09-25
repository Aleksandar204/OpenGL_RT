#include <rtEngine/model.hpp>

void Model::loadModel(std::string model_path)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(model_path, aiProcess_Triangulate | aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << importer.GetErrorString() << std::endl;
        throw new std::runtime_error(importer.GetErrorString());
    }
    directory = model_path.substr(0, model_path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    for (GLuint i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    for (GLuint i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture *> diffuseMaps;
    std::vector<Texture *> specularMaps;
    std::vector<Texture *> normalMaps;
    std::vector<Texture *> heightMaps;
    std::vector<Texture *> emissionMaps;

    for (GLuint i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vec;

        vec.x = mesh->mVertices[i].x;
        vec.y = mesh->mVertices[i].y;
        vec.z = mesh->mVertices[i].z;
        vertex.position = vec;

        vec.x = mesh->mNormals[i].x;
        vec.y = mesh->mNormals[i].y;
        vec.z = mesh->mNormals[i].z;
        vertex.normal = vec;

        if (mesh->mTextureCoords[0])
        {
            glm::vec2 v;
            v.x = mesh->mTextureCoords[0][i].x;
            v.y = mesh->mTextureCoords[0][i].y;
            vertex.tex_coords = v;
        }
        else
            vertex.tex_coords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    for (GLuint i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (GLuint j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
    heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height");
    emissionMaps = loadMaterialTextures(material, aiTextureType_EMISSIVE, "texture_emission");
    return Mesh(vertices, indices, diffuseMaps, specularMaps, normalMaps, heightMaps, emissionMaps);
}

std::vector<Texture *> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture *> textures;
    // std::cout << mat->GetTextureCount(type) << std::endl;
    for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for (auto tex : m_loaded_textures)
        {
            std::string filepath = directory + '/' + std::string(str.C_Str());
            if(std::strcmp(tex->path.data(), filepath.c_str()) == 0)
            {
                skip = true;
                textures.push_back(tex);
                break;
            }
        }
        if(skip)
            continue;
        std::string filepath = directory + '/' + std::string(str.C_Str());
        std::cout << "Loading texture: " << filepath << " as " << type << std::endl;
        Texture *tex = new Texture(filepath.c_str());
        tex->type = typeName;
        textures.push_back(tex);
        m_loaded_textures.push_back(tex);
    }
    return textures;
}

Model::Model(std::string model_path)
{
    loadModel(model_path);
}

// Model::Model(std::string model_path, Shader sh)
// {
//     loadModel(model_path);
//     for (GLuint i = 0; i < meshes.size(); i++)
//     {
//         meshes[i].raster_shader = sh;
//     }
// }