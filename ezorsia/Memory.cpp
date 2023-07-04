#include "stdafx.h"
#include "Memory.h"
#include "detours.h"

bool Memory::SetHook(bool attach, void** ptrTarget, void* ptrDetour)
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	(attach ? DetourAttach : DetourDetach)(ptrTarget, ptrDetour);
	DetourTransactionCommit();
	DetourTransactionAbort();
	return true;
}

void Memory::FillBytes(const DWORD dwOriginAddress, const unsigned char ucValue, const int nCount) {
	memset((void*)dwOriginAddress, ucValue, nCount);
}

void Memory::WriteString(const DWORD dwOriginAddress, const char* sContent, const size_t nSize) {
	memset((void*)dwOriginAddress, *sContent, nSize);
}

void Memory::WriteByte(const DWORD dwOriginAddress, const unsigned char ucValue) {
	*(unsigned char*)dwOriginAddress = ucValue;
}

void Memory::WriteByteArray(const DWORD dwOriginAddress, const unsigned char ucValue[], const int ucValueSize) {
	for (int i = 0; i < ucValueSize; i++) {
		const DWORD newAddr = dwOriginAddress + i;
		*(unsigned char*)newAddr = ucValue[i];
	}
}

void Memory::WriteShort(const DWORD dwOriginAddress, const unsigned short usValue) {
	*(unsigned short*)dwOriginAddress = usValue;
}

void Memory::WriteInt(const DWORD dwOriginAddress, const unsigned int dwValue) {
	*(unsigned int*)dwOriginAddress = dwValue;
}

void Memory::CodeCave(void* ptrCodeCave, const DWORD dwOriginAddress) {
	__try {
		WriteByte(dwOriginAddress, 0xe9); // jmp instruction
		WriteInt(dwOriginAddress + 1, (int)(((int)ptrCodeCave - (int)dwOriginAddress) - 5)); // [jmp(1 byte)][address(4 bytes)]
	} __except (EXCEPTION_EXECUTE_HANDLER) {}
}
