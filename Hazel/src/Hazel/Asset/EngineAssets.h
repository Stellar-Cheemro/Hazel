#pragma once

#include <Hazel/Asset/AssetTypes.h>
#include <Hazel/Core/Core.h>
#include <Hazel/Core/Ref.h>
#include <Hazel/Renderer/Shader.h>
#include <Hazel/Renderer/Texture.h>

#include <cstdint>

namespace Hazel
{

enum class EngineShader : uint8_t
{
    FlatColor = 0,
    Texture,

    Count
};

enum class EngineTexture : uint8_t
{
    Checkerboard = 0,
    ChernoLogo,

    Count
};

class HAZEL_API EngineAssets
{
public:
    static void Init();
    static void Shutdown();

    static AssetHandle GetShaderHandle(EngineShader shader);
    static AssetHandle GetTexture2DHandle(EngineTexture texture);

    static Ref<Shader> GetShader(EngineShader shader);
    static Ref<Texture2D> GetTexture2D(EngineTexture texture);
};

} // namespace Hazel