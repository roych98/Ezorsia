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


		// Patch to UI Whisper Constraint
		Memory::WriteShort(0x8D4EA7, 0xEB);
		Memory::WriteShort(0x8D4EA8, 0x68);
		Memory::FillBytes(0x8D4EA9, 0x90, 0x8D4EAC - 0x8D4EA9);

		// Patch to /whisper Constraint
		Memory::WriteShort(0x52E4AA, 0xEB);
		Memory::WriteShort(0x52E4AB, 0x14);
		Memory::FillBytes(0x52E4AC, 0x90, 0x52E4AF - 0x52E4AC);

		// Spam Emotes
		Memory::WriteInt(0xA244AE, 0x83);
		Memory::WriteInt(0xA244AF, 0xF8);
		Memory::WriteByte(0xA244B0, 0x01);
		Memory::WriteByte(0xA244B1, 0x90);
		Memory::WriteByte(0xA244B2, 0x90);

		break;
	}
	default: break;
	case DLL_PROCESS_DETACH:
		ExitProcess(0);
	}
	return TRUE;
}