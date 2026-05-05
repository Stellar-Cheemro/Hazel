#pragma once

#include <Hazel/Asset/Asset.h>
#include <Hazel/Core/Core.h>
#include <Hazel/Core/Ref.h>
#include <Hazel/Renderer/Shader.h>

namespace Hazel
{

class HAZEL_API ShaderAsset : public Asset
{
public:
    ShaderAsset() = default;

    explicit ShaderAsset(const Ref<Shader>& shader) : m_Shader(shader)
    {
    }

    AssetType GetAssetType() const override
    {
        return AssetType::Shader;
    }

    const Ref<Shader>& GetShader() const
    {
        return m_Shader;
    }

private:
    Ref<Shader> m_Shader;
};

} // namespace Hazel