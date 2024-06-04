cmake_minimum_required(VERSION 3.10)
project(MeshEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(MESH_ENGINE_HEADERS
    HalfEdge/HalfEdge.h

    Logger/Logger.h

    Manipulators/Manipulator.h
    Manipulators/RotationManipulator.h
    Manipulators/ScaleManipulator.h
    Manipulators/TranslationManipulator.h
    Manipulators/Triad.h
    
    Math/Intersections.h
    
    Misc/DynamicLibrary.h
    Misc/Timer.h
    
    Model/Node.h
    Model/Mesh.h
    Model/Model.h

    Parsers/AssimpParser.h
    Parsers/ColladaParser.h
    Parsers/STLParser.h

    RenderSystem/Export.h

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
    HalfEdge/HalfEdge.cpp

    Manipulators/Manipulator.cpp
    Manipulators/RotationManipulator.cpp
    Manipulators/ScaleManipulator.cpp
    Manipulators/TranslationManipulator.cpp
    Manipulators/Triad.cpp

    Math/Intersections.cpp

    Misc/DynamicLibrary.cpp
    Misc/Timer.cpp

    Model/Node.cpp
    Model/Mesh.cpp
    Model/Model.cpp
    
    Parsers/AssimpParser.cpp
    Parsers/ColladaParser.cpp
    Parsers/STLParser.cpp

    RenderSystem/Export.cpp

    RenderSystem/OpenGL/GLGuiSystem.cpp
    RenderSystem/OpenGL/GLRenderSystem.cpp
    RenderSystem/OpenGL/GLShader.cpp
    RenderSystem/OpenGL/GLWindow.cpp

    Viewport/Camera.cpp
    Viewport/Viewport.cpp
)

add_subdirectory(ThirdParty)
add_library(MeshEngine STATIC ${MESH_ENGINE_SOURCES})

target_include_directories(MeshEngine PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(MeshEngine PUBLIC glfw glad glm tinyxml2 stb imgui spdlog)