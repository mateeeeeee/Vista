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
	class DxilModuleWrapper
	{
	public:
		DxilModuleWrapper(void const* shaderBytecode, Uint64 bytecodeSize);
		~DxilModuleWrapper();
		hlsl::DxilModule& GetDxilModule();

	private:
		std::unique_ptr<llvm::LLVMContext> llvmContext;
		std::unique_ptr<llvm::Module> llvmModule;
		hlsl::DxilModule* dxilModule;
	};
}