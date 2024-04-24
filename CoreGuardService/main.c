#include "main.h"

NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT driver_object,
	_In_ PUNICODE_STRING registry_path
)
{
	UNREFERENCED_PARAMETER(registry_path);

	NTSTATUS status;
	PFLT_FILTER filter_handle = NULL;
	status = MiniFilterRegister(driver_object, &filter_handle);
	if (!NT_SUCCESS(status)) {
		return status;
	}

	// Initialize the filter port
	status = MiniFilterPortOpen(filter_handle);
	if (!NT_SUCCESS(status)) {
		DbgPrint("MiniFilterPortOpen failed (status: 0x%x)\n", status);
		MiniFilterPortClose();
		MiniFilterUnregister();
		return status;
	}

	// Register obcallbacks
	RegisterObCallbacks();

	// Set a driver unload routine
	driver_object->DriverUnload = DriverUnload;

	return STATUS_SUCCESS;
}

VOID DriverUnload(
	_In_ PDRIVER_OBJECT driver_object
)
{
	UNREFERENCED_PARAMETER(driver_object);

	// Destroy comport, minifilter & obcallback
	MiniFilterPortClose();
	MiniFilterUnregister();
	UnregisterObCallbacks();
}