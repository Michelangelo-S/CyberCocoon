#pragma once
#include "minifilter.h"
#include "globals.h"
#include "minifilterport.h"
#include "obcallback.h"

NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT driver_object,
	_In_ PUNICODE_STRING registry_path
);

VOID DriverUnload(
	_In_ PDRIVER_OBJECT driver_object
);