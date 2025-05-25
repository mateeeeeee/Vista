#pragma once
#include "Tracking/ObjectTraits.h"

namespace vista
{
	struct CommandSignatureDesc
	{
		D3D12_COMMAND_SIGNATURE_DESC cmdSignatureDesc;
		ObjectID rootSignatureId = InvalidObjectID;
	};
	template<>
	struct ObjectTraits<ID3D12CommandSignature>
	{
		using Desc = CommandSignatureDesc;
		static constexpr ObjectType Type = ObjectType::CommandSignature;
	};
}