#pragma once

#include "Hazel/Core.h"

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

// // 事件格式化输出 在glfw初始化WindowsWindow时爆雷，抛弃
// template <typename T>
//     requires std::derived_from<T, Event>
// std::string format_as(const T& e)
// {
//     return e.ToString();
// }

// inline std::ostream& operator<<(std::ostream& os, const Event& e)
// {
//     return os << e.ToString();
// }

} // namespace Hazel
/**
**@note 可以使用formatter格式化输出，但需要调用ToString函数 如HAZEL_CORE_TRACE("{}", e.ToString())
**/
// template <> struct fmt::formatter<Hazel::Event> : fmt::ostream_formatter
// {
// };
