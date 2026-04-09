add_library(Hazel SHARED
    src/Hazel/Application.cpp
    src/Hazel/Log.cpp
    src/Hazelpch.cpp
    src/Platform/Windows/WindowsWindow.cpp
    src/Hazel/LayerStack.cpp
    src/Hazel/Layer.cpp
    # src/Hazel/EntryPoint.h
)

# include路径设置
target_include_directories(Hazel
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog/include>
        $<INSTALL_INTERFACE:include>
)

# 预处理宏定义 
target_compile_definitions(Hazel
    PUBLIC    
        HAZEL_PLATFORM_WINDOWS
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
        imgui
        opengl32
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    target_compile_definitions(Hazel PRIVATE HAZEL_ARCH_X64)
endif()

if(MSVC)
    target_compile_options(Hazel PRIVATE /W4 /permissive- /EHsc)
else()
    target_compile_options(Hazel PRIVATE -Wall -Wextra -Wpedantic)
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