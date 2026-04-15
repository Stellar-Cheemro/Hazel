#pragma once

#include <Hazel/Renderer/GraphicsContext.h>

class GLFWwindow;

namespace Hazel
{
class OpenGLContext : public GraphicsContext
{
public:
    OpenGLContext(void* windowHandle);

    virtual void Init() override;
    virtual void SwapBuffers() override;

private:
    GLFWwindow* m_WindowHandle;
};
} // namespace Hazel