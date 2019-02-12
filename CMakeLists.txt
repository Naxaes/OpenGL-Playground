cmake_minimum_required(VERSION 3.12)
project(Next)

set(CMAKE_CXX_STANDARD 17)


# GLFW START
# http://www.glfw.org/docs/latest/build_guide.html
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")


add_executable(Next source/main.cpp)
target_include_directories(Next PRIVATE includes)


# GLFW
add_subdirectory(libraries/glfw-3.3.1)
target_compile_definitions(Next PRIVATE GLFW_INCLUDE_GLEXT)
target_include_directories(Next PRIVATE libraries/glfw-3.3.1/include)
target_compile_definitions(Next PRIVATE GLFW_INCLUDE_NONE)
target_link_libraries(Next glfw)
# GLAD
add_library(glad libraries/glad/src/glad.c)
target_include_directories(glad PRIVATE libraries/glad/include)
target_include_directories(Next PRIVATE libraries/glad/include)
target_link_libraries(Next glad ${CMAKE_DL_LIBS})
# STB
target_include_directories(Next PRIVATE libraries/stb)
# GLM
add_subdirectory(libraries/glm)
target_include_directories(Next PRIVATE libraries/glm/glm)
target_link_libraries(Next glm)
# ASSIMP
add_subdirectory(libraries/assimp-4.1.0)
target_link_libraries(Next assimp)
target_include_directories(Next PRIVATE libraries/assimp-4.1.0/include)
# IMGUI
target_sources(
        Next
        PRIVATE libraries/imgui/imgui.cpp
        PRIVATE libraries/imgui/imgui_demo.cpp
        PRIVATE libraries/imgui/imgui_draw.cpp
        PRIVATE libraries/imgui/implementation/imgui_impl_opengl3.cpp
        PRIVATE libraries/imgui/implementation/imgui_impl_glfw.cpp
)
target_include_directories(Next PRIVATE libraries/imgui)
target_include_directories(Next PRIVATE libraries/imgui/implementation/)


add_executable(Instanced source/instance.cpp)
target_include_directories(Instanced PRIVATE includes)

# GLFW
target_compile_definitions(Instanced PRIVATE GLFW_INCLUDE_GLEXT)
target_include_directories(Instanced PRIVATE libraries/glfw-3.3.1/include)
target_compile_definitions(Instanced PRIVATE GLFW_INCLUDE_NONE)
target_link_libraries(Instanced glfw)
# GLAD
target_include_directories(Instanced PRIVATE libraries/glad/include)
target_link_libraries(Instanced glad ${CMAKE_DL_LIBS})
# STB
target_include_directories(Instanced PRIVATE libraries/stb)
# GLM
target_include_directories(Instanced PRIVATE libraries/glm/glm)
target_link_libraries(Instanced glm)
# ASSIMP
target_link_libraries(Instanced assimp)
target_include_directories(Instanced PRIVATE libraries/assimp-4.1.0/include)





add_executable(Shadow source/shadow.cpp)
target_include_directories(Shadow PRIVATE includes)

# GLFW
target_compile_definitions(Shadow PRIVATE GLFW_INCLUDE_GLEXT)
target_include_directories(Shadow PRIVATE libraries/glfw-3.3.1/include)
target_compile_definitions(Shadow PRIVATE GLFW_INCLUDE_NONE)
target_link_libraries(Shadow glfw)
# GLAD
target_include_directories(Shadow PRIVATE libraries/glad/include)
target_link_libraries(Shadow glad ${CMAKE_DL_LIBS})
# STB
target_include_directories(Shadow PRIVATE libraries/stb)
# GLM
target_include_directories(Shadow PRIVATE libraries/glm/glm)
target_link_libraries(Shadow glm)
# ASSIMP
target_link_libraries(Shadow assimp)
target_include_directories(Shadow PRIVATE libraries/assimp-4.1.0/include)



add_executable(ECS source/ecs.cpp)
target_include_directories(ECS PRIVATE includes)

# GLFW
target_compile_definitions(ECS PRIVATE GLFW_INCLUDE_GLEXT)
target_include_directories(ECS PRIVATE libraries/glfw-3.3.1/include)
target_compile_definitions(ECS PRIVATE GLFW_INCLUDE_NONE)
target_link_libraries(ECS glfw)
# GLAD
target_include_directories(ECS PRIVATE libraries/glad/include)
target_link_libraries(ECS glad ${CMAKE_DL_LIBS})
# STB
target_include_directories(ECS PRIVATE libraries/stb)
# GLM
target_include_directories(ECS PRIVATE libraries/glm/glm)
target_link_libraries(ECS glm)
# ASSIMP
target_link_libraries(ECS assimp)
target_include_directories(ECS PRIVATE libraries/assimp-4.1.0/include)




add_executable(Game source/game.cpp)
target_include_directories(Game PRIVATE includes)

# GLFW
target_compile_definitions(Game PRIVATE GLFW_INCLUDE_GLEXT)
target_include_directories(Game PRIVATE libraries/glfw-3.3.1/include)
target_compile_definitions(Game PRIVATE GLFW_INCLUDE_NONE)
target_link_libraries(Game glfw)
# GLAD
target_include_directories(Game PRIVATE libraries/glad/include)
target_link_libraries(Game glad ${CMAKE_DL_LIBS})
# STB
target_include_directories(Game PRIVATE libraries/stb)
# GLM
target_include_directories(Game PRIVATE libraries/glm/glm)
target_link_libraries(Game glm)
# ASSIMP
target_link_libraries(Game assimp)
target_include_directories(Game PRIVATE libraries/assimp-4.1.0/include)

