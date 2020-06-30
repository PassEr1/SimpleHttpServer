#include "DirectoryIterator.h"
#include <exception>
#include "Utils.h"



DirectoryIterator::DirectoryIterator(const std::wstring& path)
	:_hfind(DirectoryIterator::get_find_handle(path, &_next_file)),
	_has_next(TRUE)
{
}

DirectoryIterator::~DirectoryIterator()
{
	try 
	{ 
		FindClose(_hfind);
	}
	catch (...)
	{
	}
}

std::wstring DirectoryIterator::get_next()
{
	std::wstring next_file_name_to_return(_next_file.cFileName);
	_has_next = FindNextFileW(_hfind, &_next_file); //update data for next use. 
	if (!_has_next)
	{
		THROW_IF_NOT(GetLastError() == ERROR_NO_MORE_FILES);
	}
	return next_file_name_to_return;
}

bool DirectoryIterator::has_next()const
{
	return _has_next;
}

HANDLE DirectoryIterator::get_find_handle(const std::wstring& path, PWIN32_FIND_DATAW pfirst_file_found)
{
	static const std::wstring GLOB_FORAMT_ALL_FILES(L"/*");
	HANDLE h_find = FindFirstFileW((path + GLOB_FORAMT_ALL_FILES).c_str(), pfirst_file_found);
	
	if (h_find == INVALID_HANDLE_VALUE)
	{
		throw std::exception();
	}
	
	return h_find;
}
