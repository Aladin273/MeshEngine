#pragma once

#include <string>
#include <memory>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "MeshEngine/Model/Model.h"
#include "MeshEngine/Model/MeshNode.h"

class AssimpParser
{    
public:
    std::unique_ptr<Model> loadModel(const std::string& filename);
    //void saveModel(const Model& model, const std::string& filename);

private:
    std::unique_ptr<MeshNode> loadNode(Node* parent, aiNode* aNode, const aiScene* aScene);
    std::unique_ptr<Mesh> loadMesh(aiMesh* aMesh, const aiScene* aScene);
    Material loadMaterial(aiMaterial* aMat);
    std::vector<Texture> loadTextures(aiMaterial* aMat, aiTextureType aType, std::string typeName);

    //void saveNode(Node* parent);
    //void saveMesh(Mesh* mesh);
    //void saveMaterial(const Material& material);

    glm::vec2 convertVec2(const aiVector2D& vec);
    glm::vec3 convertVec3(const aiVector3D& vec);
    glm::quat convertQuat(const aiQuaternion& pOrientation);
    glm::mat4 convertMatrix(const aiMatrix4x4& from);

    std::string m_filename;
    std::string m_directory;
};
