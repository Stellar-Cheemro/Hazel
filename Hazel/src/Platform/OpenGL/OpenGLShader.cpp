#include "OpenGLShader.h"

#include <Hazel/Core/Log.h>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <string>
namespace Hazel
{
OpenGLShader::OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc)
{
    // 创建顶点着色器句柄
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // 将着色器源代码附加到着色器对象上
    const GLchar* source = static_cast<const GLchar*>(vertexSrc.c_str());
    glShaderSource(vertexShader, 1, &source, 0);

    // 编译着色器
    glCompileShader(vertexShader);

    GLint isCompiled = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

        glDeleteShader(vertexShader);

        // 输出编译错误日志
        HAZEL_CORE_ERROR("{0}", infoLog.data());
        HAZEL_CORE_ASSERT(false, "Vertex shader compilation failure!");
        return;
    }

    // 创建片段着色器句柄
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    source = static_cast<const GLchar*>(fragmentSrc.c_str());
    glShaderSource(fragmentShader, 1, &source, 0);

    // 编译片段着色器
    glCompileShader(fragmentShader);

    isCompiled = 0;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        // 输出编译错误日志
        HAZEL_CORE_ERROR("{0}", infoLog.data());
        HAZEL_CORE_ASSERT(false, "Fragment shader compilation failure!");
        return;
    }

    // 创建着色器程序对象
    m_RendererID = glCreateProgram();
    // 将编译好的着色器附加到程序对象上
    glAttachShader(m_RendererID, vertexShader);
    glAttachShader(m_RendererID, fragmentShader);
    // 链接着色器程序
    glLinkProgram(m_RendererID);

    // 链接错误检查
    GLint isLinked = 0;
    glGetProgramiv(m_RendererID, GL_LINK_STATUS, (int*)&isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(m_RendererID, maxLength, &maxLength, &infoLog[0]);

        glDeleteProgram(m_RendererID);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        // 输出链接错误日志
        HAZEL_CORE_ERROR("{0}", infoLog.data());
        HAZEL_CORE_ASSERT(false, "Shader program linking failure!");
        return;
    }

    // 链接成功后记得分离着色器对象
    glDetachShader(m_RendererID, vertexShader);
    glDetachShader(m_RendererID, fragmentShader);
}

OpenGLShader::~OpenGLShader()
{
    glDeleteProgram(m_RendererID);
}

void OpenGLShader::Bind() const
{
    glUseProgram(m_RendererID);
}

void OpenGLShader::Unbind() const
{
    glUseProgram(0);
}

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
} // namespace Hazel