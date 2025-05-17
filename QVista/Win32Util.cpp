#include "Win32Util.h"
#include <vector>

namespace win32util 
{

	bool CreateSuspendedProcess(std::wstring const& exePath,
		std::wstring const& workingDir,
		std::wstring const& arguments,
		PROCESS_INFORMATION& pi,
		std::wstring& errorMessage)
	{
		std::wstring cmdLine = L"\"" + exePath + L"\" " + arguments;
		std::vector<wchar_t> cmdLineBuf(cmdLine.begin(), cmdLine.end());
		cmdLineBuf.push_back(L'\0');

		STARTUPINFO si = { sizeof(si) };
		DWORD flags = CREATE_SUSPENDED | CREATE_NEW_CONSOLE;

		BOOL success = ::CreateProcessW(
			nullptr,
			cmdLineBuf.data(),
			nullptr,
			nullptr,
			FALSE,
			flags,
			nullptr,
			workingDir.empty() ? nullptr : workingDir.c_str(),
			&si,
			&pi
		);

		if (!success) 
		{
			DWORD error = GetLastError();
			wchar_t buffer[512];
			FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				nullptr, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				buffer, sizeof(buffer) / sizeof(wchar_t), nullptr);
			errorMessage = buffer;
			return false;
		}

		return true;
	}

	InjectionResult InjectDLL(DWORD processId, std::wstring const& dllPath)
	{
		InjectionResult result{ false, L"" };
		HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_CREATE_THREAD, FALSE, processId);
		if (!hProcess) 
		{
			result.errorMessage = L"Failed to open target process.";
			return result;
		}

		HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
		if (!hKernel32) 
		{
			result.errorMessage = L"Failed to get handle to kernel32.dll.";
			CloseHandle(hProcess);
			return result;
		}

		auto pLoadLibrary = reinterpret_cast<LPTHREAD_START_ROUTINE>(GetProcAddress(hKernel32, "LoadLibraryW"));
		if (!pLoadLibrary) 
		{
			result.errorMessage = L"Failed to get address of LoadLibraryW.";
			CloseHandle(hProcess);
			return result;
		}

		SIZE_T size = (dllPath.length() + 1) * sizeof(wchar_t);
		LPVOID remoteBuf = VirtualAllocEx(hProcess, nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (!remoteBuf) 
		{
			result.errorMessage = L"Failed to allocate memory in target process.";
			CloseHandle(hProcess);
			return result;
		}

		if (!WriteProcessMemory(hProcess, remoteBuf, dllPath.c_str(), size, nullptr)) 
		{
			result.errorMessage = L"Failed to write DLL path to remote process.";
			VirtualFreeEx(hProcess, remoteBuf, 0, MEM_RELEASE);
			CloseHandle(hProcess);
			return result;
		}

		HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0, pLoadLibrary, remoteBuf, 0, nullptr);
		if (!hThread) 
		{
			result.errorMessage = L"Failed to create remote thread.";
			VirtualFreeEx(hProcess, remoteBuf, 0, MEM_RELEASE);
			CloseHandle(hProcess);
			return result;
		}

		WaitForSingleObject(hThread, INFINITE);

		DWORD exitCode = 0;
		GetExitCodeThread(hThread, &exitCode);
		if (exitCode != 0) 
		{
			result.success = true;
		}
		else 
		{
			result.errorMessage = L"Remote LoadLibraryW returned 0 (failure).";
		}

		VirtualFreeEx(hProcess, remoteBuf, 0, MEM_RELEASE);
		CloseHandle(hThread);
		CloseHandle(hProcess);

		return result;
	}

	bool TerminateProcessById(DWORD processId, std::wstring& errorMessage)
	{
		HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processId);
		if (!hProcess) 
		{
			errorMessage = L"Failed to open process for termination.";
			return false;
		}

		BOOL result = ::TerminateProcess(hProcess, 1);
		CloseHandle(hProcess);

		if (!result) 
		{
			errorMessage = L"Failed to terminate process.";
			return false;
		}
		return true;
	}
} 
