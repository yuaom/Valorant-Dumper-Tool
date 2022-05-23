#pragma once

#include <winternl.h>
#include <fstream>
#include <Psapi.h>
#include <heapapi.h>
#include <dbghelp.h>
#include <shlobj.h>
#include <tchar.h>

void(__stdcall* ZwRaiseException)(PEXCEPTION_RECORD ExceptionRecord, PCONTEXT ThreadContext, BOOLEAN HandleException);

namespace Utils
{
	namespace Fmt
	{
		void PrintMessage(const char* fmt, ...);
		void PrintSingleCharacter(char fmt);
	}

	namespace Memory
	{
		uintptr_t PatternScan(const char* buffer, uint64_t startAddress, uint64_t size, const char* pattern, const char* mask, int offset);

		void Protect(PVOID address);
		DWORD WINAPI RaiseEx(PVOID address);

		DWORD_PTR TextBase = 0;
		DWORD TextSize = 0;
	}

	bool GetAddresses(DWORD_PTR Address, const char* buffer, DWORD Size);
	bool PrintOffsets();
};

namespace Offsets
{
	namespace Globals
	{
		uintptr_t UWorldState, UWorldKey = UWorldState + 0x38;
	}
	uintptr_t OwningGameInstance, LocalPlayers, LocalPawn, RootComponent, RootPosition, DamageController, CameraPosition, CameraRotation, CameraFOV;
};

bool Utils::GetAddresses(DWORD_PTR Address, const char* buffer, DWORD Size)
{
	Offsets::Globals::UWorldState = Memory::PatternScan(buffer, Address, Size, "\x4B\x8B\xBC\xD5\x00\x00\x00\x00\x41\x8B\xC2\x2B\xC2\x45\x8B\xCA", "xxxx????xxxxxxxx", 0x5);
	if (!Offsets::Globals::UWorldState)
	{
		Utils::Fmt::PrintMessage("Failed to get UWorldState!\n");
	}

	Offsets::OwningGameInstance = Memory::PatternScan(buffer, Address, Size, "\x49\x8B\x87\x00\x00\x00\x00\x48\x85\xC0\x74\x09\x48\x8B\xB0\x00\x00\x00\x00\xEB\x07", "xxx????xxxxxxxx????xx", 0x4);
	if (!Offsets::OwningGameInstance)
	{
		Utils::Fmt::PrintMessage("Failed to get OwningGameInstance!\n");
	}

	Offsets::LocalPlayers = Memory::PatternScan(buffer, Address, Size, "\x4E\x8B\x4C\xC1\x00\x49\xC1\xEA\x20\xB8\x00\x00\x00\x00\x41\xF7\xE0\x41\x8B\xC0", "xxxx?xxxxx????xxxxxx", 0x5);
	if (!Offsets::LocalPlayers)
	{
		Utils::Fmt::PrintMessage("Failed to get LocalPlayers!\n");
	}

	Offsets::LocalPawn = Memory::PatternScan(buffer, Address, Size, "\x48\x8B\x40\x08\xFF\x15\x00\x00\x00\x00\x90\x48\x85\xF6\x74\x14\x48\x8B\x06\x48\x8B\xCE\x48\x8B\x80\x00\x00\x00\x00\xFF\x15\x00\x00\x00\x00\x90", "xxxxxx????xxxxxxxxxxxxxxx????xx????x", 0x1A);
	if (!Offsets::LocalPawn)
	{
		Utils::Fmt::PrintMessage("Failed to get LocalPawn!\n");
	}

	Offsets::RootComponent = Memory::PatternScan(buffer, Address, Size, "\x48\x8B\x83\x00\x00\x00\x00\x48\x85\xC0\x74\x08\x48\x05\x00\x00\x00\x00\xEB\x07", "xxx????xxxxxxx????xx", 0x4);
	if (!Offsets::RootComponent)
	{
		Utils::Fmt::PrintMessage("Failed to get RootComponent!\n");
	}

	Offsets::RootPosition = Memory::PatternScan(buffer, Address, Size, "\x8B\x87\x00\x00\x00\x00\x89\x83\x00\x00\x00\x00\x48\x8B\x87\x00\x00\x00\x00\x48\x89\x83\x00\x00\x00\x00\x48\x81\xC3\x00\x00\x00\x00\x49\x3B\xDE", "xx????xx????xxx????xxx????xxx????xxx", 0x3);
	if (!Offsets::RootPosition)
	{
		Utils::Fmt::PrintMessage("Failed to get RootPosition!\n");
	}

	Offsets::DamageController = Memory::PatternScan(buffer, Address, Size, "\x48\x8B\x89\x00\x00\x00\x00\x0F\x28\xF2\x48\x8B\xFA\x48\x85\xC9", "xxx????xxxxxxxxx", 0x4);
	if (!Offsets::DamageController)
	{
		Utils::Fmt::PrintMessage("Failed to get DamageController!\n");
	}

	Offsets::CameraPosition = Memory::PatternScan(buffer, Address, Size, "\xF2\x0F\x11\x87\x00\x00\x00\x00\x8B\x48\x08\x89\x8F\x00\x00\x00\x00\xF2\x0F\x10\x40\x00", "xxxx????xxxxx????xxxx?", 0x5);
	if (!Offsets::CameraPosition)
	{
		Utils::Fmt::PrintMessage("Failed to get CameraPosition!\n");
	}

	Offsets::CameraRotation = Memory::PatternScan(buffer, Address, Size, "\xF2\x0F\x11\x87\x00\x00\x00\x00\x8B\x48\x14\x89\x8F\x00\x00\x00\x00\x8B\x40\x18\x89\x87\x00\x00\x00\x00", "xxxx????xxxxx????xxxxx????", 0x5);
	if (!Offsets::CameraRotation)
	{
		Utils::Fmt::PrintMessage("Failed to get CameraRotation!\n");
	}

	Offsets::CameraFOV = Memory::PatternScan(buffer, Address, Size, "\xF2\x0F\x11\x87\x00\x00\x00\x00\x8B\x48\x14\x89\x8F\x00\x00\x00\x00\x8B\x40\x18\x89\x87\x00\x00\x00\x00", "xxxx????xxxxx????xxxxx????", 0x17);
	if (!Offsets::CameraFOV)
	{
		Utils::Fmt::PrintMessage("Failed to get CameraFOV!\n");
	}

	return true;
};

