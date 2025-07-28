#pragma warning(push)
#pragma warning(disable: 4267 4244)
#include "dxc/DXIL/DxilModule.h"
#include "dxc/DxilContainer/DxilContainer.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/MemoryBuffer.h"
#pragma warning(pop)
#include "DxilModule.h"

namespace vista
{
	DxilModule::DxilModule(void const* shaderBytecode, Uint64 bytecodeSize)
	{
		hlsl::DxilProgramHeader const* pProgramHeader = reinterpret_cast<hlsl::DxilProgramHeader const*>(shaderBytecode);
		Uint32 partSize = static_cast<Uint32>(bytecodeSize);

		hlsl::DxilContainerHeader const* pContainer = hlsl::IsDxilContainerLike(shaderBytecode, bytecodeSize);
		if (pContainer != nullptr)
		{
			VISTA_ASSERT(hlsl::IsValidDxilContainer(pContainer, bytecodeSize));
			hlsl::DxilPartIterator it = std::find_if(begin(pContainer), end(pContainer), hlsl::DxilPartIsType(hlsl::DFCC_ShaderDebugInfoDXIL));
			VISTA_ASSERT(it != end(pContainer));

			pProgramHeader = reinterpret_cast<hlsl::DxilProgramHeader const*>(GetDxilPartData(*it));
			partSize = (*it)->PartSize;
		}
		VISTA_ASSERT(hlsl::IsValidDxilProgramHeader(pProgramHeader, partSize));

		Char const* pIL;
		Uint32 pILLength;
		hlsl::GetDxilProgramBitcode(pProgramHeader, &pIL, &pILLength);

		std::unique_ptr<llvm::MemoryBuffer> pBitcodeBuf(llvm::MemoryBuffer::getMemBuffer(llvm::StringRef(pIL, pILLength), "", false));
		llvmContext.reset(new llvm::LLVMContext);

		llvm::ErrorOr<std::unique_ptr<llvm::Module>> pModule(llvm::parseBitcodeFile(pBitcodeBuf->getMemBufferRef(), *llvmContext));
		if (std::error_code ec = pModule.getError())
		{
			VISTA_ASSERT(false);
		}
		llvmModule = std::move(pModule.get());
		dxilModule = hlsl::DxilModule::TryGetDxilModule(llvmModule.get());
	}

	hlsl::DxilModule& DxilModule::GetHLSLDxilModule()
	{
		return *dxilModule;
	}
}