// clang-format off
#include "OpenGLShader.h"

#include <Hazel/Core/Log.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
// clang-format on

namespace Hazel
{
namespace
{
GLenum ShaderTypeFromString(const std::string& type)
{
    if (type == "vertex")
        return GL_VERTEX_SHADER;
    if (type == "fragment" || type == "pixel")
        return GL_FRAGMENT_SHADER;

    HAZEL_CORE_ASSERT(false, "Unknown shader type while pre-processing shader source code.");
    return 0;
}

GLuint CompileShaderStage(GLenum type, const std::string& source)
{
    GLuint shader = glCreateShader(type);
    const GLchar* sourceCStr = static_cast<const GLchar*>(source.c_str());
    glShaderSource(shader, 1, &sourceCStr, 0);

    glCompileShader(shader);
    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

        glDeleteShader(shader);

        // 输出编译错误日志
        HAZEL_CORE_ERROR("{0}", infoLog.data());
        HAZEL_CORE_ASSERT(false, "{0} Shader compilation failure!",
                          (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment"));
        return 0;
    }
    return shader;
}
} // namespace
// ----------------------------------------------------------------------------
// 构造/析构函数
// ----------------------------------------------------------------------------
OpenGLShader::OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc)
    : m_RendererID(0)
{
    std::unordered_map<GLenum, std::string> shaderSources;
    shaderSources[GL_VERTEX_SHADER] = vertexSrc;
    shaderSources[GL_FRAGMENT_SHADER] = fragmentSrc;
    Compile(shaderSources);
}

OpenGLShader::OpenGLShader(const std::filesystem::path& filepath) : m_RendererID(0)
{
    std::string shaderStr = ReadFile(filepath);
    auto shaderSources = PreProcess(shaderStr);
    Compile(shaderSources);
}

OpenGLShader::~OpenGLShader()
{
    glDeleteProgram(m_RendererID);
}
// ----------------------------------------------------------------------------
// Public API
// ----------------------------------------------------------------------------

void OpenGLShader::Bind() const
{
    glUseProgram(m_RendererID);
}

void OpenGLShader::Unbind() const
{
    glUseProgram(0);
}

// ----------------------------------------------------------------------------
// Uniform上传接口
// ----------------------------------------------------------------------------
void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& vector)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
}

void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& vector)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform3f(location, vector.x, vector.y, vector.z);
}

void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& vector)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform2f(location, vector.x, vector.y);
}

void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform1f(location, value);
}

void OpenGLShader::UploadUniformInt(const std::string& name, int value)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform1i(location, value);
}

void OpenGLShader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform1iv(location, count, values);
}
// ----------------------------------------------------------------------------
// 内部工具函数
// ----------------------------------------------------------------------------
std::string OpenGLShader::ReadFile(const std::filesystem::path& filepath)
{
    std::ifstream in(filepath, std::ios::in | std::ios::binary);
    if (!in)
    {
        HAZEL_CORE_ERROR("Could not open file '{0}'", filepath.string());
        HAZEL_CORE_ASSERT(false, "Failed to open Shader file!");
        return std::string();
    }

    in.seekg(0, std::ios::end);
    size_t size = in.tellg();
    in.seekg(0, std::ios::beg);
    std::string result;
    result.resize(size);
    in.read(&result[0], size);
    in.close();
    return result;
}

std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
{
    std::unordered_map<GLenum, std::string> shaderSources;
    const char* typeToken = "#type";
    size_t typeTokenLength = strlen(typeToken);
    size_t pos = source.find(typeToken, 0);

    while (pos != std::string::npos)
    {
        // 当前 "#type xxx"行的结尾
        size_t eol = source.find_first_of("\r\n", pos);
        HAZEL_CORE_ASSERT(eol != std::string::npos,
                          "ShaderSyntax error: Missing end of line after shader type declaration!");

        // 提取着色器类型如"vertex""/"fragment""
        size_t begin = pos + typeTokenLength + 1;
        std::string type = source.substr(begin, eol - begin);
        HAZEL_CORE_ASSERT(type == "vertex" || type == "fragment", "Invalid shader type specified");

        // 跳过空行，定位到真正的着色器代码的开始位置
        size_t nextLinePos = source.find_first_not_of("\r\n", eol);
        // 寻找下一个 "#type"标记
        pos = source.find(typeToken, nextLinePos);
        // 提取当前着色器代码块，如果找不到下一个 "#type"标记，则一直提取到文件末尾
        shaderSources[ShaderTypeFromString(type)] =
            (pos == std::string::npos) ? source.substr(nextLinePos)
                                       : source.substr(nextLinePos, pos - nextLinePos);
    }
    return shaderSources;
}

void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
{
    GLuint program = glCreateProgram();
    std::vector<GLuint> shaderIDs;
    shaderIDs.reserve(shaderSources.size());
    for (auto& [type, source] : shaderSources)
    {
        GLuint shader = CompileShaderStage(type, source);
        if (shader == 0)
        {
            glDeleteProgram(program);
            break;
        }
        shaderIDs.push_back(shader);
        glAttachShader(program, shader);
        shaderIDs.push_back(shader);
    }

    // 链接着色器程序
    glLinkProgram(program);

    // 链接错误检查
    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

        glDeleteProgram(program);
        for (auto id : shaderIDs)
        {
            glDeleteShader(id);
        }
        HAZEL_CORE_ERROR("{0}", infoLog.data());
        HAZEL_CORE_ASSERT(false, "Shader program linking failure!");
        return;
    }

    for (auto id : shaderIDs)
    {
        glDetachShader(program, id);
        glDeleteShader(id);
    }
    m_RendererID = program;
}
} // namespace Hazel