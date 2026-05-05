# HAZEL本体各库路径
set(HAZEL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/Hazel)
set(PLATFORM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/Platform)
# 第三方库路径宏设置
set(GLM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glm)
set(SPDLOG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog)
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui)
set(STB_IMAGE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb_image)

# 源文件列表
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
)
set(HAZEL_SOURCES
    ## --------------------------------------
    ## HAZEL
    ## --------------------------------------
    ## --------------------------------------
    ## Core
    ## --------------------------------------
    ${HAZEL_DIR}/Core/Application.cpp
    ${HAZEL_DIR}/Core/Log.cpp
    ${HAZEL_DIR}/Core/LayerStack.cpp
    ${HAZEL_DIR}/Core/Layer.cpp
    ${HAZEL_DIR}/Core/Timestep.cpp
    ## --------------------------------------
    ## Controllers
    ## --------------------------------------
    ${HAZEL_DIR}/Controllers/OrthographicCameraController.cpp
    ## --------------------------------------
    ## ImGui
    ## --------------------------------------
    ${HAZEL_DIR}/ImGui/ImGuiLayer.cpp
    ${HAZEL_DIR}/ImGui/ImGuiBuild.cpp
    ## --------------------------------------
    ## Renderer
    ## --------------------------------------
    ${HAZEL_DIR}/Renderer/OrthographicCamera.cpp
    ${HAZEL_DIR}/Renderer/Buffer.cpp
    ${HAZEL_DIR}/Renderer/Shader.cpp
    ${HAZEL_DIR}/Renderer/GraphicsContext.cpp
    ${HAZEL_DIR}/Renderer/SceneRenderer.cpp
    ${HAZEL_DIR}/Renderer/VertexArray.cpp
    ${HAZEL_DIR}/Renderer/RendererAPI.cpp
    ${HAZEL_DIR}/Renderer/RenderCommand.cpp
    ${HAZEL_DIR}/Renderer/Renderer2D.cpp
    ${HAZEL_DIR}/Renderer/Texture.cpp
    ## --------------------------------------
    ## Project
    ## --------------------------------------
    ${HAZEL_DIR}/Project/Project.cpp
    ## --------------------------------------
    ## Asset
    ## --------------------------------------
    ${HAZEL_DIR}/Asset/AssetManager.cpp
    ${HAZEL_DIR}/Asset/AssetRegistry.cpp
    ${HAZEL_DIR}/Asset/Serialization/TextureSerializer.cpp
    ${HAZEL_DIR}/Asset/Serialization/ShaderSerializer.cpp
    ${HAZEL_DIR}/Asset/UserAssetManager.cpp
    ${HAZEL_DIR}/Asset/AssetPath.cpp
    ${HAZEL_DIR}/Asset/Internal/AssetHandleAllocator.cpp
    ${HAZEL_DIR}/Asset/Internal/AssetRuntimeCache.cpp
    ${HAZEL_DIR}/Asset/Internal/AssetSerializerRegistry.cpp
    ${HAZEL_DIR}/Asset/Internal/AssetSystemFileResolver.cpp
    ${HAZEL_DIR}/Asset/Internal/AssetRootLocator.cpp
    ## --------------------------------------
    ## PLATFORM
    ## --------------------------------------
    ## --------------------------------------
    ## Windows
    ## --------------------------------------
    ${PLATFORM_DIR}/Windows/WindowsWindow.cpp
    ${PLATFORM_DIR}/Windows/WindowsInput.cpp
    ## --------------------------------------
    ## OpenGL
    ## --------------------------------------
    ${PLATFORM_DIR}/OpenGL/OpenGLBuffer.cpp
    ${PLATFORM_DIR}/OpenGL/OpenGLContext.cpp
    ${PLATFORM_DIR}/OpenGL/OpenGLRendererAPI.cpp
    ${PLATFORM_DIR}/OpenGL/OpenGLVertexArray.cpp
    ${PLATFORM_DIR}/OpenGL/OpenGLShader.cpp
    ${PLATFORM_DIR}/OpenGL/OpenGLTexture.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/Hazelpch.cpp
)
set(STB_IMAGE_SOURCES
    ${STB_IMAGE_DIR}/stb_image.cpp
)
# 添加Hazel库
add_library(Hazel SHARED
    ${HAZEL_SOURCES}
    ${IMGUI_SOURCES}
    ${STB_IMAGE_SOURCES}
    src/Hazel/Asset/Internal/AssetRootLocator.cpp
)

# include路径设置
target_include_directories(Hazel
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${SPDLOG_DIR}/include>
        $<BUILD_INTERFACE:${GLM_DIR}>
        $<BUILD_INTERFACE:${IMGUI_DIR}>
        $<BUILD_INTERFACE:${STB_IMAGE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE
        $<BUILD_INTERFACE:${IMGUI_DIR}/backends>
)

# 预处理宏定义 
target_compile_definitions(Hazel
    PUBLIC    
        HAZEL_PLATFORM_WINDOWS
        IMGUI_USER_CONFIG="${HAZEL_DIR}/ImGui/ImGuiConfig.h"
    PRIVATE
        HAZEL_BUILD_DLL
        $<$<CONFIG:Debug>:HAZEL_DEBUG>
        $<$<CONFIG:Release>:HAZEL_RELEASE>
        $<$<CONFIG:RelWithDebInfo>:HAZEL_RELEASE>
)

# 预编译头文件
target_precompile_headers(Hazel 
    PRIVATE
        "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/src/Hazelpch.h>"
)

# 链接第三方库
target_link_libraries(Hazel
    PRIVATE
        glad
        glfw
        opengl32
        imgui
        glm::glm
)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    target_compile_definitions(Hazel PRIVATE HAZEL_ARCH_X64)
endif()

if(MSVC)
    target_compile_options(Hazel PRIVATE /W4 /permissive- /EHsc)
else()
    target_compile_options(Hazel PRIVATE -Wall -Wextra -Wpedantic)
# glm SIMD优化
target_compile_definitions(Hazel PUBLIC
    $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:GLM_FORCE_INTRINSICS>
    $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:GLM_FORCE_INLINE>
)
target_compile_options(Hazel PRIVATE
    $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:/arch:AVX2>
)
endif()

if(WIN32 AND MSVC)
    set_target_properties(Hazel PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS OFF
    )
endif()


# 安装规则
include(GNUInstallDirs)

install(TARGETS Hazel
    EXPORT HazelTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/Hazel/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Hazel
)

install(EXPORT HazelTargets
    FILE HazelTargets.cmake
    NAMESPACE Hazel::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Hazel
)