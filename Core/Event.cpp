#include "Core/Macro.hpp"
#include <Core/Event.hpp>

void EventDispatcher::RemoveListener(std::function<bool(uint32_t code, void *data)> listener)
{
    CHROME_TRACE_FUNCTION();
    for (size_t i = 0; i < mListeners.size(); i++)
    {
        if (mListeners[i].target<bool(uint32_t code, void *data)>() == listener.target<bool(uint32_t code, void *data)>())
        {
            mListeners.erase(mListeners.begin() + i);
        }
    }
}
void EventDispatcher::Dispatch(int code, void *data)
{
    for (size_t i = 0; i < mListeners.size(); i++)
    {
        if (mListeners[i](code, data) == true)
        {
            return;
        }
    }
}

EventDispatcher::~EventDispatcher()
{
    mListeners.clear();
}
EventDispatcher::EventDispatcher(EventDispatcher &&eventDispatcher) noexcept
{
    for (int i = 0; i < eventDispatcher.mListeners.size(); i++)
    {
        mListeners.emplace_back(std::move(eventDispatcher.mListeners[i]));
    }

    eventDispatcher.mListeners.clear();
}
EventDispatcher &EventDispatcher::operator=(EventDispatcher &&eventDispatcher) noexcept
{
    for (int i = 0; i < eventDispatcher.mListeners.size(); i++)
    {
        mListeners.emplace_back(std::move(eventDispatcher.mListeners[i]));
    }

    eventDispatcher.mListeners.clear();
    return *this;
}
void EventDispatcher::AddListener(const std::function<bool(uint32_t code, void *data)> &listener)
{
    mListeners.push_back(listener);
}
