#include "ShaderSerializer.h"
#include <Hazel/Asset/AssetManager.h>
#include <Hazel/Asset/ShaderAsset.h>
#include <Hazel/Core/Log.h>
#include <Hazel/Renderer/Shader.h>
#include <filesystem>
namespace Hazel
{
bool Hazel::ShaderSerializer::TrySerialize(const AssetMetadata& metadata, Ref<Asset>& asset)
{
    if (metadata.Type != AssetType::Shader)
    {
        return false;
    }
    std::filesystem::path FullPath = AssetManager::GetFileSystemPath(metadata);
    if (FullPath.empty())
    {
        HAZEL_CORE_ERROR("Shader file does not exist: {0} while trying to serialize",
                         FullPath.string());
        return false;
    }

    Ref<Shader> shader = Shader::Create(FullPath.string());
    if (!shader)
    {
        HAZEL_CORE_ERROR("Failed to create shader from file: {0} while trying to serialize",
                         FullPath.string());
        return false;
    }

    Ref<ShaderAsset> shaderAsset = Ref<ShaderAsset>::Create(shader);
    shaderAsset->Handle = metadata.handle;

    asset = shaderAsset.As<Asset>();
    return true;
}
} // namespace Hazel