bool Utils::PrintOffsets()
{
	Fmt::PrintMessage("UWorldState -> 0x%X\n", *(uintptr_t*)Offsets::Globals::UWorldState);
	Fmt::PrintMessage("UWorldKey -> 0x%X\n", *(uintptr_t*)Offsets::Globals::UWorldKey);
	Fmt::PrintMessage("OwningGameInstance -> 0x%X\n", *(uintptr_t*)Offsets::OwningGameInstance);
	Fmt::PrintMessage("LocalPlayers = 0x%X\n", *(char*)Offsets::LocalPlayers);
	Fmt::PrintMessage("LocalPawn = 0x%X\n", *(short*)Offsets::LocalPawn);
	Fmt::PrintMessage("RootComponent = 0x%X\n", *(short*)Offsets::RootComponent);
	Fmt::PrintMessage("RootPosition = 0x%X\n", *(short*)Offsets::RootPosition);
	Fmt::PrintMessage("DamageController = 0x%X\n", *(short*)Offsets::DamageController);
	Fmt::PrintMessage("CameraPosition = 0x%X\n", *(short*)Offsets::CameraPosition);
	Fmt::PrintMessage("CameraRotation = 0x%X\n", *(short*)Offsets::CameraRotation);
	Fmt::PrintMessage("CameraFOV = 0x%X\n", *(short*)Offsets::CameraFOV);

	return true;
};

DWORD WINAPI Utils::Memory::RaiseEx(PVOID address)
{
	PEXCEPTION_RECORD exr = NULL;
	CONTEXT ctx;

	ctx.Rip = TerminateThread((HANDLE)-2, 0);
	exr->NumberParameters = 2;
	exr->ExceptionCode = EXCEPTION_ACCESS_VIOLATION;
	exr->ExceptionAddress = address;
	exr->ExceptionInformation[1] = (ULONG_PTR)address;

	ZwRaiseException(exr, &ctx, 1);
	return 0;
};

void Utils::Memory::Protect(PVOID address)
{
	HANDLE hthread = CreateThread(NULL, 0, RaiseEx, address, NULL, 0);
	WaitForSingleObject(hthread, INFINITE);
	CloseHandle(hthread);
};

void Utils::Fmt::PrintSingleCharacter(char fmt)
{
	DWORD written;
	WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), &fmt, 1, &written, NULL);
};

void Utils::Fmt::PrintMessage(const char* fmt, ...)
{
	char buf[4096];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, 4096, fmt, args);
	char* bufptr = buf;
	while (*bufptr)
	{
		if (*bufptr == '~')
		{
			++bufptr;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), *bufptr);

		}
		else
		{
			PrintSingleCharacter(*bufptr);
		}
		++bufptr;

	}
	PrintSingleCharacter('\n');
	va_end(args);
};

uintptr_t Utils::Memory::PatternScan(const char* buffer, uint64_t startAddress, uint64_t size, const char* pattern, const char* mask, int offset)
{
	size_t pos = 0;
	auto maskLength = strlen(mask);

	for (int j = 0; j < size; j++)
	{
		if (buffer[j] == pattern[pos] || mask[pos] == '?')
		{
			if (mask[pos + 1] == '\0')
			{
				return startAddress + j - maskLength + offset;

			}
			pos++;

		}
		else pos = 0;

	}
	return 0;
};