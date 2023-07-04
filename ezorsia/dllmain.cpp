// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "NMCO.h"
#include "INIReader.h"

void CreateConsole() {
	AllocConsole();
	FILE* stream;
	freopen_s(&stream, "CONOUT$", "w", stdout);
}

bool HookGetModuleFileName() {
	static decltype(&GetModuleFileNameW) _GetModuleFileNameW = &GetModuleFileNameW;

	const decltype(&GetModuleFileNameW) GetModuleFileNameW_Hook = [](HMODULE hModule, LPWSTR lpFileName, DWORD dwSize) -> DWORD {
		auto len = _GetModuleFileNameW(hModule, lpFileName, dwSize);
		// Check to see if the length is invalid (zero)
		if (!len) {
			// Try again without the provided module for a fixed result
			len = _GetModuleFileNameW(nullptr, lpFileName, dwSize);
		}
		return len;
	};

	return Memory::SetHook(true, reinterpret_cast<void**>(&_GetModuleFileNameW), GetModuleFileNameW_Hook);
}

/// <summary>
/// Creates a detour for the User32.dll CreateWindowExA function applying the following changes:
/// 1. Enable the window minimize box
/// </summary>
inline void HookCreateWindowExA() {
	static auto create_window_ex_a = decltype(&CreateWindowExA)(GetProcAddress(LoadLibraryA("USER32"), "CreateWindowExA"));
	static const decltype(&CreateWindowExA) hook = [](DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) -> HWND {
		dwStyle |= WS_MINIMIZEBOX; // enable minimize button
		return create_window_ex_a(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	};

	Memory::SetHook(true, reinterpret_cast<void**>(&create_window_ex_a), hook);
}

__declspec(naked) void IsValidCharacterAlwaysTrue() {
	__asm {
		mov eax, 1
		retn
	}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
	{
		//CreateConsole();
		std::cout << "2" << std::endl;
		HookCreateWindowExA();
		std::cout << "3" << std::endl;
		HookGetModuleFileName();
		std::cout << "GetModuleFileName hook created" << std::endl;
		NMCO::CreateHook();
		std::cout << "NMCO hook initialized" << std::endl;

		Memory::CodeCave(*IsValidCharacterAlwaysTrue, 0x79FC74);

		unsigned char Spam_Emotes[] = { 0x83, 0xF8, 0x01, 0x90, 0x90 };
		Memory::WriteByteArray(0xA244AE, Spam_Emotes, sizeof(Spam_Emotes));
		
		const unsigned char EquipItemsOfEveryJob[] = { 0xE9, 0x18, 0x01, 0x0, 0x0, 0x90, 0x90, 0x90, 0x90 };
		Memory::WriteByteArray(0x4F2D92, EquipItemsOfEveryJob, sizeof(EquipItemsOfEveryJob));

		break;
	}
	default: break;
	case DLL_PROCESS_DETACH:
		ExitProcess(0);
	}
	return TRUE;
}