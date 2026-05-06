#include "EngineAssets.h"

#include <Hazel/Asset/AssetManager.h>
#include <Hazel/Asset/UserAssetManager.h>
#include <Hazel/Core/Log.h>

#include <array>
#include <cstddef>

namespace Hazel
{
namespace
{

constexpr size_t ShaderCount = static_cast<size_t>(EngineShader::Count);
constexpr size_t TextureCount = static_cast<size_t>(EngineTexture::Count);

std::array<AssetHandle, ShaderCount> s_ShaderHandles = {};
std::array<AssetHandle, TextureCount> s_TextureHandles = {};

template <typename Enum, size_t Count> bool IsValidIndex(Enum value)
{
    return static_cast<size_t>(value) < Count;
}

} // namespace

void EngineAssets::Init()
{
    s_ShaderHandles.fill(0);
    s_TextureHandles.fill(0);

    s_ShaderHandles[static_cast<size_t>(EngineShader::FlatColor)] =
        AssetManager::ImportEngineAsset("Shaders/FlatColor.glsl");

    s_ShaderHandles[static_cast<size_t>(EngineShader::Texture)] =
        AssetManager::ImportEngineAsset("Shaders/Texture.glsl");

    s_TextureHandles[static_cast<size_t>(EngineTexture::Checkerboard)] =
        AssetManager::ImportEngineAsset("Textures/Checkerboard.png");

    s_TextureHandles[static_cast<size_t>(EngineTexture::ChernoLogo)] =
        AssetManager::ImportEngineAsset("Textures/ChernoLogo.png");
}

void EngineAssets::Shutdown()
{
    s_ShaderHandles.fill(0);
    s_TextureHandles.fill(0);
}

AssetHandle EngineAssets::GetShaderHandle(EngineShader shader)
{
    if (!IsValidIndex<EngineShader, ShaderCount>(shader))
    {
        HAZEL_CORE_ERROR("Fail:get engine shader handle. Invalid EngineShader: {0}",
                         static_cast<int>(shader));
        return 0;
    }

    return s_ShaderHandles[static_cast<size_t>(shader)];
}

AssetHandle EngineAssets::GetTexture2DHandle(EngineTexture texture)
{
    if (!IsValidIndex<EngineTexture, TextureCount>(texture))
    {
        HAZEL_CORE_ERROR("Fail:get engine texture2d handle. Invalid EngineTexture: {0}",
                         static_cast<int>(texture));
        return 0;
    }

    return s_TextureHandles[static_cast<size_t>(texture)];
}

Ref<Shader> EngineAssets::GetShader(EngineShader shader)
{
    return UserAssetManager::LoadShader(GetShaderHandle(shader));
}

Ref<Texture2D> EngineAssets::GetTexture2D(EngineTexture texture)
{
    return UserAssetManager::LoadTexture2D(GetTexture2DHandle(texture));
}

} // namespace Hazel