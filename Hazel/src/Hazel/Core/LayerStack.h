#pragma once

#include <Hazel/Core/Core.h>
#include <Hazel/Core/Layer.h>
#include <Hazel/Core/Scope.h>

#include <concepts>
#include <cstdint>
#include <vector>
namespace Hazel
{
template <typename T>
concept LayerType = std::derived_from<T, Layer>;

class HAZEL_API LayerStack
{
public:
    // LayerStack持有所有Layer的所有权
    LayerStack();
    ~LayerStack();

    template <LayerType T, typename... Args> T& PushLayer(Args&&... args)
    {
        auto layer = std::make_unique<T>(std::forward<Args>(args)...);
        T& ref = *layer;
        m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, std::move(layer));
        m_LayerInsertIndex++;
        ref.OnAttach();
        return ref;
    }

    template <LayerType T, typename... Args> T& PushOverlay(Args&&... args)
    {
        auto overlay = std::make_unique<T>(std::forward<Args>(args)...);
        T& ref = *overlay;
        m_Layers.emplace_back(std::move(overlay));
        ref.OnAttach();
        return ref;
    }

    void PopLayer(Layer* layer);
    void PopOverlay(Layer* overlay);

    std::vector<Scope<Layer>>::iterator begin()
    {
        return m_Layers.begin();
    }
    std::vector<Scope<Layer>>::iterator end()
    {
        return m_Layers.end();
    }

    LayerStack(const LayerStack&) = delete;
    LayerStack& operator=(const LayerStack&) = delete;

    LayerStack(const LayerStack&&) = delete;
    LayerStack& operator=(const LayerStack&&) = delete;

private:
    // [Layer][Overlay]
    std::vector<Scope<Layer>> m_Layers;
    uint32_t m_LayerInsertIndex = 0;
};
} // namespace Hazel