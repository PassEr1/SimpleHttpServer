#include "SmartHandleHolder.h"

SmartHandleHolder::SmartHandleHolder(HANDLE handle)
	:_handle(handle)
{
}

SmartHandleHolder::~SmartHandleHolder()
{
	try {
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
