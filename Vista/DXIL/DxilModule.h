#pragma once

namespace llvm
{
	class LLVMContext;
	class Module;
}
namespace hlsl
{
	class DxilModule;
}

namespace vista
{
	class DxilModule
	{
	public:
		DxilModule(void const* shaderBytecode, Uint64 bytecodeSize);

		hlsl::DxilModule& GetHLSLDxilModule();

	private:
		std::unique_ptr<llvm::LLVMContext> llvmContext;
		std::unique_ptr<llvm::Module> llvmModule;
		hlsl::DxilModule* dxilModule;
	};
}