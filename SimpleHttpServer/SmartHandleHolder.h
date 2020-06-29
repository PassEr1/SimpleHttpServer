#pragma once
#include <Windows.h>

class SmartHandleHolder
{
public:
	SmartHandleHolder(HANDLE handle);
	~SmartHandleHolder();

public: // CR: If you change to inheritance, this should be protected
	HANDLE data()const;

public:
	SmartHandleHolder(const SmartHandleHolder& other) = delete;
	SmartHandleHolder(SmartHandleHolder&& other) = delete;
	SmartHandleHolder& operator=(SmartHandleHolder&& other) = delete;
	SmartHandleHolder& operator=(SmartHandleHolder& other) = delete;


private:
	const HANDLE _handle;
};
