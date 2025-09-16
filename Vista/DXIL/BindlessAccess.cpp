#include "BindlessAccess.h"
#include "DxilModuleWrapper.h"
#include "Utilities/Hash.h"
#pragma warning(push)
#pragma warning(disable: 4267 4244)
#include "dxc/DXIL/DxilModule.h"
#include "dxc/DXIL/DxilOperations.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#pragma warning(pop)


namespace vista
{
	inline Bool operator==(CBufferSourceInfo const& lhs, CBufferSourceInfo const& rhs)
	{
		return lhs.ResourceBinding == rhs.ResourceBinding &&
			lhs.ResourceSpace == rhs.ResourceSpace &&
			lhs.ByteOffset == rhs.ByteOffset &&
			lhs.ResourceName == rhs.ResourceName;
	}

	struct BindlessAccessHasher
	{
		Uint64 operator()(BindlessAccess const& s) const noexcept
		{
			HashState hashState{};
			hashState.Combine(std::hash<Int>{}(static_cast<Int>(s.ResourceKind)));
			hashState.Combine(std::hash<Int>{}(static_cast<Int>(s.ResourceClass)));
			hashState.Combine(std::hash<IndexSourceType>{}(s.Index.type));
			Uint64 detailsHash = std::visit([](auto&& arg) -> Uint64
				{
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, vista::CBufferSourceInfo>) 
				{
					HashState cbufHashState{};
					cbufHashState.Combine(std::hash<std::string>{}(arg.ResourceName));
					cbufHashState.Combine(std::hash<Uint32>{}(arg.ResourceBinding));
					cbufHashState.Combine(std::hash<Uint32>{}(arg.ResourceSpace));
					cbufHashState.Combine(std::hash<Uint32>{}(arg.ByteOffset));
					return cbufHashState;
				}
				else 
				{
					return std::hash<T>{}(arg);
				}
				}, s.Index.details);

