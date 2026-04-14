#pragma once

#include <Hazel/Core/Core.h>

#include <functional>
#include <sstream>
#include <string>
namespace Hazel
{
enum class EventType
{
    // clang-format off
    None = 0,
    WindowClose,WindowResize,WindowFocus,WindowLostFocus,WindowMoved,
    AppTick,AppUpdate,AppRender,
    KeyPressed,KeyReleased,KeyTyped,
    MouseButtonPressed,MouseButtonReleased,MouseMoved,MouseScrolled
    // clang-format on
};
enum EventCategory
{
    None = 0,
    EventCategoryApplication = BIT(0),
    EventCategoryInput = BIT(1),
    EventCategoryKeyboard = BIT(2),
    EventCategoryMouse = BIT(3),
    EventCategoryMouseButton = BIT(4)
};

#define EVENT_CLASS_TYPE(type)                                                                     \
    static EventType GetStaticType()                                                               \
    {                                                                                              \
        return EventType::type;                                                                    \
    }                                                                                              \
    virtual EventType GetEventType() const override                                                \
    {                                                                                              \
        return GetStaticType();                                                                    \
    }                                                                                              \
    virtual const char* GetName() const override                                                   \
    {                                                                                              \
        return #type;                                                                              \
    }

#define EVENT_CLASS_CATEGORY(category)                                                             \
    virtual int GetCategoryFlags() const override                                                  \
    {                                                                                              \
        return category;                                                                           \
    }

class HAZEL_API Event
{
    friend class EventDispatcher;

public:
    virtual EventType GetEventType() const = 0;
    virtual const char* GetName() const = 0;
    virtual int GetCategoryFlags() const = 0;
    virtual std::string ToString() const
    {
        return GetName();
    }

    inline bool IsInCategory(EventCategory category)
    {
        return GetCategoryFlags() & category;
    }

    inline bool IsHandled() const
    {
        return m_Handled;
    }

protected:
    bool m_Handled = false;
};

class EventDispatcher
{
    template <typename T> using EventFn = std::function<bool(T&)>;

public:
    EventDispatcher(Event& event) : m_Event(event)
    {
    }
    template <typename T> bool Dispatch(EventFn<T> func)
    {
        if (m_Event.GetEventType() == T::GetStaticType())
        {
            m_Event.m_Handled = func(static_cast<T&>(m_Event));
            return true;
        }
        return false;
    }

private:
    Event& m_Event;
};

} // namespace Hazel
/**
 * @note 最新版spdlog使用formatter来格式化日志输出，
 *       默认情况下spdlog无法直接格式化Hazel::Event及其派生类，
 *       因此需要为它们提供一个统一的formatter。
 *       该formatter位于Hazel/src/Hazel/SpdlogFormatters/EventFormatters.h中，
 *       通过调用Event的ToString方法来获取事件的字符串表示。
 */