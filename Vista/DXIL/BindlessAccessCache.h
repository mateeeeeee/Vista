#pragma once
#include "BindlessAccess.h"

namespace vista
{
	class BindlessAccessCache
	{
	public:
		BindlessAccessCache() = default;
		~BindlessAccessCache() = default;

		VISTA_NONCOPYABLE(BindlessAccessCache)
		VISTA_DEFAULT_MOVABLE(BindlessAccessCache)

		VISTA_MAYBE_UNUSED std::vector<BindlessAccess> const& GetOrAdd(void const* shaderBytecode, Uint64 bytecodeSize);
		Bool Contains(void const* shaderBytecode, Uint64 bytecodeSize) const;
		void Clear();

	private:
		std::unordered_map<Uint64, std::vector<BindlessAccess>> bindlessAccessMap;
	};
}