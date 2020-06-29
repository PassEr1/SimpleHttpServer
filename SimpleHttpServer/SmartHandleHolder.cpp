#include "SmartHandleHolder.h"

SmartHandleHolder::SmartHandleHolder(HANDLE handle)
	:_handle(handle)
{
}

SmartHandleHolder::~SmartHandleHolder()
{
	try { // CR: new line before {
		CloseHandle(_handle);
	}

	catch (...)
	{
	}

}

HANDLE SmartHandleHolder::data() const
{
	return _handle;
}