			hashState.Combine(detailsHash);
			return hashState;
		}
	};

	struct BindlessAccessComparer
	{
		Bool operator()(BindlessAccess const& lhs, BindlessAccess const& rhs) const
		{
			if (lhs.ResourceKind != rhs.ResourceKind ||
				lhs.ResourceClass != rhs.ResourceClass ||
				lhs.Index.type != rhs.Index.type)
			{
				return false;
			}
			return lhs.Index.details == rhs.Index.details;
		}
	};

	static IndexSource AnalyzeIndexSource(hlsl::DxilModule& module, llvm::Value* indexValue)
	{
		IndexSource indexSource = { IndexSourceType::Unknown, std::monostate{} };
		if (llvm::ConstantInt* constInt = llvm::dyn_cast<llvm::ConstantInt>(indexValue))
		{
			return { IndexSourceType::ImmediateConstant, constInt->getZExtValue() };
		}
		auto GetCBufferInfo = [&](llvm::Value* handleValue) -> std::optional<hlsl::DxilCBuffer>
		{
			llvm::Value* currentHandle = handleValue;
			while (currentHandle) 
			{
				llvm::CallInst* call = llvm::dyn_cast<llvm::CallInst>(currentHandle);
				if (!call)
				{
					break;
				}

				hlsl::OP* dxilOp = module.GetOP();
				hlsl::DXIL::OpCode opCode = dxilOp->getOpCode(call);
				if (opCode == hlsl::DXIL::OpCode::CreateHandleFromBinding) 
				{
					llvm::Constant* resBind = llvm::dyn_cast<llvm::Constant>(call->getArgOperand(1));
					if (resBind) 
					{
						//ResBind struct: { lowerBound, upperBound, space, resourceClass }
						llvm::ConstantInt* resClassVal = llvm::cast<llvm::ConstantInt>(resBind->getAggregateElement(3u));
						if (static_cast<hlsl::DXIL::ResourceClass>(resClassVal->getZExtValue()) == hlsl::DXIL::ResourceClass::CBuffer) 
						{
							llvm::ConstantInt* lowerBound = llvm::cast<llvm::ConstantInt>(resBind->getAggregateElement(0u));
							llvm::ConstantInt* upperBound = llvm::cast<llvm::ConstantInt>(resBind->getAggregateElement(1u));
							llvm::ConstantInt* registerSpace = llvm::cast<llvm::ConstantInt>(resBind->getAggregateElement(2u));

							auto const& cbuffers = module.GetCBuffers();
							for (auto const& cbuffer : cbuffers)
							{
								if (cbuffer->GetLowerBound() == lowerBound->getZExtValue() &&
									cbuffer->GetUpperBound() == upperBound->getZExtValue() &&
									cbuffer->GetSpaceID() == registerSpace->getZExtValue())
								{
									return *cbuffer;
								}
							}
							return std::nullopt;
						}
					}
					break; 
				}
				else if (opCode == hlsl::DXIL::OpCode::CreateHandle) 
				{
					// Signature: CreateHandle(opcode, resClass, rangeId, ...)
					llvm::ConstantInt* resClassVal = llvm::cast<llvm::ConstantInt>(call->getArgOperand(1));
					if (static_cast<hlsl::DXIL::ResourceClass>(resClassVal->getZExtValue()) == hlsl::DXIL::ResourceClass::CBuffer) 
					{
						llvm::ConstantInt* rangeIdVal = llvm::cast<llvm::ConstantInt>(call->getArgOperand(2));
						return module.GetCBuffer((Uint32)rangeIdVal->getZExtValue());
					}
					break;
				}
				else if (opCode == hlsl::DXIL::OpCode::AnnotateHandle) 
				{
					currentHandle = call->getArgOperand(1);
				}
				else 
				{
					break;
				}
			}
			return std::nullopt; 
		};

		//The index comes directly from a load instruction.
		if (llvm::CallInst* call = llvm::dyn_cast<llvm::CallInst>(indexValue))
		{
			hlsl::OP* dxilOp = module.GetOP();
			if (dxilOp->getOpCode(call) == hlsl::DXIL::OpCode::BufferLoad) 
			{
				//CBV: BufferLoad(handle, byte_offset, alignment)
				std::optional<hlsl::DxilCBuffer> cbuffer = GetCBufferInfo(call->getArgOperand(1));
				if (cbuffer) 
				{
					if (llvm::ConstantInt* offsetConst = llvm::dyn_cast<llvm::ConstantInt>(call->getArgOperand(2)))
					{
						CBufferSourceInfo cbufferSource{};
						cbufferSource.ResourceName = cbuffer->GetGlobalName();
						cbufferSource.ResourceBinding = cbuffer->GetLowerBound();
						cbufferSource.ResourceSpace = cbuffer->GetSpaceID();
						cbufferSource.ByteOffset = (Uint32)offsetConst->getZExtValue();
						return { IndexSourceType::ConstantBuffer, cbufferSource };
					}
				}
			}
		}
		//The index comes from extracting a value from a legacy load.
		else if (llvm::ExtractValueInst* extractInst = llvm::dyn_cast<llvm::ExtractValueInst>(indexValue))
		{
			if (llvm::CallInst* call = llvm::dyn_cast<llvm::CallInst>(extractInst->getOperand(0)))
			{
				hlsl::OP* dxilOp = module.GetOP();
				if (dxilOp->getOpCode(call) == hlsl::DXIL::OpCode::CBufferLoadLegacy) 
				{
					std::optional<hlsl::DxilCBuffer> cbuffer = GetCBufferInfo(call->getArgOperand(1));
					if (cbuffer) 
					{
						Uint32 baseByteOffset = 0;
						if (llvm::ConstantInt* offsetConst = llvm::dyn_cast<llvm::ConstantInt>(call->getArgOperand(2)))
						{
							baseByteOffset = (Uint32)offsetConst->getZExtValue();
						}

						Uint32 const componentIndex = *extractInst->getIndices().begin();
						CBufferSourceInfo cbufferSource{};
						cbufferSource.ResourceName = cbuffer->GetGlobalName();
						cbufferSource.ResourceBinding = cbuffer->GetLowerBound();
						cbufferSource.ResourceSpace = cbuffer->GetSpaceID();
						cbufferSource.ByteOffset = baseByteOffset * 16 + (componentIndex * 4); 
						return { IndexSourceType::ConstantBuffer, cbufferSource };
					}
				}
			}
		}
		return indexSource; 
	}

	std::vector<BindlessAccess> FindBindlessResourceAccesses(const void* shaderBytecode, Uint64 bytecodeSize) 
	{
		DxilModuleWrapper dxilModule(shaderBytecode, bytecodeSize);
		hlsl::DxilModule& hlslModule = dxilModule.GetDxilModule();

		std::unordered_set<BindlessAccess, BindlessAccessHasher, BindlessAccessComparer> results;
		llvm::Module* module = hlslModule.GetModule();
		hlsl::OP* dxilOp = hlslModule.GetOP();

		for (llvm::Function& F : *module) 
		{
			if (F.isDeclaration())
			{
				continue;
			}

			for (llvm::BasicBlock& BB : F) 
			{
				for (llvm::Instruction& I : BB) 
				{
					auto* createHandleCall = llvm::dyn_cast<llvm::CallInst>(&I);
					if (!createHandleCall || dxilOp->getOpCode(createHandleCall) != hlsl::DXIL::OpCode::CreateHandleFromHeap) 
					{

						continue;
					}

					BindlessAccess result;
					result.Index = AnalyzeIndexSource(hlslModule, createHandleCall->getArgOperand(1));
					for (llvm::User* user : createHandleCall->users()) 
					{
						if (llvm::CallInst* annotateCall = llvm::dyn_cast<llvm::CallInst>(user))
						{
							if (dxilOp->getOpCode(annotateCall) == hlsl::DXIL::OpCode::AnnotateHandle) 
							{
								llvm::Constant* props = llvm::dyn_cast<llvm::Constant>(annotateCall->getArgOperand(2));
								if (props) 
								{
									//UNPACKING BASED ON DxilResourceProperties.h
									llvm::ConstantInt* props0Val = llvm::cast<llvm::ConstantInt>(props->getAggregateElement(0u));
									Uint32 const rawDword0 = (Uint32)props0Val->getZExtValue();
									Uint32 const kindVal = rawDword0 & 0xFF;
									result.ResourceKind = static_cast<hlsl::DXIL::ResourceKind>(kindVal);
									Bool const isUAV = (rawDword0 >> 12) & 1;
									result.ResourceClass = isUAV ? hlsl::DXIL::ResourceClass::UAV : hlsl::DXIL::ResourceClass::SRV;
									break;
								}
							}
						}
					}

					if (result.ResourceKind == hlsl::DXIL::ResourceKind::Invalid || result.ResourceKind == hlsl::DXIL::ResourceKind::Sampler) 
					{
						llvm::ConstantInt* isSamplerConst = llvm::dyn_cast<llvm::ConstantInt>(createHandleCall->getArgOperand(2));
						if (isSamplerConst && isSamplerConst->getZExtValue() != 0) 
						{
							result.ResourceClass = hlsl::DXIL::ResourceClass::Sampler;
							result.ResourceKind = hlsl::DXIL::ResourceKind::Sampler;
						}
					}
					results.insert(result);
				}
			}
		}
		return std::vector<BindlessAccess>(results.begin(), results.end());
	}
}
