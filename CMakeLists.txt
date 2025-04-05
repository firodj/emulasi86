cmake_minimum_required(VERSION 3.20)
project(emulasi86)

set(EXT_PATH "../ext" CACHE PATH "Ext folder")
if (IS_ABSOLUTE ${EXT_PATH})
  file(RELATIVE_PATH EXT_PATH ${PROJECT_SOURCE_DIR} ${EXT_PATH})
endif()
message(STATUS "Relative EXT_PATH = ${EXT_PATH}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)
set(VENDOR_DIR kosongg/cpp)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
option(BUILD_APPLE_BUNDLE "Build Apple Bundle" OFF)
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin")

if (APPLE)
  ## update the environment with VIRTUAL_ENV variable (mimic the activate script)
  set (ENV{VIRTUAL_ENV} "${CMAKE_SOURCE_DIR}/venv")
  ## change the context of the search
  set (Python3_FIND_VIRTUALENV FIRST)
  ## unset Python3_EXECUTABLE because it is also an input variable (see documentation, Artifacts Specification section)
  unset (Python3_EXECUTABLE)
endif()
find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)
set(SDL_TEST OFF)
set(SDL2_DISABLE_SDL2MAIN ON)
add_subdirectory("${EXT_PATH}/sdl2" "ext/sdl2")
add_subdirectory("kosongg/cpp/imgui")
add_subdirectory("${EXT_PATH}/glad2/cmake" glad_cmake)
glad_add_library(glad_gl_core_33 REPRODUCIBLE API gl:core=3.3)
set(GLM_DIR "${EXT_PATH}/glm")
add_subdirectory("${EXT_PATH}/fmt" "ext/fmt")
add_subdirectory("${EXT_PATH}/pe-parse/pe-parser-library" "ext/pe-parse")
add_subdirectory("${EXT_PATH}/googletest" "ext/googletest")
add_subdirectory("${EXT_PATH}/libassert" "ext/libassert")
set(HSCPP_BUILD_EXAMPLES OFF)
set(HSCPP_BUILD_TESTS OFF)
add_subdirectory("${EXT_PATH}/hscpp" "ext/hscpp")

if (IS_ABSOLUTE ${IMGUI_DIR})
  file(RELATIVE_PATH IMGUI_DIR ${PROJECT_SOURCE_DIR} ${IMGUI_DIR})
endif()
message(STATUS "Relative IMGUI_DIR = ${IMGUI_DIR}")

include_directories(SYSTEM
  ${OPENGL_INCLUDE_DIR}
  ${GLM_DIR}
  ${OPENAL_INCLUDE_DIR}
  ${fmt_SOURCE_DIR}/include
)

set(SOURCE
  ${VENDOR_DIR}/main.cpp
  ${VENDOR_DIR}/Engine.cpp
  ${VENDOR_DIR}/GLUtil.cpp
  ${VENDOR_DIR}/UtfConv.c
  ${VENDOR_DIR}/imkosongg/ImKosongg.cpp
  ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
  ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
## -- sources --
  src/MainApp.cpp
  src/GameBase.cpp
  src/Shader.cpp
  src/Offscreen.cpp
  src/PlainBox.cpp
## -- endsources --
)

