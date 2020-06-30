#include "FileReader.h"
#include "Utils.h"

HANDLE FileReader::get_file_handler(const std::wstring& file_path, DWORD share_mode, DWORD creation_disposition)
{
	HANDLE hfile = CreateFileW(
		file_path.c_str(),
		GENERIC_READ,
		share_mode,
		NULL,
		creation_disposition,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	// CR: what about checking for errors...
	return hfile;
}

FileReader::FileReader(const std::wstring file_path, DWORD share_mode, DWORD creation_disposition)
:SmartHandleHolder(get_file_handler(file_path, share_mode, creation_disposition))
{
}

FileReader::~FileReader()
{
}

FileReader::Buffer FileReader::read(size_t size) const
{
	static const LPOVERLAPPED DONT_USE_OVERLLAPED = NULL;
	unsigned long total_bytes_read = 0;
	DWORD bytes_read = 0;
	bool status = TRUE;
	Buffer buffer(size);

	status = ReadFile(
		this->get_handle(), //for expressivity sake
		buffer.data() + total_bytes_read,
		size,
		&bytes_read,
		DONT_USE_OVERLLAPED);

	THROW_IF_NOT(status);
	buffer.resize(total_bytes_read);
	return buffer;
}

FileReader::PathAttribute FileReader::get_path_attribute(const std::wstring& path)
{
	DWORD attribute_identifer = GetFileAttributesW(path.c_str());
	if (INVALID_FILE_ATTRIBUTES == attribute_identifer)
	{
		return PathAttribute::None; 
	}

	else if (FILE_ATTRIBUTE_DIRECTORY & attribute_identifer)
	{
		return PathAttribute::Directory;
	}
	else
	{
		return PathAttribute::File;
	}
}
