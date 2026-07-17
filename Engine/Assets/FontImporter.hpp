#pragma once
#include "Assets/Font.hpp"
#include <string_view>

class FontImporter
{
public:
    Font Import(std::string_view filename);

private:
};