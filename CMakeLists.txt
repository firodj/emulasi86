cmake_minimum_required(VERSION 3.20)
project(emulasi)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)

option("USE_VM" "Use hardware virtualization backend for Unicorn-Engine" OFF)

find_package(OpenGL REQUIRED)

set(GLM_DIR ext/glm)
set(IMGUI_DIR ext/imgui-docking)
set(VENDOR_DIR kosongg/vendor)
set(GLAD_SOURCES_DIR "${PROJECT_SOURCE_DIR}/ext/glad2")

set(SDL_TEST OFF)
set(SDL2_DISABLE_SDL2MAIN ON)
add_subdirectory(ext/sdl2)

add_subdirectory(ext/yaml-cpp)

set(UNICORN_BUILD_SAMPLES OFF)
set(UNICORN_ARCH "x86")
add_subdirectory(ext/unicorn)

add_subdirectory("${GLAD_SOURCES_DIR}/cmake" glad_cmake)
glad_add_library(glad_gl_core_33 REPRODUCIBLE API gl:core=3.3)

include_directories(SYSTEM
  ${OPENGL_INCLUDE_DIR}
  ${SDL2_SOURCE_DIR}/include
  glad/include
  ${IMGUI_DIR}
  ${IMGUI_DIR}/backends

  ${unicorn_SOURCE_DIR}/include
  ${YAML_CPP_SOURCE_DIR}/include
  ${GLM_DIR}
)

set(SOURCE
  ${VENDOR_DIR}/main.cpp
  ${VENDOR_DIR}/Engine.cpp
  ${VENDOR_DIR}/Component.cpp
  ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
  ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_demo.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp

  src/MainApp.cpp
  src/GameBase.cpp
)

SET(HEADERS
  ${VENDOR_DIR}/Component.h
  ${VENDOR_DIR}/Engine.h

  src/MainApp.h
  src/GameBase.h
)

add_executable(emulasi
  ${SOURCE} ${HEADERS}
)

target_compile_definitions(emulasi PUBLIC
  -D_CRT_SECURE_NO_WARNINGS
)

target_link_libraries(emulasi
  ${OPENGL_LIBRARIES}
  glad_gl_core_33
  SDL2-static
  yaml-cpp
  unicorn
)

set_target_properties(emulasi PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}"
  VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  DEBUG_POSTFIX "d"
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT emulasi)
