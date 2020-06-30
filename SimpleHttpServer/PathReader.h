#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include "Utils.h"
#include "FileReader.h"

static DWORD DEFAULT_READ_SIZE_BYTES = 1024 * 1024;

class PathReader
{
public:
	using MemberFunctionPathHandler = FileReader::BufferPtr (PathReader::*)(void) const;
	using HandlersMapping = std::map<FileReader::PathAttribute, MemberFunctionPathHandler>;

public:
	PathReader(const std::wstring& abs_path);
	~PathReader();

public:
	FileReader::BufferPtr read_now()const;

public:
	PathReader(const PathReader& other)=delete;
	PathReader& operator=(const PathReader& other)=delete;
	PathReader(PathReader&& other) = delete;
	PathReader& operator=(PathReader&& other) = delete;

private:
	HandlersMapping get_path_handlers() const;
	FileReader::BufferPtr defult_handler() const;
	FileReader::BufferPtr file_handle()const;
	FileReader::BufferPtr directory_handle()const;
	
private:
	const std::wstring _abs_path;
};



