#pragma once
#include <cstdint>

namespace Hazel
{
using AssetHandle = uint64_t;
enum class AssetType
{
    None = 0,
    Texture2D,
    Shader,
    Scene,
    Mesh,
    Audio
};
}