set(HEADERS
  ${VENDOR_DIR}/kosongg/Engine.h
  ${VENDOR_DIR}/kosongg/GLUtil.h
  ${VENDOR_DIR}/kosongg/UtfConv.h
  ${VENDOR_DIR}/imkosongg/ImKosongg.hpp
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

if (BUILD_APPLE_BUNDLE)
set(RESOURCE_FILES "")
set(ICON_NAME "emulasi86.icns")
set(ICON_PATH ${PROJECT_SOURCE_DIR}/${ICON_NAME})
set_source_files_properties(${PROJECT_SOURCE_DIR}/${ICON_NAME} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
set_source_files_properties(${PROJECT_SOURCE_DIR}/kosongg/fonts/selawkl.ttf PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/fonts")
set_source_files_properties(${PROJECT_SOURCE_DIR}/kosongg/fonts/fa-solid-900.ttf PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/fonts")

list(APPEND SOURCE
  ${ICON_PATH}
  ${PROJECT_SOURCE_DIR}/kosongg/fonts/selawkl.ttf
  ${PROJECT_SOURCE_DIR}/kosongg/fonts/fa-solid-900.ttf
  ${VENDOR_DIR}/Utils.mm
)
add_executable(emulasi86 MACOSX_BUNDLE ${SOURCE} ${HEADERS})
set_target_properties(emulasi86 PROPERTIES
  MACOSX_BUNDLE_NAME                             "emulasi86"
  MACOSX_BUNDLE_GUI_IDENTIFIER                   "org.firodj.emulasi86"
  MACOSX_BUNDLE_ICON_FILE                        ${ICON_NAME}
  MACOSX_BUNDLE_INFO_PLIST                       "${CMAKE_SOURCE_DIR}/Info.plist.in"
  MACOSX_BUNDLE                                  "TRUE"
)
list(APPEND _COMPILE_DEFINITIONS
  BUILD_APPLE_BUNDLE
)
else()
add_executable(emulasi86
  ${SOURCE} ${HEADERS}
)
endif()

foreach(_source IN ITEMS ${SOURCE})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
    source_group("${_source_path_msvc}" FILES "${_source}")
endforeach()

foreach(_source IN ITEMS ${HEADERS})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
    source_group("${_source_path_msvc}" FILES "${_source}")
endforeach()

if (MSVC)

  # Silence MSVC CRT security warnings
  list(APPEND _COMPILE_DEFINITIONS
    _CRT_SECURE_NO_WARNINGS

  )

  if (USE_ASAN)
    target_compile_options(emulasi86 PRIVATE /fsanitize=address)
  endif()
else()
  target_compile_options(emulasi86 PRIVATE -Wno-deprecated)
  if (USE_ASAN)
    target_compile_options(emulasi86 PRIVATE -fsanitize=address)
    target_link_options(emulasi86 PRIVATE -fsanitize=address)
  endif()
  if (APPLE)
    if (CMAKE_GENERATOR STREQUAL Xcode)
      set_target_properties(emulasi86 PROPERTIES XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf-with-dsym")
    else()
      add_custom_command(
        TARGET emulasi86
        POST_BUILD
        COMMAND dsymutil $<TARGET_FILE:emulasi86>
      )
    endif()
  endif()
endif()

list(APPEND _COMPILE_DEFINITIONS
  _PROJECT_SRC_PATH_="${CMAKE_SOURCE_DIR}"
  SDL_MAIN_HANDLED
  _USE_HSCPP_
)

add_compile_definitions(${_COMPILE_DEFINITIONS})

target_link_libraries(emulasi86
  SDL2
  ${OPENGL_LIBRARIES}
  imgui
  glad_gl_core_33
  ${OPENAL_LIBRARY}
  fmt
  pe-parse::pe-parse
  libassert::assert
  hscpp
  hscpp-mem
)

if (APPLE)
  # Cocoa
  find_library(COCOA_LIBRARY Cocoa)
  find_library(APPKIT_LIB AppKit)
  target_link_libraries(emulasi86 ${COCOA_LIBRARY} ${APPKIT_LIB})
endif()

if (BUILD_APPLE_BUNDLE)

else()
set_target_properties(emulasi86 PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin"
  VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
  DEBUG_POSTFIX "d"
)
endif()

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT emulasi86)

## -- others --
set(WINAPIEMU_SORCES
#  src/emulasi/core/com/a3d.cpp
#  src/emulasi/core/com/d3d.cpp
#  src/emulasi/core/com/ddraw.cpp
#  src/emulasi/core/com/dinput.cpp
#  src/emulasi/core/com/dplay.cpp

  src/emulasi/core/dll/advapi32.cpp
  src/emulasi/core/dll/advapi32_impl.cpp
#  src/emulasi/core/dll/gdi32.cpp
#  src/emulasi/core/dll/kernel32.cpp
#  src/emulasi/core/dll/ole32.cpp
#  src/emulasi/core/dll/smush.cpp
#  src/emulasi/core/dll/user32.cpp
#  src/emulasi/core/dll/winmm.cpp
#  src/emulasi/core/dll/version.cpp
#  src/emulasi/core/dll/comctl32.cpp
#  src/emulasi/core/dll/ole32.cpp
)
SET(WINAPIEMU_HEADERS
#  src/emulasi/core/com/a3d.hpp
#  src/emulasi/core/com/d3d.hpp
#  src/emulasi/core/com/ddraw.hpp
#  src/emulasi/core/com/dinput.hpp
#  src/emulasi/core/com/dplay.hpp

  src/emulasi/core/dll/advapi32.hpp
  src/emulasi/core/dll/advapi32_impl.hpp
#  src/emulasi/core/dll/gdi32.hpp
#  src/emulasi/core/dll/kernel32.hpp
#  src/emulasi/core/dll/user32.hpp
#  src/emulasi/core/dll/winmm.hpp
#  src/emulasi/core/dll/version.hpp
#  src/emulasi/core/dll/comctl32.hpp
#  src/emulasi/core/dll/ole32.hpp
)
add_library(winapiemu STATIC
  ${WINAPIEMU_SORCES} ${WINAPIEMU_HEADERS}
)
target_link_libraries(winapiemu
  fmt glad_gl_core_33 SDL2 libassert::assert
  ${OPENGL_LIBRARY}
  ${OPENAL_LIBRARY}
)

enable_testing()
# Now simply link against gtest or gtest_main as needed. Eg

add_executable(winapiemu_test
  src/emulasi/core/dll/advapi32_test.cpp
  src/emulasi/core/blockalloc_test.cpp

  src/emulasi/core/blockalloc.cpp
)

target_link_libraries(winapiemu_test winapiemu
  gtest_main
  fmt
  libassert::assert
)

add_test(NAME winapiemu_test COMMAND winapiemu_test)
## -- endothers --