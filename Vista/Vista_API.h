#pragma once

#ifdef VISTA_EXPORTS
#define VISTA_API __declspec(dllexport)
#else
#define VISTA_API __declspec(dllimport)
#endif

extern "C" 
{
	void FreezeCapture();
} 
