cmake_minimum_required(VERSION 3.20)
project(emulasi)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)
option("USE_VM" "Use hardware virtualization backend for Unicorn-Engine" OFF)
find_package(OpenGL REQUIRED)

set(SDL2_DIR ext/sdl2)
set(IMGUI_DIR ext/imgui-docking)
set(VENDOR_DIR kosongg/vendor)

set(SDL_TEST OFF)
set(SDL2_DISABLE_SDL2MAIN ON)
add_subdirectory(${SDL2_DIR})

include_directories(SYSTEM
  ${OPENGL_INCLUDE_DIR}
  ${SDL2_DIR}/include
  glad/include
  ${IMGUI_DIR}
  ${IMGUI_DIR}/backends
)

set(SOURCE
  ${VENDOR_DIR}/main.cpp
  ${VENDOR_DIR}/Engine.cpp
  ${VENDOR_DIR}/Component.cpp
  ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer2.cpp
  ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_demo.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp

  src/MainApp.cpp
)

SET(HEADERS
  ${VENDOR_DIR}/Component.h
  ${VENDOR_DIR}/Engine.h

  src/MainApp.h
)

add_executable(emulasi
  ${SOURCE} ${HEADERS}
)

target_compile_definitions(emulasi PUBLIC
  -D_CRT_SECURE_NO_WARNINGS
)

target_link_libraries(emulasi
  ${OPENGL_LIBRARIES}
  SDL2-static
)

set_target_properties(emulasi PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}"
  VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  DEBUG_POSTFIX "d"
)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT emulasi)
