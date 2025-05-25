#pragma once
#include "Tracking/ObjectTraits.h"

namespace vista
{
	struct StateObjectDesc
	{
		D3D12_STATE_OBJECT_TYPE type;

		struct Subobject
		{
			D3D12_STATE_SUBOBJECT_TYPE type;

			std::vector<Uint8> data;

			Ref<IUnknown> interfaceRef;

			std::vector<std::wstring> ownedExportNames;
			std::vector<D3D12_EXPORT_DESC> ownedExportStructs;

			std::vector<std::wstring> ownedAssociationExportNames;
			std::vector<LPCWSTR> ownedAssociationExportPointers;
			Int32 associatedSubobjectIndex = -1;
			std::wstring associatedSubobjectName;

			std::wstring hitGroupName;
			std::wstring anyHitShader;
			std::wstring closestHitShader;
			std::wstring intersectionShader;
			D3D12_HIT_GROUP_TYPE hitGroupType;

			LPCWSTR const* originalAssociationExportsPtr = nullptr;
			const D3D12_STATE_SUBOBJECT* originalAssociatedSubobjectPtr = nullptr;
		};
		std::vector<Subobject> subobjects;
	};

	template<>
	struct ObjectTraits<ID3D12StateObject>
	{
		using Desc = StateObjectDesc;
		static constexpr ObjectType Type = ObjectType::StateObject;
	};
}