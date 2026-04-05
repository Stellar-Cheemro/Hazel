cmake_minimum_required(VERSION 3.21)

project(HazelEngine VERSION 1.0.0 LANGUAGES CXX)

# --------------------------------
# 1. 全局 C++ 设置
# --------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel" CACHE STRING "" FORCE)
endif()

# --------------------------------
# 2. MSVC 运行时库
# --------------------------------
option(HAZEL_USE_STATIC_MSVC_RUNTIME "Use /MT /MTd instead of /MD /MDd" OFF)

if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()

if(MSVC)
    if(HAZEL_USE_STATIC_MSVC_RUNTIME)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    else()
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    endif()
endif()

# --------------------------------
# 3. 统一输出目录
# --------------------------------
set(HAZEL_OUT_ROOT "${CMAKE_SOURCE_DIR}/out")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${HAZEL_OUT_ROOT}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${HAZEL_OUT_ROOT}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${HAZEL_OUT_ROOT}/lib")

if(MSVC)
    set(CMAKE_PDB_OUTPUT_DIRECTORY "${HAZEL_OUT_ROOT}/pdb")
    set(CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY "${HAZEL_OUT_ROOT}/pdb")
endif()

# --------------------------------
# 4. 子项目
# --------------------------------
add_subdirectory(Hazel)
add_subdirectory(Sandbox)

# --------------------------------
# 5. VS 默认启动项目
# --------------------------------
if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Sandbox)
endif()