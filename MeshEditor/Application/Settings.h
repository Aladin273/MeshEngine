#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

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
    const double znear = 0.1;
    const double zfar = 1000;

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

    // Axes
    const glm::vec3 axisX = { 1.0f, 0.0f, 0.0f };
    const glm::vec3 axisY = { 0.0f, 1.0f, 0.0f };
    const glm::vec3 axisZ = { 0.0f, 0.0f, 1.0f };

    // Arrow
    const float pointAR = 0.1f;
    const float shaftAR = 0.05f;
    const float pointAL = 0.2f;
    const float shaftAL = 0.8f;
    const float sizeA = 0.15f;

    // Triad
    const float pointTR = 0.06f;
    const float shaftTR = 0.02f;
    const float pointTL = 0.1f;
    const float shaftTL = 0.8f;
    const float minorTR = 0.02f;
    const float majorTR = 0.98f;
    const float cubeTR = 0.1f;
    const float cubeTL = 0.6f;
    const float sizeT = 0.15f;
    const uint32_t numSubs = 32;

    // Colors
    const glm::vec4 colorBackground = { 0.15f, 0.15f, 0.15f, 1.0f };
    const glm::vec4 colorWhite = { 1.0f, 1.0f, 1.0f, 1.0f};
    const glm::vec4 colorGray = { 0.5f, 0.5f, 0.5f, 1.0f};
    const glm::vec4 colorBlack = { 0.0f, 0.0f, 0.0f, 1.0f};

    const glm::vec4 colorRed = { 1.0f, 0.0f, 0.0f, 1.0f };
    const glm::vec4 colorGreen = { 0.0f, 1.0f, 0.0f, 1.0f };
    const glm::vec4 colorBlue = { 0.0f, 0.0f, 1.0f, 1.0f };

    // Materials
    const Material ruby{ glm::vec3(1.07, 0.01, 0.01), glm::vec4(0.61, 0.04, 0.04, 0.85), glm::vec3(0.72, 0.62, 0.62), glm::vec3(0.0), 75.f };
    const Material emerald{ glm::vec3(0.1, 0.9, 0.5), glm::vec4(0.08, 0.61, 0.58, 0.55), glm::vec3(0.63, 0.73, 0.63), glm::vec3(0.0), 128.f };
    
    const Material gold{ glm::vec3(0.25, 0.22, 0.06), glm::vec4(0.35, 0.31, 0.09, 1.0), glm::vec3(0.80, 0.72, 0.21), glm::vec3(0.0), 83.2f };
    const Material copper{ glm::vec3(1.09, 0.07, 0.02), glm::vec4(0.70, 0.27, 0.08, 1.0), glm::vec3(0.26, 1.04, 0.09), glm::vec3(0.0), 12.8f };
    const Material chrome{ glm::vec3(0.25, 0.25, 0.25), glm::vec4(0.4, 0.4, 0.4, 1.0), glm::vec3(0.77, 0.77, 0.77), glm::vec3(0.0), 75.f };
    
    const Material dark{ glm::vec3(0.15f), glm::vec4(0.15f, 0.15f, 0.15f, 1.0f), glm::vec3(0.15f), glm::vec3(0.0f), 128.f };
    const Material clear{ glm::vec3(0.0f), glm::vec4(1.0f), glm::vec3(1.0f), glm::vec3(0.0f), 128.f };
    
    const Material red    { glm::vec3(0.0f), glm::vec4(0.0f), glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 128.f };
    const Material green  { glm::vec3(0.0f), glm::vec4(0.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 128.f };
    const Material blue   { glm::vec3(0.0f), glm::vec4(0.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), 128.f };
    const Material yellow { glm::vec3(0.0f), glm::vec4(0.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 1.0f), 128.f };

    // Paths
    const std::string dataPath = DATA_PATH;
    const std::string shadersPath = SHADERS_PATH;
    const std::string resourcesPath = RESOURCES_PATH;

    // Models
    const std::string model1 = dataPath + "Cube.stl";
    const std::string model2 = dataPath + "Sphere.stl";
    const std::string model3 = dataPath + "Rhino.stl";
    const std::string model4 = dataPath + "RhinoHoles.stl";
    const std::string model5 = dataPath + "PudgeBig.stl";
    const std::string model6 = dataPath + "Torus.stl";

    const std::string model7 = dataPath + "Cube.dae";
    const std::string model8 = dataPath + "Sphere.dae";
    const std::string model9 = dataPath + "M4A1.dae";
    const std::string model10 = dataPath + "Duck.dae";
    const std::string model11 = dataPath + "Sword.dae";
    const std::string model12 = dataPath + "Cat.dae";

    const std::string model13 = dataPath + "Backpack/backpack.obj";

    // Application
    const std::string title = "MeshEditor";
    const std::string icon = resourcesPath + "icon.png";
};
