#include "AStar.h"
#include "DllApi.h"


//// remarks (�G��@):
//// case 1 (�s��) decltype(::YourMethod)*		: �@�Y���� #include �A�� DLL �� export dll header file, e.g. exported_methods.h
//// case 2 (�¦�) static_cast<void(*)(int x)>	: �@�Y�ۤv��������T
void YourMethod(int x); // from another DLL's header

// Model 1:

// ����Х�
Dll dllPathfinding("pathfinding.dll");

void use_1(void)
{
	static_cast<decltype(::YourMethod)*>(dllPathfinding["YourMethod"])(500);	// case 1
	// or
	static_cast<void(*)(int x)>(dllPathfinding["YourMethod"])(500);				// case 2
}


// OR

// Model 2:
// �о����

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
	pathfindingDll.YourMethod(500); // ��ۤv�Υ��A����
}