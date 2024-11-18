#include "AssimpParser.h"

#include "MeshEngine/Misc/Logger.h"
#include "MeshEngine/Model/MeshNode.h"

std::unique_ptr<Node> AssimpParser::loadModel(const std::string& filename)
{
    MeshEngine::Logger::info("AssimpParser loading from {:}", filename);

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
    //const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals | aiProcess_ForceGenNormals | aiProcess_EmbedTextures | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        MeshEngine::Logger::error("{:}", importer.GetErrorString());   
        return std::make_unique<Node>();
    }

    m_filename = filename;
    m_directory = filename.substr(0, filename.find_last_of('\\'));
    
    std::unique_ptr<Node> model = loadNode(nullptr, scene->mRootNode, scene);
    model->setName(filename);

    return model;
}

std::unique_ptr<Node> AssimpParser::loadNode(Node* parent, aiNode* aNode, const aiScene* aScene)
{
    std::unique_ptr<MeshNode> node = std::make_unique<MeshNode>();
    
    node->setName(aNode->mName.C_Str());
    node->setRelativeTransform(convertMatrix(aNode->mTransformation));

    for (size_t i = 0; i < aNode->mNumMeshes; ++i)
    {
        aiMesh* aMesh = aScene->mMeshes[aNode->mMeshes[i]];
        node->attachMesh(loadMesh(aMesh, aScene));
    }

    for (size_t i = 0; i < aNode->mNumChildren; ++i)
    {
        loadNode(node.get(), aNode->mChildren[i], aScene);
    }

    if (parent)
        parent->attachNode(std::move(node));

    return node;
}

std::unique_ptr<Mesh> AssimpParser::loadMesh(aiMesh* aMesh, const aiScene* aScene)
{
    heds::HalfEdgeTable<Vertex> table;

    std::vector<heds::VertexHandle> vhs;

    for (size_t i = 0; i < aMesh->mNumVertices; ++i)
    {
        Vertex vertex;

        // Position
        vertex.position = convertVec3(aMesh->mVertices[i]);
        
        // Normals
        vertex.normal = convertVec3(aMesh->mNormals[i]);

        // TexCoords
        if (aMesh->mTextureCoords[0]) vertex.texCoords = glm::vec2(convertVec3(aMesh->mTextureCoords[0][i]));

        // Tangents
        //vertex.tangent = convertVec3(aMesh->mTangents[i]);

        // Bitangents
        //vertex.bitangent = convertVec3(aMesh->mBitangents[i]);

        vhs.push_back(table.addVertex(vertex));
    }

    for (size_t i = 0; i < aMesh->mNumFaces; ++i)
    {
        aiFace aFace = aMesh->mFaces[i];

        heds::VertexHandle vh0 = vhs[aFace.mIndices[0]];
        heds::VertexHandle vh1 = vhs[aFace.mIndices[1]];
        heds::VertexHandle vh2 = vhs[aFace.mIndices[2]];

        table.addFace(vh0, vh1, vh2);
    }

    table.connectTwins();

    // Materials
    aiMaterial* aMat = aScene->mMaterials[aMesh->mMaterialIndex];
    
    std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>(table, loadMaterial(aMat));
    mesh->setName(aMesh->mName.C_Str());

    return mesh;
}

Material AssimpParser::loadMaterial(aiMaterial* aMat)
{   
    std::vector<Texture> diffuseMaps = loadTextures(aMat, aiTextureType_DIFFUSE, "texture_diffuse");
    std::vector<Texture> specularMaps = loadTextures(aMat, aiTextureType_SPECULAR, "texture_specular");
    std::vector<Texture> emissionMaps = loadTextures(aMat, aiTextureType_EMISSIVE, "texture_emission");
    std::vector<Texture> normalMaps = loadTextures(aMat, aiTextureType_HEIGHT, "texture_normal");
    std::vector<Texture> heightMaps = loadTextures(aMat, aiTextureType_AMBIENT, "texture_height");

    Material material;
    
    if (!diffuseMaps.empty()) material.diffuseMap = diffuseMaps.front();
    if (!specularMaps.empty()) material.specularMap = specularMaps.front();
    if (!emissionMaps.empty()) material.emissionMap = emissionMaps.front();

    return material;
}

std::vector<Texture> AssimpParser::loadTextures(aiMaterial* aMat, aiTextureType aType, std::string typeName)
{
    std::vector<Texture> textures;

    for (size_t i = 0; i < aMat->GetTextureCount(aType); ++i)
    {
        aiString str;
        aMat->GetTexture(aType, i, &str);

        Texture texture;
        texture.id = 0;
        texture.type = typeName;
        texture.path = m_directory + "\\" + std::string(str.C_Str());

        textures.push_back(texture);
    }

    return textures;
}

glm::vec2 AssimpParser::convertVec2(const aiVector2D& vec)
{
    return glm::vec2(vec.x, vec.y);
}

glm::vec3 AssimpParser::convertVec3(const aiVector3D& vec)
{
    return glm::vec3(vec.x, vec.y, vec.z);
}

glm::quat AssimpParser::convertQuat(const aiQuaternion& pOrientation)
{
    return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
}

glm::mat4 AssimpParser::convertMatrix(const aiMatrix4x4& from)
{
    glm::mat4 to;

    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;

    return to;
}