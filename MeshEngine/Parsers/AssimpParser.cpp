#include "AssimpParser.h"

#include "MeshEngine/Misc/Logger.h"

std::unique_ptr<Model> AssimpParser::loadModel(const std::string& filename)
{
    MeshEngine::Logger::info("AssimpParser loading from {:}", filename);

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes);
    //const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals | aiProcess_ForceGenNormals | aiProcess_EmbedTextures | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        MeshEngine::Logger::error("{:}", importer.GetErrorString());   
        return {};
    }

    m_filename = filename;
    m_directory = filename.substr(0, filename.find_last_of('\\'));
    
    std::unique_ptr<Model> model = std::make_unique<Model>();
    
    model->setName(filename);
    model->attachNode(loadNode(nullptr, scene->mRootNode, scene));

    return model;
}

std::unique_ptr<Node> AssimpParser::loadNode(Node* parent, aiNode* aNode, const aiScene* aScene)
{
    std::unique_ptr<Node> node = std::make_unique<Node>();
    
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
        // Position
        vhs.push_back(table.addVertex(Vertex(convertVec3(aMesh->mVertices[i]))));
        
        // Normals

        //vector.x = aMesh->mNormals[i].x;
        //vector.y = aMesh->mNormals[i].y;
        //vector.z = aMesh->mNormals[i].z;
        //vertex.Normal = vector;

        // TexCoords

        //if (aMesh->mTextureCoords[0])
        //{
        //    glm::vec2 vec;
        //
        //    vec.x = aMesh->mTextureCoords[0][i].x;
        //    vec.y = aMesh->mTextureCoords[0][i].y;
        //    vertex.TexCoords = vec;
        //}
        //else
        //    vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        // Tangents

        //vector.x = aMesh->mTangents[i].x;
        //vector.y = aMesh->mTangents[i].y;
        //vector.z = aMesh->mTangents[i].z;
        //vertex.Tangent = vector;

        // Bitangents

        //vector.x = aMesh->mBitangents[i].x;
        //vector.y = aMesh->mBitangents[i].y;
        //vector.z = aMesh->mBitangents[i].z;
        //vertex.Bitangent = vector;

        //vertices.push_back(vertex);
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

    // Textures

    //std::vector<Texture> textures;

    //std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    //textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    //
    //std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    //textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    //
    //std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    //textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    //
    //std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    //textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>(table);
    mesh->setName(aMesh->mName.C_Str());

    return mesh;
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