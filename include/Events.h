#pragma once

namespace Events
{
    class InputEventHandler : public EventSingleton<InputEventHandler, RE::InputEvent*>
    {
    public:
        RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>* a_eventSource) noexcept override;
    };
} // namespace Events
