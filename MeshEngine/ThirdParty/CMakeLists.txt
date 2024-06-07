cmake_minimum_required(VERSION 3.10)
project(ThirdParty)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GLFW_LIB_NAME glfw)
set(GLFW_INC_PATH ${GLFW_LIB_NAME}/include)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
add_subdirectory(${GLFW_LIB_NAME})

set(GLAD_LIB_NAME glad)
set(GLAD_SRC_PATH ${GLAD_LIB_NAME}/src)
set(GLAD_INC_PATH ${GLAD_LIB_NAME}/include)
add_library(${GLAD_LIB_NAME} STATIC ${GLAD_SRC_PATH}/gl.c)
target_include_directories(${GLAD_LIB_NAME} PUBLIC ${GLAD_INC_PATH})

set(GLM_LIB_NAME glm)
set(GLM_INC_PATH ${GLM_LIB_NAME})
add_library(${GLM_LIB_NAME} INTERFACE)
target_include_directories(${GLM_LIB_NAME} INTERFACE ${GLM_INC_PATH})
target_compile_definitions(${GLM_LIB_NAME} INTERFACE GLM_ENABLE_EXPERIMENTAL)

set(TINYXML2_LIB_NAME tinyxml2)
set(TINYXML2_SRC_PATH ${TINYXML2_LIB_NAME}/src)
set(TINYXML2_INC_PATH ${TINYXML2_LIB_NAME}/include)
add_library(${TINYXML2_LIB_NAME} STATIC ${TINYXML2_SRC_PATH}/tinyxml2.cpp)
target_include_directories(${TINYXML2_LIB_NAME} PUBLIC ${TINYXML2_INC_PATH})

set(STB_LIB_NAME stb)
set(STB_SRC_PATH ${STB_LIB_NAME}/src)
set(STB_INC_PATH ${STB_LIB_NAME}/include)
add_library(${STB_LIB_NAME} STATIC ${STB_SRC_PATH}/stb_image.cpp)
target_include_directories(${STB_LIB_NAME} PUBLIC ${STB_INC_PATH})

set(ASSIMP_LIB_NAME assimp)
set(ASSIMP_INC_PATH ${ASSIMP_LIB_NAME}/include)
add_subdirectory(${ASSIMP_LIB_NAME})

set(SPDLOG_LIB_NAME spdlog)
set(SPDLOG_SRC_PATH ${SPDLOG_LIB_NAME}/src)
set(SPDLOG_INC_PATH ${SPDLOG_LIB_NAME}/include)
add_library(${SPDLOG_LIB_NAME} STATIC ${SPDLOG_SRC_PATH}/spdlog.cpp)
target_include_directories(${SPDLOG_LIB_NAME} PUBLIC ${SPDLOG_INC_PATH})
target_compile_definitions(${SPDLOG_LIB_NAME} PRIVATE SPDLOG_COMPILED_LIB)

set(IMGUI_LIB_NAME imgui)
set(IMGUI_INC_PATH ${IMGUI_LIB_NAME})
set(IMGUI_SRC_PATH ${IMGUI_LIB_NAME})

set(IMGUI_HEADERS
    ${IMGUI_INC_PATH}/imconfig.h
    ${IMGUI_INC_PATH}/imgui.h
    ${IMGUI_INC_PATH}/imgui_internal.h
    ${IMGUI_INC_PATH}/imstb_rectpack.h
    ${IMGUI_INC_PATH}/imstb_textedit.h
    ${IMGUI_INC_PATH}/imstb_truetype.h

    ${IMGUI_INC_PATH}/backends/imgui_impl_glfw.h
    ${IMGUI_INC_PATH}/backends/imgui_impl_opengl3.h
    ${IMGUI_INC_PATH}/backends/imgui_impl_opengl3_loader.h
)

set(IMGUI_SOURCES
    ${IMGUI_SRC_PATH}/imgui.cpp
    ${IMGUI_SRC_PATH}/imgui_demo.cpp
    ${IMGUI_SRC_PATH}/imgui_draw.cpp
    ${IMGUI_SRC_PATH}/imgui_tables.cpp
    ${IMGUI_SRC_PATH}/imgui_widgets.cpp

    ${IMGUI_SRC_PATH}/backends/imgui_impl_glfw.cpp
    ${IMGUI_SRC_PATH}/backends/imgui_impl_opengl3.cpp
)

add_library(${IMGUI_LIB_NAME} STATIC ${IMGUI_HEADERS} ${IMGUI_SOURCES})
target_include_directories(${IMGUI_LIB_NAME} PUBLIC ${IMGUI_INC_PATH})
target_link_libraries(${IMGUI_LIB_NAME} PUBLIC ${GLFW_LIB_NAME})

set(IMGUIZMO_LIB_NAME imguizmo)
set(IMGUIZMO_SRC_PATH ${IMGUIZMO_LIB_NAME})
set(IMGUIZMO_INC_PATH ${IMGUIZMO_LIB_NAME})
add_library(${IMGUIZMO_LIB_NAME} STATIC ${IMGUIZMO_SRC_PATH}/ImGuizmo.cpp)
target_include_directories(${IMGUIZMO_LIB_NAME} PUBLIC ${IMGUIZMO_INC_PATH})
target_link_libraries(${IMGUIZMO_LIB_NAME} PUBLIC ${IMGUI_LIB_NAME})