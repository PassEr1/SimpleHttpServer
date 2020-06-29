#include "Utils.h"
#include <signal.h>
#include <iostream>
#include <vector>


size_t fnGetWStringLength(PCWSTR szString, size_t maxSize)
{
	size_t result;
	HRESULT status = StringCbLengthW(szString, maxSize, &result);
	if (S_OK == status)
		return result;
	else
		return 0;
}

size_t fnGetWStringSize(PCWSTR szString, size_t maxSize)
{
	return fnGetWStringLength(szString, maxSize) * sizeof(WCHAR);
}

LPVOID fnAllocate(SIZE_T cbBytes)
{
	return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cbBytes);
}

void fnFree(LPVOID lpPtr)
{
	HeapFree(GetProcessHeap(), 0, lpPtr);
}

PWSTR convertCSTR(PCSTR sString, DWORD cbBytesInString)
{
	PWSTR sNewString = static_cast<PWSTR>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cbBytesInString * 2));
	if (NULL == sNewString)
	{
		return NULL;
	}

	for (size_t i = 0; i < cbBytesInString; i++)
	{
		sNewString[i] = (WCHAR)sString[i];
	}
	return sNewString;
}

void add_string_to_vector(const std::wstring& str, std::vector<char>& buffer, unsigned int offset)
{
	CopyMemory(
		buffer.data() + offset,
		str.data(),
		str.size() * sizeof(WCHAR));
}


void add_string_to_vector(const std::string& str, std::vector<char>& buffer, unsigned int offset)
{
	CopyMemory(
		buffer.data() + offset,
		str.data(),
		str.size());
}
