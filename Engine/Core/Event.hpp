#pragma once
#include <cstdint>
#include <functional>
#include <vector>

#define BindMember(callback) std::bind(&callback, this, std::placeholders::_1, std::placeholders::_2)
#define BindFunction(callback) std::bind(&callback, std::placeholders::_1, std::placeholders::_2)

class EventDispatcher
{
public:
    EventDispatcher() = default;
    EventDispatcher(EventDispatcher &&eventDispatcher)
    {
        for (int i = 0; i < eventDispatcher.mListeners.size(); i++)
        {
            mListeners.emplace_back(std::move(eventDispatcher.mListeners[i]));
        }

        eventDispatcher.mListeners.clear();
    }
    EventDispatcher &operator=(EventDispatcher &&eventDispatcher) noexcept
    {
        for (int i = 0; i < eventDispatcher.mListeners.size(); i++)
        {
            mListeners.emplace_back(std::move(eventDispatcher.mListeners[i]));
        }

        eventDispatcher.mListeners.clear();
        return *this;
    }
    void AddListener(const std::function<bool(uint32_t code, void *data)> &listener)
    {
        mListeners.push_back(listener);
    }
    void RemoveListener(std::function<bool(uint32_t code, void *data)> listener);
    void Dispatch(int code, void *data);

    ~EventDispatcher();

private:
    std::vector<std::function<bool(uint32_t code, void *data)>> mListeners;
};