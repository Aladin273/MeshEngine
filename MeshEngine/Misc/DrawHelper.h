#pragma once

#include <glm/glm.hpp>

class RenderSystem;

namespace MeshEngine
{
    class DrawHelper
    {
    public:
        static void drawDebugTriangle(RenderSystem* renderSystem, glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec4 color = glm::vec4(1.f), bool fill = false, float thickness = 2.0f);
        static void drawDebugQuad(RenderSystem* renderSystem, glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec4 color = glm::vec4(1.f), bool fill = false, float thickness = 2.0f);
        static void drawDebugPoint(RenderSystem* renderSystem, glm::vec3 point, glm::vec4 color = glm::vec4(1.f), float thickness = 10.0f);
        static void drawDebugLine(RenderSystem* renderSystem, glm::vec3 start, glm::vec3 end, glm::vec4 color = glm::vec4(1.f), float thickness = 10.0f);
        
        static void drawDebugSphere(RenderSystem* renderSystem, glm::vec3 position, float radius, glm::vec4 color = glm::vec4(1.f), bool fill = false, float thickness = 2.0f);
        static void drawDebugSphere(RenderSystem* renderSystem, glm::vec3 position, glm::vec3 size, glm::vec4 color = glm::vec4(1.f), bool fill = false, float thickness = 2.0f);
        
        static void drawDebugBox(RenderSystem* renderSystem, glm::vec3 position, glm::vec3 size, glm::vec4 color = glm::vec4(1.f), bool fill = false, float thickness = 2.0f);
        static void drawDebugBox(RenderSystem* renderSystem, const glm::mat4& transform, glm::vec3 min, glm::vec3 max, glm::vec4 color = glm::vec4(1.f), bool fill = false, float thickness = 2.0f);

        //static void drawDebugCone(RenderSystem* renderSystem);
        //static void drawDebugCylinder(RenderSystem* renderSystem);
        
        //static void drawDebugArrow(RenderSystem* renderSystem);

    private:
        static void drawDebugSphereInternal(RenderSystem* renderSystem, const glm::mat4& matrix, glm::vec4 color, bool fill, float thickness);
        static void drawDebugBoxInternal(RenderSystem* renderSystem, const glm::mat4& matrix, glm::vec4 color, bool fill, float thickness);

        static void drawDebugInternal(RenderSystem* renderSystem, uint32_t renderId, const glm::mat4& matrix, glm::vec4 color, uint32_t type, float thickness);
    };
}