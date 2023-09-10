#ifdef _WIN32
#include <Windows.h>

typedef void PlatformLib;

PlatformLib* Platform_LoadLibrary(const char* pszPlatformLibName)
{
	HMODULE hModule = ::LoadLibraryA(pszPlatformLibName);
	return static_cast<PlatformLib *>(hModule);
}

void Platform_UnloadLibrary(PlatformLib* pPlatformLib)
{
	::FreeLibrary(static_cast<HMODULE>(pPlatformLib));
}

void* Platform_GetLibPfn(PlatformLib* pPlatformLib, const char *pszMethodName)
{
	return ::GetProcAddress(static_cast<HMODULE>(pPlatformLib), pszMethodName);
}

#else
#error "Implementation time for a specific platform."
#endif