cmake_minimum_required(VERSION 3.15)
project(FingerprintDemo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_GENERATOR "Ninja" CACHE INTERNAL "")

# Raylib setup
set(CMAKE_PREFIX_PATH "C:/msys64/ucrt64")
find_package(raylib REQUIRED)

# ✅ Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include        # for libzkfp headers
    ${CMAKE_SOURCE_DIR}/libs/x64       # for libzkfp import libs / headers
    ${raylib_INCLUDE_DIRS}
)

# ✅ Source files
add_executable(fingerprint_demo
    src/main.cpp
    src/FingerprintDevice.cpp
    src/GuiDemo.cpp
)

# ✅ Link Raylib, libzkfp, and Windows system libs
target_link_libraries(fingerprint_demo
    raylib
    ${CMAKE_SOURCE_DIR}/libs/x64/libzkfp.dll.a   # link the import library
    winmm
    gdi32
    opengl32
    user32
    shell32
    ole32
    oleaut32
    uuid
    comdlg32
    advapi32
)

# ✅ Copy the fingerprint SDK DLL beside the final .exe
add_custom_command(TARGET fingerprint_demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/libs/x64/libzkfp.dll"
    $<TARGET_FILE_DIR:fingerprint_demo>
)
