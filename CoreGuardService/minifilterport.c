#include "minifilterport.h"
#include "../common/common.h"

static PFLT_FILTER mini_filter_handle;
static PFLT_PORT server_com_port;
static PFLT_PORT client_com_port;

NTSTATUS
MiniFilterPortOpen(
	_In_ PFLT_FILTER miniflt_handle
)
{
	UNICODE_STRING port_name = { 0 };
	PSECURITY_DESCRIPTOR security_descriptor = { 0 };
	OBJECT_ATTRIBUTES object_attributes = { 0 };
	
	NTSTATUS status = FltBuildDefaultSecurityDescriptor(
		&security_descriptor,
		FLT_PORT_ALL_ACCESS
	);
	if (!NT_SUCCESS(status)) {
		DbgPrint("FltBuildDefaultSecurityDescriptor failed (status: 0x%x)\n", status);
		MiniFilterPortClose();
		return status;
	}

	RtlInitUnicodeString(&port_name, TEXT(COM_PORT_NAME));
	InitializeObjectAttributes(
		&object_attributes, &port_name, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, security_descriptor
	);

	status = FltCreateCommunicationPort(
		miniflt_handle,
		&server_com_port,
		&object_attributes,
		NULL,
		MiniFilterConnectNotifyCallback,
		MiniFilterDisconnectNotifyCallback,
		NULL,
		1
	);
	if (!NT_SUCCESS(status)) {
		DbgPrint("FltCreateCommunicationPort failed (status: 0x%x)\n", status);
		MiniFilterPortClose();
		return status;
	}

	mini_filter_handle = miniflt_handle;
	return STATUS_SUCCESS;
}

VOID
MiniFilterPortClose()
{
	if (server_com_port) {
		FltCloseCommunicationPort(server_com_port);
	}
}

NTSTATUS
MiniFilterConnectNotifyCallback(
	_In_ PFLT_PORT connected_client_port,
	_In_ PVOID server_cookie,
	_In_ PVOID connection_context,
	_In_ ULONG connection_context_size,
	_Out_ PVOID* connection_port_cookie
)
{
	UNREFERENCED_PARAMETER(server_cookie);
	UNREFERENCED_PARAMETER(connection_context);
	UNREFERENCED_PARAMETER(connection_context_size);
	UNREFERENCED_PARAMETER(connection_port_cookie);

	DbgPrint(
		"Client with pid %u connected\n",
		PtrToUint(PsGetCurrentProcessId())
	);

	userland_pid = PsGetCurrentProcessId();
	client_com_port = connected_client_port;

	return STATUS_SUCCESS;
}

VOID
MiniFilterDisconnectNotifyCallback(
	_In_ PVOID connection_cookie
)
{
	UNREFERENCED_PARAMETER(connection_cookie);

	DbgPrint(
		"Client with pid %u disconnected\n",
		PtrToUint(PsGetCurrentProcessId())
	);

	userland_pid = 0;
}

NTSTATUS
MinifltPortSendMessage(
	_In_ PVOID send_data,
	_In_ ULONG send_data_size,
	_Out_opt_ PVOID recv_buffer,
	_In_ ULONG recv_buffer_size,
	_Out_ PULONG written_bytes_to_recv_buffer
)
{
	NTSTATUS status = STATUS_SUCCESS;

	if (recv_buffer) {
		status = FltSendMessage(
			mini_filter_handle,
			&client_com_port,
			send_data,
			send_data_size,
			recv_buffer,
			&recv_buffer_size,
			NULL
		);

		*written_bytes_to_recv_buffer = recv_buffer_size;
	}
	else {
		LARGE_INTEGER timeout;
		timeout.QuadPart = 0;

		status = FltSendMessage(
			mini_filter_handle,
			&client_com_port,
			send_data,
			send_data_size,
			NULL,
			NULL,
			&timeout
		);

		*written_bytes_to_recv_buffer = 0;
	}

	return status;
}