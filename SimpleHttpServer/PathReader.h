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
	using StringBufferPtr = std::shared_ptr<FileReader::Buffer>;
	using MemberFunctionPathHandler = FileReader::Buffer (PathReader::*)(void) const;
	using HandlersMapping = std::map<FileReader::PathAttribute, MemberFunctionPathHandler>;

public:
	PathReader(const std::wstring& abs_path);
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
	FileReader::Buffer defult_handler() const;
	FileReader::Buffer file_handle()const;
	FileReader::Buffer directory_handle()const;
	
private:
	const std::wstring _abs_path;
};



