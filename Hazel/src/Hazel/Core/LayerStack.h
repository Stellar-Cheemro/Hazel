#pragma once

#include <Hazel/Core/Core.h>
#include <Hazel/Core/Layer.h>

#include <vector>

namespace Hazel
{
class HAZEL_API LayerStack
{
public:
    LayerStack();
    // 当前 LayerStack 直接持有所有 Layer* 的生命周期
    // 因此析构时会统一 delete
    // 不要在外部再重复 delete
    ~LayerStack();

    void PushLayer(Layer* layer);
    void PushOverlay(Layer* overlay);
    void PopLayer(Layer* layer);
    void PopOverlay(Layer* overlay);

    std::vector<Layer*>::iterator begin()
    {
        return m_Layers.begin();
    }
    std::vector<Layer*>::iterator end()
    {
        return m_Layers.end();
    }

private:
    // [Layer][Overlay]
    std::vector<Layer*> m_Layers;
    unsigned int m_LayerInsertIndex = 0;
};
} // namespace Hazel