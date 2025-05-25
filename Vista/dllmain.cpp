#include "Core/Vista.h"

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID)
{
	DisableThreadLibraryCalls(hInstance);
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		g_Vista.Initialize();
		break;
	case DLL_PROCESS_DETACH:
		g_Vista.Shutdown();
		break;
	}
	return TRUE;
}