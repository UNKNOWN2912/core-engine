#include <Core/Event.hpp>

void EventDispatcher::RemoveListener(std::function<bool(uint32_t code, void* data)> listener)
{
    for (size_t i = 0; i < mListeners.size(); i++)
    {
        if(mListeners[i].target<bool(uint32_t code, void* data)>() == listener.target<bool(uint32_t code, void* data)>())
        {
            mListeners.erase(mListeners.begin() + i);
        }
    }
}
