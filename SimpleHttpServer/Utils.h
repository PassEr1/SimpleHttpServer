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

