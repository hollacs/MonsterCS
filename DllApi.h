#pragma once

class Dll
{
public: // �p�G�Y Model 2, �i�H�వ protected:�A���h�v�T���j
	Dll(const char* pszDllName)
	{
		typedef void PlatformLib;
		PlatformLib* Platform_LoadLibrary(const char* pszPlatformLibName);
		m_pLib = Platform_LoadLibrary(pszDllName);
	}

	~Dll(void)
	{
		typedef void PlatformLib;
		void Platform_UnloadLibrary(PlatformLib * pPlatformLib);
		Platform_UnloadLibrary(m_pLib);
	}

	void* operator[](const char *pszMethodName) const
	{
		typedef void PlatformLib;
		void* Platform_GetLibPfn(PlatformLib * pPlatformLib, const char* pszMethodName);
		return Platform_GetLibPfn(m_pLib, pszMethodName);
	}

private:
	typedef void PlatformLib;
	PlatformLib* m_pLib;
};