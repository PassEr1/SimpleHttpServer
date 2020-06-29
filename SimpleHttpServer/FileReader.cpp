#include "FileReader.h"


HANDLE FileReader::get_file_hanler(const std::wstring file_path, DWORD share_mode, DWORD creation_disposition)
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
	:_file_handler(get_file_hanler(file_path, share_mode, creation_disposition))
{
}

FileReader::~FileReader()
{
}

FileReader::Buffer FileReader::read(size_t size) const
{
	// CR: unlike write... read either succeeds with all the bytes you asked for (or less if the file 
	// is shorter) or fails. no need for a loop here. 
	// Even if you did need a loop, there is no error handling here! Almost all functions where you call a 
	// winapi function should have an error flow with an exception...

	static const LPOVERLAPPED DONT_USE_OVERLLAPED = NULL;
	unsigned long total_bytes_read = 0;
	DWORD bytes_read = 0;
	bool status = TRUE;
	Buffer buffer(size);

	while (
		(total_bytes_read < size)
		&& status)
	{
		status = ReadFile(
			_file_handler.data(),
			buffer.data() + total_bytes_read,
			size,
			&bytes_read,
			DONT_USE_OVERLLAPED);

		if (!status || !bytes_read)
		{
			break;
		}
		total_bytes_read += bytes_read;
	}

	buffer.resize(total_bytes_read);
	return buffer; // CR: this buffer is copied. also, put the using somewhere more useful (buffer.hpp maybe) so that you wont need to redefine it
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
	else // is a file
	{
		return PathAttribute::File;
	}
}
