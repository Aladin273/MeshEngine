#pragma once

#include <string>
#include <glm/glm.hpp>

#include "MeshEngine/Base/Material.h"

namespace MeshEngine
{
    namespace Settings
    {
        // General
        const int invalid = -1;

        // Viewport
        const int x = 0;
        const int y = 0;
        const int width = 1920;
        const int height = 1080;
        const double fov = 60.0;
        const double znear = 0.1f;
        const double zfar = 100000.f;

        // Zoom
        const float zoomIn = 1.1f;
        const float zoomOut = 0.9f;

        // Light
        const glm::vec3 ambient = { 0.2f, 0.2f, 0.2f };
        const glm::vec3 diffuse = { 1.f, 1.f, 1.f };
        const glm::vec3 specular = { 1.f, 1.f, 1.f };

        // Camera
        const glm::vec3 eye = { 0,0,10 };
        const glm::vec3 target = { 0,0,0 };
        const glm::vec3 up = { 0,1,0 };
        const glm::vec3 worldUp = { 0,1,0 };

        // Octree
        const float bounds = 10000.f;
        const int depth = 8;
        const int objects = 4;

        // Axes
        const glm::vec3 axisX = { 1.0f, 0.0f, 0.0f };
        const glm::vec3 axisY = { 0.0f, 1.0f, 0.0f };
        const glm::vec3 axisZ = { 0.0f, 0.0f, 1.0f };

        // Colors
        const glm::vec4 colorWhite = { 1.0f, 1.0f, 1.0f, 1.0f };
        const glm::vec4 colorGray = { 0.5f, 0.5f, 0.5f, 1.0f };
        const glm::vec4 colorBlack = { 0.0f, 0.0f, 0.0f, 1.0f };

        const glm::vec4 colorRed = { 1.0f, 0.0f, 0.0f, 1.0f };
        const glm::vec4 colorGreen = { 0.0f, 1.0f, 0.0f, 1.0f };
        const glm::vec4 colorBlue = { 0.0f, 0.0f, 1.0f, 1.0f };

        // Materials
        const Material materialRuby{ glm::vec3(1.07, 0.01, 0.01), glm::vec4(0.61, 0.04, 0.04, 0.85), glm::vec3(0.72, 0.62, 0.62), glm::vec3(0.0), 75.f };
        const Material materialEmerald{ glm::vec3(0.1, 0.9, 0.5), glm::vec4(0.08, 0.61, 0.58, 0.55), glm::vec3(0.63, 0.73, 0.63), glm::vec3(0.0), 128.f };

        const Material materialGold{ glm::vec3(0.25, 0.22, 0.06), glm::vec4(0.35, 0.31, 0.09, 1.0), glm::vec3(0.80, 0.72, 0.21), glm::vec3(0.0), 83.2f };
        const Material materialCopper{ glm::vec3(1.09, 0.07, 0.02), glm::vec4(0.70, 0.27, 0.08, 1.0), glm::vec3(0.26, 1.04, 0.09), glm::vec3(0.0), 12.8f };
        const Material materialChrome{ glm::vec3(0.25, 0.25, 0.25), glm::vec4(0.4, 0.4, 0.4, 1.0), glm::vec3(0.77, 0.77, 0.77), glm::vec3(0.0), 75.f };

        const Material materialDark{ glm::vec3(0.15f), glm::vec4(0.15f, 0.15f, 0.15f, 1.0f), glm::vec3(0.15f), glm::vec3(0.0f), 128.f };
        const Material materialClear{ glm::vec3(0.0f), glm::vec4(1.0f), glm::vec3(1.0f), glm::vec3(0.0f), 128.f };

        const Material materialRed{ glm::vec3(0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), glm::vec3(0.0f), glm::vec3(0.0f), 128.f };
        const Material materialGreen{ glm::vec3(0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec3(0.0f), glm::vec3(0.0f), 128.f };
        const Material materialBlue{ glm::vec3(0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec3(0.0f), glm::vec3(0.0f), 128.f };
        const Material materialYellow{ glm::vec3(0.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec3(0.0f), glm::vec3(0.0f), 128.f };
        const Material materialWhite{ glm::vec3(0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec3(0.0f), glm::vec3(0.0f), 128.f };
        const Material materialGray{ glm::vec3(0.0f), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), glm::vec3(0.0f), glm::vec3(0.0f), 128.f };
        const Material materialBlack{ glm::vec3(0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec3(0.0f), glm::vec3(0.0f), 128.f };

        // Paths
        const std::string dataPath = DATA_PATH;
        const std::string shadersPath = SHADERS_PATH;
        const std::string resourcesPath = RESOURCES_PATH;

        // Application
        const std::string title = "MeshEditor";
        const std::string icon = resourcesPath + "icon.png";
    };
};
