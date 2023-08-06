#pragma once

namespace Anker {

// AssetPtr is a reference counted pointer to a specific asset.
template <typename T>
using AssetPtr = std::shared_ptr<T>;

template <typename T>
AssetPtr<T> makeAssetPtr(T&& v)
{
	return std::make_shared<T>(std::forward<T>(v));
}

template <typename T, typename... Args>
AssetPtr<T> makeAssetPtr(Args&&... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

} // namespace Anker
