#include "AStar.h"
#include "DllApi.h"


//// remarks (二選一):
//// case 1 (新式) decltype(::YourMethod)*		: 一係直接 #include 你個 DLL 個 export dll header file, e.g. exported_methods.h
//// case 2 (舊式) static_cast<void(*)(int x)>	: 一係自己打類型資訊
void YourMethod(int x); // from another DLL's header

// Model 1:

// 易整煩用
Dll dllPathfinding("pathfinding.dll");

void use_1(void)
{
	static_cast<decltype(::YourMethod)*>(dllPathfinding["YourMethod"])(500);	// case 1
	// or
	static_cast<void(*)(int x)>(dllPathfinding["YourMethod"])(500);				// case 2
}


// OR

// Model 2:
// 煩整易用

class PathfindingDll : private Dll
{
public:
	PathfindingDll(void) : Dll("pathfinding.dll") {}

	void YourMethod(int x)
	{
		static_cast<decltype(::YourMethod)*>(dllPathfinding["YourMethod"])(x);	// case 1
		// or
		static_cast<void(*)(int x)>(dllPathfinding["YourMethod"])(500);			// case 2
	}
};

PathfindingDll pathfindingDll;

void use_2(void)
{
	pathfindingDll.YourMethod(500); // 當自己屋企，任用
}