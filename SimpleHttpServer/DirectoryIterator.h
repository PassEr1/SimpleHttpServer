#pragma once
#include <string>
#include <vector>
#include <windows.h>


class DirectoryIterator 
{

public:
	DirectoryIterator(const std::wstring& path);
	~DirectoryIterator();

	bool has_next()const;
	std::wstring get_next();

public:
	DirectoryIterator(const DirectoryIterator& other) = delete;
	DirectoryIterator( DirectoryIterator&& other) = delete;
	DirectoryIterator& operator=(const DirectoryIterator& other) = delete;
	DirectoryIterator& operator=(DirectoryIterator&& other) = delete;

private:
	HANDLE get_find_handle(const std::wstring& path); // CR: should be static

private:
	const HANDLE _hfind;
	WIN32_FIND_DATAW _next_file;
	
	bool _has_next;

};
