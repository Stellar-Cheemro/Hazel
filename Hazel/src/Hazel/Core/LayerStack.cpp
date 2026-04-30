#include "LayerStack.h"

namespace Hazel
{
// ----------------------------------------------------------------------------
// 构造/析构函数
// ----------------------------------------------------------------------------
LayerStack::LayerStack()
{
}
LayerStack::~LayerStack()
{
    for (const auto& layer : m_Layers)
    {
        if (layer)
        {
            layer->OnDetach();
        }
    }
}
// ----------------------------------------------------------------------------
// PUBLIC API
// ----------------------------------------------------------------------------
void LayerStack::PopLayer(Layer* layer)
{
    auto begin = m_Layers.begin();
    auto end = m_Layers.begin() + m_LayerInsertIndex;
    auto it = std::find_if(begin, end,
                           [layer](const Scope<Layer>& current) { return current.get() == layer; });
    if (it != end)
    {
        (*it)->OnDetach();
        m_Layers.erase(it);
        m_LayerInsertIndex--;
    }
}

void LayerStack::PopOverlay(Layer* overlay)
{
    auto begin = m_Layers.begin() + m_LayerInsertIndex;
    auto end = m_Layers.end();
    auto it = std::find_if(begin, end, [overlay](const Scope<Layer>& current)
                           { return current.get() == overlay; });
    if (it != end)
    {
        (*it)->OnDetach();
        m_Layers.erase(it);
    }
}
} // namespace Hazel