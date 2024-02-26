cmake_minimum_required(VERSION 3.20)
project(emulasi)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)
set(VENDOR_DIR kosongg/cpp)

if (APPLE)
  ## update the environment with VIRTUAL_ENV variable (mimic the activate script)
  set (ENV{VIRTUAL_ENV} "${CMAKE_SOURCE_DIR}/venv")
  ## change the context of the search
  set (Python3_FIND_VIRTUALENV FIRST)
  ## unset Python3_EXECUTABLE because it is also an input variable (see documentation, Artifacts Specification section)
  unset (Python3_EXECUTABLE)
endif()
find_package(OpenGL REQUIRED)
option("USE_VM" "Use hardware virtualization backend for Unicorn-Engine" OFF)
set("UNICORN_BUILD_SAMPLES" OFF)
set("UNICORN_ARCH" "x86")
add_subdirectory(ext/unicorn2)
set("SDL_TEST" OFF)
set("SDL2_DISABLE_SDL2MAIN" ON)
add_subdirectory(ext/sdl2)
set("IMGUI_DIR" "ext/imgui-docking")
set("GLAD_SOURCES_DIR" "${PROJECT_SOURCE_DIR}/ext/glad2")
add_subdirectory("${GLAD_SOURCES_DIR}/cmake" glad_cmake)
glad_add_library(glad_gl_core_33 REPRODUCIBLE API gl:core=3.3)
set("GLM_DIR" "ext/glm")

include_directories(SYSTEM
  ${OPENGL_INCLUDE_DIR}
  ${unicorn_SOURCE_DIR}/include
  ${SDL2_SOURCE_DIR}/include
  ${IMGUI_DIR}
  ${IMGUI_DIR}/backends
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
## -- sources --
  src/MainApp.cpp
  src/GameBase.cpp
  src/Shader.cpp
  src/Offscreen.cpp
  src/PlainBox.cpp
## -- endsources --
)

set(HEADERS
  ${VENDOR_DIR}/kosongg/Component.h
  ${VENDOR_DIR}/kosongg/Engine.h
  ${VENDOR_DIR}/kosongg/GLUtil.h
## -- headers --
  src/MainApp.h
  src/GameBase.h
  src/Shader.h
  src/Offscreen.h
  src/PlainBox.h
## -- endheaders --
)

include_directories(
  ${VENDOR_DIR}
  src
)

add_executable(emulasi
  ${SOURCE} ${HEADERS}
)

if (MSVC)
target_compile_definitions(emulasi PUBLIC
  -D_CRT_SECURE_NO_WARNINGS
)
endif()

target_link_libraries(emulasi
  ${OPENGL_LIBRARIES}
  unicorn
  SDL2-static
  glad_gl_core_33
)

set_target_properties(emulasi PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}"
  VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  DEBUG_POSTFIX "d"
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT emulasi)