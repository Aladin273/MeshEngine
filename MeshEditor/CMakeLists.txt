cmake_minimum_required(VERSION 3.10)
project(MeshEditor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(MESH_EDITOR_HEADERS
    Editor/Editor.h
    Editor/View.h

    Node/PlaneNode.h
    Node/ArrowNode.h
    Node/OriginNode.h

    Operator/Operator.h
    Operator/OperatorDispatcher.h

    Operator/Delete/DeleteFaceOperator.h

    Operator/Edit/EditFaceOperator.h
    Operator/Edit/EditNodeOperator.h
    Operator/Edit/EditVertexOperator.h

    Operator/View/Arcball.h
    Operator/View/Pan.h

    UI/BaseLayer.h
    UI/DockpaneLayer.h
    UI/ConsoleLayer.h
    UI/PropertiesLayer.h
    UI/TreeLayer.h
    UI/ViewportLayer.h
    UI/SettingsLayer.h
)

set(MESH_EDITOR_SOURCES
    Editor/Editor.cpp
    Editor/View.cpp
    
    Node/PlaneNode.cpp
    Node/ArrowNode.cpp
    Node/OriginNode.cpp

    Operator/OperatorDispatcher.cpp

    Operator/Delete/DeleteFaceOperator.cpp
    
    Operator/Edit/EditFaceOperator.cpp
    Operator/Edit/EditNodeOperator.cpp
    Operator/Edit/EditVertexOperator.cpp

    Operator/View/Arcball.cpp
    Operator/View/Pan.cpp

    UI/BaseLayer.cpp
    UI/DockpaneLayer.cpp
    UI/ConsoleLayer.cpp
    UI/PropertiesLayer.cpp
    UI/TreeLayer.cpp
    UI/ViewportLayer.cpp
    UI/SettingsLayer.cpp
    
    Main.cpp
)

add_compile_definitions(DATA_PATH="../../../Data/")
add_compile_definitions(SHADERS_PATH="../../../Shaders/")
add_compile_definitions(RESOURCES_PATH="../../../Resources/")

add_executable(MeshEditor ${MESH_EDITOR_HEADERS} ${MESH_EDITOR_SOURCES})

target_include_directories(MeshEditor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(MeshEditor PRIVATE MeshEngine)

if (${SHIPPING_PRESET})
    add_compile_definitions(DATA_PATH="Data/")
    add_compile_definitions(SHADERS_PATH="Shaders/")
    add_compile_definitions(RESOURCES_PATH="Resources/")

    add_custom_command(TARGET MeshEditor POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_SOURCE_DIR}/"Data/" ${CMAKE_CURRENT_BINARY_DIR}/"Data/")
    add_custom_command(TARGET MeshEditor POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_SOURCE_DIR}/"Shaders/" ${CMAKE_CURRENT_BINARY_DIR}/"Shaders/")    
    add_custom_command(TARGET MeshEditor POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_SOURCE_DIR}/"Resources/" ${CMAKE_CURRENT_BINARY_DIR}/"Resources/")
    add_custom_command(TARGET MeshEditor POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/"Resources/imgui.ini" ${CMAKE_CURRENT_BINARY_DIR}/"imgui.ini")

    if (WIN32)
        target_link_options(MeshEditor PRIVATE "/SUBSYSTEM:WINDOWS" "/ENTRY:mainCRTStartup")
    else()
        target_link_options(MeshEditor PRIVATE "-mwindows")
    endif()

else()
    add_custom_command(TARGET MeshEditor POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_SOURCE_DIR}/"Resources/" ${CMAKE_CURRENT_BINARY_DIR}/"Resources/")
    add_custom_command(TARGET MeshEditor POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/"Resources/imgui.ini" ${CMAKE_CURRENT_BINARY_DIR}/"imgui.ini")
endif()