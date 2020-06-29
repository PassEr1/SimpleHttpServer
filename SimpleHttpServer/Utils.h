#pragma once
#include <Windows.h>
#include <strsafe.h>
#include <vector>
#include <string>
#include <exception>

#define THROW_IF_NOT(condition)\
	if(!(condition))\
		throw std::exception();\


size_t fnGetWStringLength(PCWSTR szString, size_t maxSize);

size_t fnGetWStringSize(PCWSTR szString, size_t maxSize);

LPVOID fnAllocate(SIZE_T cbBytes);

void fnFree(LPVOID);

PWSTR convertCSTR(PCSTR sString, DWORD cbBytesInString);



void add_string_to_vector(const std::string& str, std::vector<char>& buffer, unsigned int offset);

void add_string_to_vector(const std::wstring& str, std::vector<char>& buffer, unsigned int offset);

class SmartHandleHolder
{
public:
	SmartHandleHolder(HANDLE handle);
	~SmartHandleHolder();

public:
	HANDLE data()const;

public:
	SmartHandleHolder(const SmartHandleHolder& other) = delete;
	SmartHandleHolder(SmartHandleHolder&& other) = delete;
	SmartHandleHolder& operator=(SmartHandleHolder&& other) = delete;
	SmartHandleHolder& operator=(SmartHandleHolder& other) = delete;


private:
	HANDLE _handle;
};


class FileReader {
public:
	using Buffer = std::vector<char>;

	FileReader(const std::wstring file_path, DWORD share_mode, DWORD creation_disposition);
	~FileReader();

public:
	Buffer read(size_t size)const;

public:
	FileReader(const FileReader& other) = delete;
	FileReader(FileReader&& other) = delete;
	FileReader& operator=(FileReader&& other) = delete;
	FileReader& operator=(FileReader& other) = delete;

private:
	HANDLE get_file_hanler(const std::wstring file_path, DWORD share_mode, DWORD creation_disposition);

private:
	SmartHandleHolder _file_handler;
};