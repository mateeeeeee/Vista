#include <string>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace win32util
{
	struct InjectionResult 
	{
		bool success;
		std::wstring errorMessage;
	};

	bool CreateSuspendedProcess(std::wstring const& exePath,
		std::wstring const& workingDir,
		std::wstring const& arguments,
		PROCESS_INFORMATION& pi,
		std::wstring& errorMessage);

	InjectionResult InjectDLL(DWORD processId, const std::wstring& dllPath);

	bool TerminateProcessById(DWORD processId, std::wstring& errorMessage);

}