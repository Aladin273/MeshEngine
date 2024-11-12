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

    HalfEdge/HalfEdge.h

    Manipulators/Manipulator.h
    Manipulators/RotationManipulator.h
    Manipulators/ScaleManipulator.h
    Manipulators/TranslationManipulator.h
    Manipulators/Triad.h
    
    Math/Intersections.h
    Math/BoundingBox.h
    Math/Ray.h
    Math/Contact.h
    
    Base/Base.h
    Base/Texture.h
    Base/Material.h
    Base/Mesh.h

    Model/Model.h
    Model/Scene.h
    Model/Node.h
    Model/MeshNode.h

    Parsers/AssimpParser.h
    Parsers/ColladaParser.h
    Parsers/STLParser.h

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

    HalfEdge/HalfEdge.cpp

    Manipulators/Manipulator.cpp
    Manipulators/RotationManipulator.cpp
    Manipulators/ScaleManipulator.cpp
    Manipulators/TranslationManipulator.cpp
    Manipulators/Triad.cpp

    Math/Intersections.cpp
    Math/BoundingBox.cpp
    Math/Ray.cpp
    Math/Contact.cpp

    Base/Base.cpp
    Base/Texture.cpp
    Base/Material.cpp
    Base/Mesh.cpp

    Model/Model.cpp
    Model/Scene.cpp
    Model/Node.cpp
    Model/MeshNode.cpp
    
    Parsers/AssimpParser.cpp
    Parsers/ColladaParser.cpp
    Parsers/STLParser.cpp

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