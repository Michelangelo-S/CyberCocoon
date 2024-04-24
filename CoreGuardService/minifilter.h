#pragma once
#include <fltKernel.h>

#include "minifilterport.h"

#include "../common/common.h" 

NTSTATUS MiniFilterRegister(
	_In_ PDRIVER_OBJECT driver_object,
	_Out_ PFLT_FILTER* filter_handle
);

NTSTATUS MiniFilterUnregister();

FLT_POSTOP_CALLBACK_STATUS
MiniFilterCreatePostRoutine(
	_Inout_      PFLT_CALLBACK_DATA Data,
	_In_         PCFLT_RELATED_OBJECTS FltObjects,
	_In_opt_     PVOID CompletionContext,
	_In_         FLT_POST_OPERATION_FLAGS Flags
);

NTSTATUS
MiniFilterUnloadRoutine(
	_In_ FLT_FILTER_UNLOAD_FLAGS flags
);