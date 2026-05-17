#pragma once
#include <string_view>

class Model
{
    public:
        void LoadModel(std::string_view filename);
        void DestroyModel();
    private:
};