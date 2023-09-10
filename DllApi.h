#pragma once

class Dll
{
public: // 如果係 Model 2, 可以轉做 protected:，雖則影響唔大
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