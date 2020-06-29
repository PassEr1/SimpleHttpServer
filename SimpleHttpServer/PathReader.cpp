#include "PathReader.h"
#include <sstream>
#include <string>
#include "DirectoryIterator.h"
#include "Utils.h"

const PathReader::StringBuffer PathReader::_default_empty_data = StringBuffer();

PathReader::PathReader(const std::wstring& abs_path, DWORD max_size_for_data)
:_max_size_for_data(max_size_for_data),
_abs_path(abs_path)		
{
}

PathReader::~PathReader()
{

}

PathReader::HandlersMapping PathReader::get_path_handlers()const
{
	HandlersMapping mapping_handlers;
	mapping_handlers.insert({ FileReader::PathAttribute::File , &PathReader::file_handle});
	mapping_handlers.insert({ FileReader::PathAttribute::Directory, &PathReader::directory_handle});
	mapping_handlers.insert({ FileReader::PathAttribute::None, &PathReader::defualt_handler});
	return mapping_handlers;
}

HANDLE PathReader::get_file_hanler()const // for CR maker: should I return here a unique ptr that usese Win32 API "CloseHandle" function ?
{
	HANDLE hfile = CreateFileW(_abs_path.c_str(),
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	return hfile;
}

PathReader::StringBuffer PathReader::defualt_handler() const
{
	return _default_empty_data;
}

PathReader::StringBuffer PathReader::file_handle() const
{
	static const LPOVERLAPPED DONT_USE_OVERLLAPED = NULL;
	FileReader file_reader(_abs_path, FILE_SHARE_READ, OPEN_EXISTING);
	return file_reader.read(_max_size_for_data);
}

PathReader::StringBuffer PathReader::directory_handle() const
{
	static unsigned int SIZE_OF_NEW_LINE_CHARACTER = 2;
	unsigned int total_bytes_read = 0;
	unsigned long append_offset = 0;
	unsigned long size_going_to_be_written = 0;
	std::wostringstream path_builder;
	StringBuffer buffer(_max_size_for_data);

	path_builder << _abs_path << L"\\*";
	DirectoryIterator iterator(path_builder.str());

	while (iterator.has_next() && (total_bytes_read < _max_size_for_data))
	{
		std::wstring file_name = iterator.get_next();
		size_going_to_be_written = 
			file_name.size() * sizeof(WCHAR) 
			+ SIZE_OF_NEW_LINE_CHARACTER;

		if (size_going_to_be_written + total_bytes_read > _max_size_for_data)
		{
			break;
		}

		append_offset = total_bytes_read;
		add_string_to_vector(file_name, buffer, append_offset);

		append_offset = total_bytes_read + file_name.size() * sizeof(WCHAR);
		add_string_to_vector(L"\n", buffer, append_offset);
		total_bytes_read += size_going_to_be_written;
	
	}

	buffer.resize(total_bytes_read);
	return buffer;
}

PathReader::StringBufferPtr PathReader::read_now() const
{
	FileReader::PathAttribute path_attribute = FileReader::get_path_attribute(_abs_path);
	HandlersMapping handlers_mapping = get_path_handlers();
	MemberFunctionPathHandler selected_handler = handlers_mapping[path_attribute];
	StringBuffer result_buffer = (this->*selected_handler)();
	return std::make_shared<StringBuffer>(result_buffer);
}

