#include "ObCallback.h"
#include "globals.h"

PVOID gRegistrationHandle = NULL;

// Implementation of the PreCallback function
OB_PREOP_CALLBACK_STATUS PreCallback(
    _In_ PVOID RegistrationContext,
    _Inout_ POB_PRE_OPERATION_INFORMATION OpInfo
) {
    UNREFERENCED_PARAMETER(RegistrationContext);

    if (OpInfo->ObjectType == *PsProcessType) {
        PEPROCESS process = (PEPROCESS)OpInfo->Object;
        HANDLE pid = PsGetProcessId(process);

        if ((HANDLE)userland_pid == pid) {
            if (OpInfo->Operation == OB_OPERATION_HANDLE_CREATE ||
                OpInfo->Operation == OB_OPERATION_HANDLE_DUPLICATE) {
                if (OpInfo->KernelHandle != TRUE) {
                    ACCESS_MASK bits_to_clear = PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | 
                                                PROCESS_VM_WRITE | PROCESS_TERMINATE | PROCESS_SUSPEND_RESUME;
                    OpInfo->Parameters->CreateHandleInformation.DesiredAccess &= ~bits_to_clear;
                    OpInfo->Parameters->DuplicateHandleInformation.DesiredAccess &= ~bits_to_clear;
                }
            }
        }
    }
    else if (OpInfo->ObjectType == *PsThreadType) {
        PETHREAD thread = (PETHREAD)OpInfo->Object;
        HANDLE pid = PsGetThreadProcessId(thread);

        if ((HANDLE)userland_pid == pid) {
            if (OpInfo->Operation == OB_OPERATION_HANDLE_CREATE) {
                if (OpInfo->KernelHandle != TRUE) {
					ACCESS_MASK bits_to_clear = THREAD_TERMINATE | THREAD_SUSPEND_RESUME | THREAD_SET_CONTEXT |
                                                THREAD_IMPERSONATE | THREAD_DIRECT_IMPERSONATION | 
                                                THREAD_SET_INFORMATION | THREAD_SET_LIMITED_INFORMATION;
                    OpInfo->Parameters->CreateHandleInformation.DesiredAccess &= ~bits_to_clear;
                    OpInfo->Parameters->DuplicateHandleInformation.DesiredAccess &= ~bits_to_clear;
                }
            }
        }
    }

    return OB_PREOP_SUCCESS;
}

// Function to set up and register ObCallbacks
NTSTATUS RegisterObCallbacks(void) {
    OB_OPERATION_REGISTRATION operations[2] = { 0 };
    OB_CALLBACK_REGISTRATION callbackRegistration = { 0 };
    NTSTATUS status = STATUS_UNSUCCESSFUL;

    operations[0].ObjectType = PsProcessType;
    operations[0].Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
    operations[0].PreOperation = PreCallback;
    operations[0].PostOperation = NULL;

    operations[1].ObjectType = PsThreadType;
    operations[1].Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
    operations[1].PreOperation = PreCallback;
    operations[1].PostOperation = NULL;

    callbackRegistration.Version = OB_FLT_REGISTRATION_VERSION;
    callbackRegistration.OperationRegistrationCount = sizeof(operations) / sizeof(operations[0]);
    callbackRegistration.RegistrationContext = NULL;
    callbackRegistration.OperationRegistration = operations;
    RtlInitUnicodeString(&callbackRegistration.Altitude, L"320000");

    status = ObRegisterCallbacks(&callbackRegistration, &gRegistrationHandle);
    if (!NT_SUCCESS(status)) {
        // Handle error
        DbgPrint("ObRegisterCallbacks failed: %lu\n", status);
    }

    return status;
}

VOID UnregisterObCallbacks(void) {
    // Unregister the callbacks
    if (gRegistrationHandle) {
        ObUnRegisterCallbacks(gRegistrationHandle);
        gRegistrationHandle = NULL;
    }
}