#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include "Utils.h"
#include "FileReader.h"

static DWORD DEFAULT_READ_SIZE = 2028;

class PathReader
{
public:
	using StringBuffer = std::vector<char>; // CR: just use the same buffer from FileReader
	using StringBufferPtr = std::shared_ptr<StringBuffer>;
	using MemberFunctionPathHandler = StringBuffer (PathReader::*)(void) const;
	using HandlersMapping = std::map<FileReader::PathAttribute, MemberFunctionPathHandler>;

public:
	// CR: for simplicity, doint support the max size. just return all the data.
	PathReader(const std::wstring& abs_path, DWORD max_size_for_data = DEFAULT_READ_SIZE);
	~PathReader();

public:
	StringBufferPtr read_now()const;

public:
	PathReader(const PathReader& other)=delete;
	PathReader& operator=(const PathReader& other)=delete;
	PathReader(PathReader&& other) = delete;
	PathReader& operator=(PathReader&& other) = delete;

private:
	HandlersMapping get_path_handlers() const;
	HANDLE get_file_hanler() const; // CR: unused... remove this
	PathReader::StringBuffer defualt_handler() const;
	StringBuffer file_handle()const;
	StringBuffer directory_handle()const;
	
private:
	const std::wstring _abs_path;
	const unsigned int _max_size_for_data;
	static const StringBuffer _default_empty_data; // CR: this can be made static in the function, no need for it here.
};



