#pragma once
#include "Assets/Font.hpp"
#include <string_view>

class FontImporter
{
public:
    Font Import(std::string_view filename, uint32_t size);
    void Initialize();

private:
    bool mInitialize = false;
};