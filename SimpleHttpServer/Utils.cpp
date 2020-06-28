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

SmartHandleHolder::SmartHandleHolder(HANDLE handle)
	:_handle(handle)
{
}

SmartHandleHolder::~SmartHandleHolder()
{
	try {
			CloseHandle(_handle);
	}

	catch (...)
	{
	}
	
}

HANDLE SmartHandleHolder::data()const 
{
	return _handle;
}

HANDLE FileReader::get_file_hanler(const std::wstring file_path) // for CR maker: should I return here a unique ptr that usese Win32 API "CloseHandle" function ?
{
	HANDLE hfile = CreateFileW(file_path.c_str(),
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	return hfile;
}

FileReader::FileReader(const std::wstring file_path)
:_file_handler(get_file_hanler(file_path))
{
}

FileReader::~FileReader()
{
}

FileReader::Buffer FileReader::read(size_t size) const
{
	static const LPOVERLAPPED DONT_USE_OVERLLAPED = NULL;
	unsigned long total_bytes_read = 0;
	DWORD bytes_read = 0;
	bool status=TRUE;
	Buffer buffer(size);

	while (
		(total_bytes_read < size)
		&& status)
	{
		status = ReadFile(
			_file_handler.data(),
			buffer.data() + total_bytes_read,
			size,
			&bytes_read,
			DONT_USE_OVERLLAPED);

		if (!status || !bytes_read)
		{
			break;
		}
		total_bytes_read += bytes_read;
	}
	buffer.resize(total_bytes_read);

	return buffer;
}
