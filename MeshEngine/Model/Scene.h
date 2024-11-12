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

class Scene : public Base
{
public:
    Scene();
    virtual ~Scene();

public:
    void init(RenderSystem* rs);

public:
    virtual void bind() override
    {
        bindProperty(cameraLight);
        bindProperty(castShadows);
        bindProperty(lightDirection);
        bindPropertyEx(Property::Color, "backgroundColor", backgroundColor);

        super::bind();
    }

    bool cameraLight = true;
    bool castShadows = false;

    glm::vec3 lightDirection{ -0.5f, -0.75f, -1.0f };
    glm::vec4 backgroundColor{ 0.15f, 0.15f, 0.15f, 1.0f };

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

    Node* getNodeByID(uint32_t id);

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
    void update(float deltaTime);

protected:
    void updateDepth(float deltaTime);
    void updateScene(float deltaTime);
    
    void requestDelete();

protected:
    RenderSystem* m_renderSystem = nullptr;
    std::unique_ptr<Shader> m_shaderScene;
    std::unique_ptr<Shader> m_shaderDepth;

    uint32_t m_renderTarget = 0;
    Viewport* m_viewport = nullptr;

    uint32_t m_depthId = 0;
    uint32_t m_depthTextureId = 0;
    uint32_t m_depthWidth = 4096;
    uint32_t m_depthHeight = 4096;
    glm::mat4 m_lightSpaceMatrix{ 1.0f };

private:
    Node* m_deleted = nullptr;
    std::vector<std::unique_ptr<Node>> m_nodes;
};

