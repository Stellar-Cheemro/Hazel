#pragma once

#include <Hazel/Asset/AssetTypes.h>
#include <Hazel/Asset/Serialization/AssetSerializer.h>
#include <Hazel/Core/Containers.h>
#include <Hazel/Core/Core.h>
#include <Hazel/Core/Scope.h>

#include <cstddef>

namespace Hazel
{

class AssetSerializerRegistry
{
public:
    void Clear();

    bool Register(Scope<AssetSerializer> serializer);
    void Validate() const;

    AssetSerializer* GetSerializer(AssetType type);
    const AssetSerializer* GetSerializer(AssetType type) const;

    AssetSerializerRegistry() = default;
    ~AssetSerializerRegistry() = default;
    AssetSerializerRegistry(const AssetSerializerRegistry&) = delete;
    AssetSerializerRegistry& operator=(const AssetSerializerRegistry&) = delete;
    AssetSerializerRegistry(AssetSerializerRegistry&&) = delete;
    AssetSerializerRegistry& operator=(AssetSerializerRegistry&&) = delete;

private:
    Vector<Scope<AssetSerializer>> m_Serializers;
    HashMap<AssetType, size_t> m_SerializerIndexByType;
};

} // namespace Hazel