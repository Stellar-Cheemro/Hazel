add_executable(Sandbox
    src/Sandbox.cpp
)

# #include路径设置
# target_include_directories(Sandbox
#     PRIVATE
#         ${CMAKE_SOURCE_DIR}/Hazel/src
#         ${CMAKE_SOURCE_DIR}/Hazel
# )

# 预处理宏定义
target_compile_definitions(Sandbox
    PRIVATE
        HAZEL_PLATFORM_WINDOWS
        $<$<CONFIG:Debug>:HAZEL_DEBUG>
        $<$<CONFIG:Release>:HAZEL_RELEASE>
        $<$<CONFIG:RelWithDebInfo>:HAZEL_RELEASE>
)

# 链接Hazel库
target_link_libraries(Sandbox PRIVATE Hazel)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    target_compile_definitions(Sandbox PRIVATE HAZEL_ARCH_X64)
endif()

if(MSVC)
    target_compile_options(Sandbox PRIVATE /W4 /permissive- /EHsc)
else()
    target_compile_options(Sandbox PRIVATE -Wall -Wextra -Wpedantic)
endif()

# 构建后把 Hazel.dll 复制到 Sandbox.exe 同目录
if(WIN32)
    add_custom_command(TARGET Sandbox POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_RUNTIME_DLLS:Sandbox>
                $<TARGET_FILE_DIR:Sandbox>
        COMMAND_EXPAND_LISTS
    )
endif()

# 安装规则
include(GNUInstallDirs)

install(TARGETS Sandbox
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)