#pragma once
#include <vector>
#include <string>
#include "SmartHandleHolder.h"



class FileReader 
	:public SmartHandleHolder
{
public:
	enum class PathAttribute { Directory, File, None };
	using Buffer = std::vector<char>;

public:
	FileReader(const std::wstring file_path, DWORD share_mode, DWORD creation_disposition); // CR: const reference
	~FileReader();

public:
	Buffer read(size_t size)const;
	static PathAttribute get_path_attribute(const std::wstring& path);

public:
	FileReader(const FileReader& other) = delete;
	FileReader(FileReader&& other) = delete;
	FileReader& operator=(FileReader&& other) = delete;
	FileReader& operator=(FileReader& other) = delete;

private:
	static HANDLE get_file_handler(const std::wstring& file_path, DWORD share_mode, DWORD creation_disposition);

};