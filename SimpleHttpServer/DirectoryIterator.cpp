#include "DirectoryIterator.h"
#include <exception>
#include "Utils.h"



DirectoryIterator::DirectoryIterator(const std::wstring& path)
	:_hfind(get_find_handle(path)),
	_has_next(TRUE)
{
}

DirectoryIterator::~DirectoryIterator()
{
	try { // CR: new line before {
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
	THROW_IF_NOT(GetLastError() != ERROR_NO_MORE_FILES); // CR: weird logic here. think about it...

	return next_file_name_to_return;
}

bool DirectoryIterator::has_next()const
{
	return _has_next;
}

HANDLE DirectoryIterator::get_find_handle(const std::wstring& path)
{
	HANDLE h_find = FindFirstFileW(path.c_str(), &_next_file);
	
	if (h_find == INVALID_HANDLE_VALUE)
	{
		throw std::exception();
	}
	
	return h_find;
}
