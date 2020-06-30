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
	/*
	For CR maker: making this function, static enforced me to pass it a pointer to my first-file's data.
				  And this is because we have to "know" the first file by calling the function "FindFirst" of win api.
				  why do you think this shoul be static though?
	*/
	static HANDLE get_find_handle(const std::wstring& path, PWIN32_FIND_DATAW first_file_found_ptr); 


private:
	const HANDLE _hfind;
	WIN32_FIND_DATAW _next_file;
	
	bool _has_next;

};
