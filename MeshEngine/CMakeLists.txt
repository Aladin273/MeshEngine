cmake_minimum_required(VERSION 3.10)
project(MeshEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(MESH_ENGINE_HEADERS
    Misc/Export.h    
    Misc/Timer.h
    Misc/Logger.h
    Misc/DynamicLibrary.h
    Misc/Settings.h
    
    Math/HalfEdge.h
    Math/Intersections.h
    Math/BoundingBox.h
    Math/Ray.h
    Math/Contact.h
    
    Base/Base.h
    Base/RenderBase.h
    Base/Texture.h
    Base/Material.h
    Base/Mesh.h
    Base/Sprite.h

    Scene/Scene.h

    Node/Node.h
    Node/MeshNode.h
    Node/SpriteNode.h
    Node/LightNode.h
    Node/DirLightNode.h
    Node/PointLightNode.h
    Node/SpotLightNode.h

    Parser/AssimpParser.h
    Parser/ColladaParser.h
    Parser/STLParser.h

    RenderSystem/GuiSystem.h
    RenderSystem/RenderSystem.h
    RenderSystem/Window.h
    RenderSystem/Shader.h

    RenderSystem/OpenGL/GLGuiSystem.h
    RenderSystem/OpenGL/GLRenderSystem.h
    RenderSystem/OpenGL/GLShader.h
    RenderSystem/OpenGL/GLWindow.h

    Viewport/Camera.h
    Viewport/Viewport.h
)

set(MESH_ENGINE_SOURCES
    Misc/Export.cpp
    Misc/Timer.cpp
    Misc/Logger.cpp
    Misc/DynamicLibrary.cpp
    Misc/Settings.cpp

    Math/HalfEdge.cpp
    Math/Intersections.cpp
    Math/BoundingBox.cpp
    Math/Ray.cpp
    Math/Contact.cpp

    Base/Base.cpp
    Base/RenderBase.cpp
    Base/Texture.cpp
    Base/Material.cpp
    Base/Mesh.cpp
    Base/Sprite.cpp

    Scene/Scene.cpp
    
    Node/Node.cpp
    Node/MeshNode.cpp
    Node/SpriteNode.cpp
    Node/LightNode.cpp
    Node/DirLightNode.cpp
    Node/PointLightNode.cpp
    Node/SpotLightNode.cpp

    Parser/AssimpParser.cpp
    Parser/ColladaParser.cpp
    Parser/STLParser.cpp

    RenderSystem/OpenGL/GLGuiSystem.cpp
    RenderSystem/OpenGL/GLRenderSystem.cpp
    RenderSystem/OpenGL/GLShader.cpp
    RenderSystem/OpenGL/GLWindow.cpp

    Viewport/Camera.cpp
    Viewport/Viewport.cpp
)

if (${SHIPPING_PRESET})
    add_compile_definitions(DATA_PATH="Data/")
    add_compile_definitions(SHADERS_PATH="Shaders/")
    add_compile_definitions(RESOURCES_PATH="Resources/")
else()
    add_compile_definitions(DATA_PATH="../../../Data/")
    add_compile_definitions(SHADERS_PATH="../../../Shaders/")
    add_compile_definitions(RESOURCES_PATH="../../../Resources/")
endif()

add_subdirectory(ThirdParty)
add_library(MeshEngine STATIC ${MESH_ENGINE_HEADERS} ${MESH_ENGINE_SOURCES})

target_include_directories(MeshEngine PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(MeshEngine PUBLIC glfw glad glm assimp tinyxml2 stb spdlog imgui imguizmo)