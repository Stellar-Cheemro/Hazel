#pragma once
#include <Hazel/Asset/AssetTypes.h>
#include <Hazel/Core/Core.h>
#include <Hazel/Core/Ref.h>
#include <concepts>
namespace Hazel
{
class HAZEL_API Asset : public RefCounted
{
public:
    virtual ~Asset() = default;
    virtual AssetType GetAssetType() const = 0;

public:
    AssetHandle Handle = 0;
};
template <typename T>
concept AssetDerived = std::derived_from<T, Asset>;
} // namespace Hazel