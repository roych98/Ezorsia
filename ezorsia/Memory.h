#pragma once

class Memory
{
public:
	static bool SetHook(bool attach, void** ptrTarget, void* ptrDetour);
	static void FillBytes(DWORD dwOriginAddress, unsigned char ucValue, int nCount);
	static void WriteString(DWORD dwOriginAddress, const char* sContent, size_t nSize);
	static void WriteByte(DWORD dwOriginAddress, unsigned char ucValue);
	static void WriteShort(DWORD dwOriginAddress, unsigned short usValue);
	static void WriteInt(DWORD dwOriginAddress, unsigned int dwValue);
	static void CodeCave(void* ptrCodeCave, DWORD dwOriginAddress);
	static void WriteByteArray(const DWORD dwOriginAddress, const unsigned char ucValue[], const int ucValueSize);
};

