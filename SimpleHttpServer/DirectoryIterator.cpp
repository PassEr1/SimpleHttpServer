#include "DirectoryIterator.h"
#include <exception>



DirectroyIterator::DirectroyIterator(const std::wstring& path)
	:_hfind(get_find_handler(path)),
	_has_next(TRUE)
{
}

DirectroyIterator::~DirectroyIterator()
{
	FindClose(_hfind); // CR: always try catch in destructor
}

DirectroyIterator::StringBuffer DirectroyIterator::get_next()
{
	StringBuffer next_file_name_to_return(_next_file.cFileName);
	_has_next = FindNextFileW(_hfind, &_next_file); //update data for next use. 
	// CR: check GetLastError to see if this failed because there are no more files or because the dir is empty. Throw exception if its a real error
	return next_file_name_to_return;
}

bool DirectroyIterator::has_next()const
{
	return _has_next;
}

HANDLE DirectroyIterator::get_find_handler(const std::wstring& path)
{
	HANDLE h_find = FindFirstFileW(path.c_str(), &_next_file);
	
	if (h_find == INVALID_HANDLE_VALUE)
	{
		throw std::exception();
	}
	
	return h_find;
}
