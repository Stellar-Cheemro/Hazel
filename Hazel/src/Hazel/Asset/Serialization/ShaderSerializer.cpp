#include "ShaderSerializer.h"

#include <Hazel/Asset/AssetManager.h>
#include <Hazel/Asset/Runtime/ShaderAsset.h>
#include <Hazel/Core/Log.h>
#include <Hazel/Renderer/Shader.h>

#include <string>
#include <string_view>

namespace Hazel
{

namespace
{

bool TryLoadShader(const AssetMetadata& metadata, std::string_view resolvedPath, Ref<Asset>& asset)
{
    const std::string resolvedPathString(resolvedPath);

    Ref<Shader> shader = Shader::Create(resolvedPathString);
    if (!shader)
    {
        HAZEL_CORE_ERROR("Fail:load shader asset. Shader creation failed. Handle: {0}, Path: {1}",
                         metadata.Handle, resolvedPathString);
        return false;
    }

    Ref<ShaderAsset> shaderAsset = CreateRef<ShaderAsset>(shader);
    shaderAsset->Handle = metadata.Handle;

    asset = shaderAsset.As<Asset>();
    return true;
}

bool TryLoadShaderAssetByType(const AssetMetadata& metadata, std::string_view resolvedPath,
                              Ref<Asset>& asset)
{
    switch (metadata.Type)
    {
        case AssetType::Shader:
            return TryLoadShader(metadata, resolvedPath, asset);

            // 后续新增 Shader 细分类型时在这里分派：
            //
            // case AssetType::ComputeShader:
            //     return TryLoadComputeShader(metadata, resolvedPath, asset);
            //
            // case AssetType::GraphicsShader:
            //     return TryLoadGraphicsShader(metadata, resolvedPath, asset);

        default:
            HAZEL_CORE_ERROR("Fail:load shader asset. No shader loader for AssetType. Handle: {0}, "
                             "Type: {1}, FilePath: {2}",
                             metadata.Handle, static_cast<int>(metadata.Type), metadata.FilePath);
            return false;
    }
}

} // namespace

bool ShaderSerializer::CanLoad(AssetType type) const
{
    switch (type)
    {
        case AssetType::Shader:
            return true;

            // 后续如果拆分 ComputeShader / GraphicsShader / MaterialShader，
            // 可以在这里扩展：
            //
            // case AssetType::ComputeShader:
            // case AssetType::GraphicsShader:
            //     return true;

        default:
            return false;
    }
}

bool ShaderSerializer::TryLoad(const AssetMetadata& metadata, Ref<Asset>& asset)
{
    if (!CanLoad(metadata.Type))
    {
        HAZEL_CORE_ERROR(
            "Fail:load shader asset. Unsupported AssetType. Handle: {0}, Type: {1}, FilePath: {2}",
            metadata.Handle, static_cast<int>(metadata.Type), metadata.FilePath);
        return false;
    }

    const std::string resolvedPath = AssetManager::ResolveAssetPath(metadata);
    if (resolvedPath.empty())
        return false;

    return TryLoadShaderAssetByType(metadata, resolvedPath, asset);
}

} // namespace Hazel