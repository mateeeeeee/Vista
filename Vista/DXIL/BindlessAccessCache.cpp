#include "BindlessAccessCache.h"
#include "BindlessAccess.h"
#include "Utilities/Hash.h"

namespace vista
{
	std::vector<BindlessAccess> const& BindlessAccessCache::GetOrAdd(void const* shaderBytecode, Uint64 bytecodeSize)
	{
		static const std::vector<BindlessAccess> empty{};
		if (!shaderBytecode || bytecodeSize == 0)
		{
			return empty;
		}

		Uint64 const shaderHash = crc64(static_cast<Char const*>(shaderBytecode), bytecodeSize);
		if (auto it = bindlessAccessMap.find(shaderHash); it != bindlessAccessMap.end())
		{
			return it->second;
		}

		std::vector<BindlessAccess> results = FindBindlessResourceAccesses(shaderBytecode, bytecodeSize);
		auto [inserted_it, was_inserted] = bindlessAccessMap.emplace(shaderHash, std::move(results));
		return inserted_it->second;
	}

	Bool BindlessAccessCache::Contains(void const* shaderBytecode, Uint64 bytecodeSize) const
	{
		if (!shaderBytecode || bytecodeSize == 0)
		{
			return false;
		}
		Uint64 const shaderHash = crc64(static_cast<Char const*>(shaderBytecode), bytecodeSize);
		return bindlessAccessMap.contains(shaderHash);
	}

	void BindlessAccessCache::Clear()
	{
		bindlessAccessMap.clear();
	}
}

