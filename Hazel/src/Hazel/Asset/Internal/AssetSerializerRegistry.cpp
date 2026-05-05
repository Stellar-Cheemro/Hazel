#include "AssetSerializerRegistry.h"

#include <Hazel/Core/Log.h>

#include <utility>

namespace Hazel
{

void AssetSerializerRegistry::Clear()
{
    m_SerializerIndexByType.clear();
    m_Serializers.clear();
}

bool AssetSerializerRegistry::Register(Scope<AssetSerializer> serializer)
{
    if (!serializer)
    {
        HAZEL_CORE_ERROR("Fail:register asset serializer. Serializer is null.");
        return false;
    }

    const size_t serializerIndex = m_Serializers.size();
    AssetSerializer* serializerPtr = serializer.get();

    bool registeredAny = false;

    for (AssetType type : SupportedFileAssetTypes)
    {
        if (!serializerPtr->CanLoad(type))
            continue;

        if (m_SerializerIndexByType.find(type) != m_SerializerIndexByType.end())
        {
            HAZEL_CORE_ERROR("Fail:register asset serializer. Duplicate serializer. AssetType: {0}",
                             static_cast<int>(type));
            continue;
        }

        m_SerializerIndexByType.emplace(type, serializerIndex);
        registeredAny = true;
    }

    if (!registeredAny)
    {
        HAZEL_CORE_ERROR("Fail:register asset serializer. Serializer supports no known AssetType.");
        return false;
    }

    m_Serializers.emplace_back(std::move(serializer));
    return true;
}

void AssetSerializerRegistry::Validate() const
{
    for (AssetType type : SupportedFileAssetTypes)
    {
        if (m_SerializerIndexByType.find(type) != m_SerializerIndexByType.end())
            continue;

        HAZEL_CORE_ERROR("Fail:validate serializer registry. Missing serializer. AssetType: {0}",
                         static_cast<int>(type));
    }
}

AssetSerializer* AssetSerializerRegistry::GetSerializer(AssetType type)
{
    auto it = m_SerializerIndexByType.find(type);
    if (it == m_SerializerIndexByType.end())
        return nullptr;

    const size_t index = it->second;
    if (index >= m_Serializers.size())
        return nullptr;

    return m_Serializers[index].get();
}

const AssetSerializer* AssetSerializerRegistry::GetSerializer(AssetType type) const
{
    auto it = m_SerializerIndexByType.find(type);
    if (it == m_SerializerIndexByType.end())
        return nullptr;

    const size_t index = it->second;
    if (index >= m_Serializers.size())
        return nullptr;

    return m_Serializers[index].get();
}

} // namespace Hazel