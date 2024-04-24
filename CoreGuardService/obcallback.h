#pragma once

#include <ntifs.h>

#ifndef PROCESS_TERMINATE
#define PROCESS_TERMINATE (0x0001)
#endif

#ifndef PROCESS_CREATE_THREAD
#define PROCESS_CREATE_THREAD (0x0002)
#endif

#ifndef PROCESS_VM_OPERATION
#define PROCESS_VM_OPERATION (0x0008)
#endif

#ifndef PROCESS_VM_WRITE
#define PROCESS_VM_WRITE (0x0020)
#endif

#ifndef PROCESS_SUSPEND_RESUME
#define PROCESS_SUSPEND_RESUME (0x0800)
#endif

#ifndef THREAD_TERMINATE
#define THREAD_TERMINATE (0x0001)
#endif

#ifndef THREAD_SUSPEND_RESUME
#define THREAD_SUSPEND_RESUME (0x0002)
#endif

#ifndef THREAD_SET_CONTEXT
#define THREAD_SET_CONTEXT (0x0010)
#endif

#ifndef THREAD_IMPERSONATE
#define THREAD_IMPERSONATE (0x0100)
#endif

#ifndef THREAD_DIRECT_IMPERSONATION
#define THREAD_DIRECT_IMPERSONATION (0x0200)
#endif

#ifndef THREAD_SET_INFORMATION
#define THREAD_SET_INFORMATION (0x0020)
#endif

#ifndef THREAD_SET_LIMITED_INFORMATION
#define THREAD_SET_LIMITED_INFORMATION   (0x0400)
#endif

// The PreOperation callback function
OB_PREOP_CALLBACK_STATUS PreCallback(
    _In_ PVOID RegistrationContext,
    _Inout_ POB_PRE_OPERATION_INFORMATION OperationInformation
);

// Register ObCallbacks
NTSTATUS RegisterObCallbacks(void);

// Destroy ObCallbacks
VOID UnregisterObCallbacks(void);