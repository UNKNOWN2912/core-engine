#pragma once
#include <cstdint>
#include <functional>
#include <vector>

#define BindMember(callback) std::bind(&callback, this, std::placeholders::_1, std::placeholders::_2)
#define BindFunction(callback) std::bind(&callback, std::placeholders::_1, std::placeholders::_2)

class EventDispatcher
{
    public:
        void AddListener(std::function<bool(uint32_t code, void* data)> listener) { mListeners.push_back(listener); }
        void RemoveListener(std::function<bool(uint32_t code, void* data)> listener);

        void Dispatch(int code, void* data)
        {
            for (size_t i = 0; i < mListeners.size(); i++)
            {
                if(mListeners[i](code, data) == true)
                {
                    return;
                }
            }
        }

    private:
        std::vector<std::function<bool(uint32_t code, void* data)>> mListeners;
};