#pragma once
#include <Windows.h>

class SmartHandleHolder
{

public:
	SmartHandleHolder(HANDLE handle);
	~SmartHandleHolder();

	// for CR maker: what if someone would like to use it with composition and hold muliple "Handlers" for instance ?
	//doesnt is should be public?
public: 
	HANDLE get_handle()const;

public:
	SmartHandleHolder(const SmartHandleHolder& other) = delete;
	SmartHandleHolder(SmartHandleHolder&& other) = delete;
	SmartHandleHolder& operator=(SmartHandleHolder&& other) = delete;
	SmartHandleHolder& operator=(SmartHandleHolder& other) = delete;


private:
	const HANDLE _handle;
};
