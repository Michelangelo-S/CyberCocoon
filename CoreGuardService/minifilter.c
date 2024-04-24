#include "minifilter.h"

FLT_OPERATION_REGISTRATION operations[] = {
	{
		IRP_MJ_CREATE,
		0,
		NULL,
		&MiniFilterCreatePostRoutine,
		NULL
	},
	{
		IRP_MJ_OPERATION_END
	}
};

FLT_REGISTRATION registration = {
	sizeof(FLT_REGISTRATION),          // size
	FLT_REGISTRATION_VERSION,          // version
	0,                                 // flags
	NULL,                              // context registration
	&operations,                        // operation registration
	&MiniFilterUnloadRoutine, // filter unload callback
	NULL,                              // instance setup callback
	NULL,                              // instance query teardown callback
	NULL,                              // instance teardown start callback
	NULL,                              // instance teardown complete callback
	NULL,                              // generate file name callback
	NULL,                              // normalize name component callback
	NULL,                              // normalize context cleanup callback
	NULL,                              // transaction notification callback
	NULL,                              // normalize name component ex callback
	NULL                               // section notification callback
};

static PFLT_FILTER mini_filter_handle;

NTSTATUS MiniFilterRegister(
	_In_ PDRIVER_OBJECT driver_object,
	_Out_ PFLT_FILTER* filter_handle
) {
	NTSTATUS status = STATUS_SUCCESS;

	// register the minifilter
	status = FltRegisterFilter(
		driver_object,
		&registration,
		&mini_filter_handle
	);
	if (!NT_SUCCESS(status)) {
		DbgPrint("FltRegisterFilter failed (status: 0x%x)\n", status);
		return status;
	}

	// start filtering
	status = FltStartFiltering(
		mini_filter_handle
	);
	if (!NT_SUCCESS(status)) {
		DbgPrint("FltStartFiltering failed (status: 0x%x)\n", status);
		MiniFilterUnregister();
		return status;
	}

	*filter_handle = mini_filter_handle;

	return status;
}

NTSTATUS MiniFilterUnregister() {
	if (mini_filter_handle) {
		FltUnregisterFilter(mini_filter_handle);
	}
}

NTSTATUS
MiniFilterUnloadRoutine(
	_In_ FLT_FILTER_UNLOAD_FLAGS flags
)
{
	UNREFERENCED_PARAMETER(flags);

	MiniFilterUnregister();

	return STATUS_SUCCESS;
}

FLT_POSTOP_CALLBACK_STATUS
MiniFilterCreatePostRoutine(
	_Inout_      PFLT_CALLBACK_DATA callback_data,
	_In_         PCFLT_RELATED_OBJECTS flt_object,
	_In_opt_     PVOID completion_context,
	_In_         FLT_POST_OPERATION_FLAGS flags
)
{
	UNREFERENCED_PARAMETER(flt_object);
	UNREFERENCED_PARAMETER(completion_context);
	UNREFERENCED_PARAMETER(flags);

	PFLT_FILE_NAME_INFORMATION name_info = NULL;
	NTSTATUS status;

	// query the file path
	status = FltGetFileNameInformation(
		callback_data,
		FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT,
		&name_info
	);
	if (!NT_SUCCESS(status)) {
		if (name_info) {
			FltReleaseFileNameInformation(name_info);
		}

		return FLT_POSTOP_FINISHED_PROCESSING;
	}

	status = FltParseFileNameInformation(name_info);
	if (!NT_SUCCESS(status)) {
		if (name_info) {
			FltReleaseFileNameInformation(name_info);
		}

		return FLT_POSTOP_FINISHED_PROCESSING;
	}

	// get the file name substring
	LPWSTR file_name = wcsrchr(name_info->Name.Buffer, L'\\');
	if (!file_name) {
		file_name = name_info->Name.Buffer;
	}

	FILE_HANDLE_REQUEST request = { 0 };
	FILE_HANDLE_REPLY reply = { 0 };
	ULONG bytes_written = 0;

	// Copy the path of the requested file
	wcscpy_s(request.path, ARRAYSIZE(request.path), name_info->Name.Buffer);
	// Set the process that requested the handle
	request.process = PsGetCurrentProcessId();

	// Ask the client what it wants to do with the file
	status = MinifltPortSendMessage(&request, sizeof(request), &reply, sizeof(reply), &bytes_written);
	// If client decided to block access, then return access denied
	if (NT_SUCCESS(status) && bytes_written > 0 && reply.block) {
		callback_data->IoStatus.Status = STATUS_ACCESS_DENIED;
	}

	FltReleaseFileNameInformation(name_info);

	return FLT_POSTOP_FINISHED_PROCESSING;
}