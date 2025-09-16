#include "StringConversions.h"

namespace vista
{
	std::wstring ToWideString(std::string const& str)
	{
		Int num_chars = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (Int)str.length(), NULL, 0);
		std::wstring wstr;
		if (num_chars)
		{
			wstr.resize(num_chars);
			MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (Int)str.length(), &wstr[0], num_chars);
		}
		return wstr;
	}
	std::string ToString(std::wstring const& wstr)
	{
		Int num_chars = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (Int)wstr.length(), NULL, 0, NULL, NULL);
		std::string str;
		if (num_chars > 0)
		{
			str.resize(num_chars);
			WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (Int)wstr.length(), &str[0], num_chars, NULL, NULL);
		}
		return str;
	}

	std::string HResultToString(HRESULT hr)
	{
		switch (hr)
		{
		case S_OK: return "S_OK";
		case DXGI_ERROR_DEVICE_HUNG: return "DXGI_ERROR_DEVICE_HUNG";
		case DXGI_ERROR_DEVICE_REMOVED: return "DXGI_ERROR_DEVICE_REMOVED";
		case DXGI_ERROR_DEVICE_RESET: return "DXGI_ERROR_DEVICE_RESET";
		case DXGI_ERROR_DRIVER_INTERNAL_ERROR: return "DXGI_ERROR_DRIVER_INTERNAL_ERROR";
		case DXGI_ERROR_FRAME_STATISTICS_DISJOINT: return "DXGI_ERROR_FRAME_STATISTICS_DISJOINT";
		case DXGI_ERROR_INVALID_CALL: return "DXGI_ERROR_INVALID_CALL";
		case DXGI_ERROR_WAS_STILL_DRAWING: return "DXGI_ERROR_WAS_STILL_DRAWING";
		case E_FAIL: return "E_FAIL";
		case E_INVALIDARG: return "E_INVALIDARG";
		case E_OUTOFMEMORY: return "E_OUTOFMEMORY";
		case E_ACCESSDENIED: return "E_ACCESSDENIED";
		case E_NOTIMPL: return "E_NOTIMPL";
		}
		return "Unknown HRESULT code";
	}

}

