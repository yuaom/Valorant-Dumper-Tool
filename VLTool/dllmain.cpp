#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <errhandlingapi.h>

#include "utils.hpp"

#pragma comment(lib, "ntdll.lib")

DWORD WINAPI Main()
{
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	SetConsoleTitleA("Valorant Dumper | github.com/chase1803");

	DWORD TextPageCount = 0;
	DWORD DataPageCount = 0;

	PIMAGE_DOS_HEADER dos_header = (PIMAGE_DOS_HEADER)GetModuleHandleA(NULL);
	PIMAGE_NT_HEADERS nt_header = (PIMAGE_NT_HEADERS)((DWORD_PTR)dos_header + dos_header->e_lfanew);

	DWORD NumberOfSections = nt_header->FileHeader.NumberOfSections;
	PIMAGE_SECTION_HEADER Section = (PIMAGE_SECTION_HEADER)((DWORD_PTR)&nt_header->OptionalHeader + nt_header->FileHeader.SizeOfOptionalHeader);

	for (DWORD i = 0; i < NumberOfSections; ++i)
	{
		if (strcmp((const char*)(Section + i)->Name, ".text") == 0)
		{
			Utils::Memory::TextBase = (DWORD_PTR)dos_header + (Section + i)->PointerToRawData - 0x1000;
			Utils::Memory::TextSize += ((Section + i)->SizeOfRawData);
			TextPageCount = ((Section + i)->SizeOfRawData) / 0x1000;
			break;
		}
	}

	const char* code_buffer = (const char*)calloc(1, Utils::Memory::TextSize);
	*(PVOID*)&ZwRaiseException = GetProcAddress(GetModuleHandleA("ntdll.dll"), "ZwRaiseException");

	Utils::Fmt::PrintMessage("Preparing Dumper! Please Wait...\n");

	for (int i = 0; i < TextPageCount; i++)
	{
		PVOID address = (PVOID)(Utils::Memory::TextBase + i * 0x1000);

		MEMORY_BASIC_INFORMATION mbi;
		memset(&mbi, 0, sizeof(mbi));
		VirtualQuery(address, &mbi, sizeof(mbi));

		if (mbi.Protect == PAGE_NOACCESS)
			Utils::Memory::Protect(address);

		memset(&mbi, 0, sizeof(mbi));
		VirtualQuery(address, &mbi, sizeof(mbi));

		if (mbi.Protect != PAGE_NOACCESS)
			memcpy((char*)code_buffer + ((DWORD_PTR)address - Utils::Memory::TextBase), address, 0x1000);
	}

	Utils::GetAddresses(Utils::Memory::TextBase, code_buffer, Utils::Memory::TextSize);
	Utils::PrintOffsets();

	return 0;
};

DWORD WINAPI Lazy(LPVOID lpParameter)
{
	Main();
	return 0;
};

BOOL WINAPI DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		CloseHandle(CreateThread(NULL, 0, Lazy, NULL, 0, NULL));
	}
	return TRUE;
};
