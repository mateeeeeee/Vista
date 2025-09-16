#pragma once

namespace vista
{
	std::wstring ToWideString(std::string const&);
	std::string ToString(std::wstring const&);
	std::string HResultToString(HRESULT hr);
}