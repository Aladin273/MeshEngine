#pragma once

#include "MeshEngine/RenderSystem/RenderSystem.h"
#include "MeshEngine/RenderSystem/Shader.h"

#include "MeshEngine/Misc/Export.h"
#include "MeshEngine/Misc/Settings.h"

#include "MeshEngine/Math/Ray.h"
#include "MeshEngine/Math/BoundingBox.h"
#include "MeshEngine/Math/Contact.h"
#include "MeshEngine/Math/Intersections.h"

#include "MeshEngine/Viewport/Viewport.h"
#include "MeshEngine/Manipulators/Manipulator.h"

#include "MeshEngine/Base/Base.h"

#include "MeshEngine/Model/Node.h"
#include "MeshEngine/Model/MeshNode.h"

struct DirLight
{
    glm::vec3 direction;
    float padding1;

    glm::vec3 ambient;
    float padding2;

    glm::vec3 diffuse;
    float padding3;

    glm::vec3 specular;
    float padding4;
};

struct PointLight
{
    glm::vec3 position;
    float padding1;

    float constant;
    float linear;
    float quadratic;
    float padding2;

    glm::vec3 ambient;
    float padding3;

    glm::vec3 diffuse;
    float padding4;

    glm::vec3 specular;
    float padding5;
};

struct SpotLight
{
    glm::vec3 position;
    float padding1;

    glm::vec3 direction;
    float padding2;

    float cutOff;
    float outerCutOff;
    float constant;
    float linear;

    float quadratic;
    glm::vec3 padding3;

    glm::vec3 ambient;
    float padding4;

    glm::vec3 diffuse;
    float padding5;

    glm::vec3 specular;
    float padding6;
};

struct MatricesUniform
{
    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 lightSpaceMatrix;
};

struct LightsUniform
{
    int32_t numDirLights = 0;
    int32_t numPointLights = 0;
    int32_t numSpotLights = 0;
    int32_t padding = 0;

    DirLight dirLights[16];
    PointLight pointLights[16];
    SpotLight spotLights[16];
};

class Scene : public Base
{
public:
    Scene();
    virtual ~Scene();

public:
    void init(RenderSystem* renderSystem);

public:
    virtual void bind() override
    {
        bindProperty(castShadows);
        bindPropertyEx(Property::Color, "backgroundColor", backgroundColor);

        super::bind();
    }

    bool castShadows = false;
    glm::vec4 backgroundColor{ 0.15f, 0.15f, 0.15f, 1.0f };

public:
    uint32_t matricesUniformId;
    MatricesUniform matricesUniform;

    uint32_t lightsUniformId;
    LightsUniform lightsUniform;

public:
    void setRenderSystem(RenderSystem* rs);
    RenderSystem* getRenderSystem() const;

    void setRenderTarget(uint32_t target);
    uint32_t getRenderTarget() const;

    void setViewport(Viewport& viewport);
    Viewport& getViewport() const;

public:
    const BoundingBox& getBoundingBox();

public:
    void attachNode(std::unique_ptr<Node> node);
    void detachNode(Node* node);

    Node* getNodeById(uint32_t id);

    const std::vector<std::unique_ptr<Node>>& getNodes() const;
    std::vector<std::unique_ptr<Node>>& getNodes();

    template<class Lambda>
    void processRecursive(Lambda lambda)
    {
        for (auto& node : m_nodes)
            node->processRecursive(lambda);
    }

public:
    std::vector<Contact> raycast(const Ray& ray, FilterValue filterValues);

public:
    void start();
    void end();

    void update(float deltaTime);
    void render();

protected:
    void renderDepth();
    void renderScene();
    
    void requestDelete();

protected:
    RenderSystem* m_renderSystem = nullptr;

    uint32_t m_renderTarget = 0;
    Viewport* m_viewport = nullptr;

    uint32_t m_depthId = 0;
    uint32_t m_depthTextureId = 0;
    uint32_t m_depthWidth = 4096;
    uint32_t m_depthHeight = 4096;
    Shader* m_shaderDepth;

private:
    Node* m_deleted = nullptr;
    std::vector<std::unique_ptr<Node>> m_nodes;
};

