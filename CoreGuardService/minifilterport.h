#pragma once

#include <fltKernel.h>

#include "globals.h"

NTSTATUS
MiniFilterPortOpen(
	_In_ PFLT_FILTER flt_handle
);

VOID
MiniFilterPortClose();

NTSTATUS
MiniFilterConnectNotifyCallback(
	_In_ PFLT_PORT client_port,
	_In_ PVOID server_cookie,
	_In_ PVOID connection_context,
	_In_ ULONG connection_context_size,
	_Out_ PVOID* connection_port_cookie
);

VOID
MiniFilterDisconnectNotifyCallback(
	_In_ PVOID connection_cookie
);

NTSTATUS
MinifltPortSendMessage(
	_In_ PVOID send_data,
	_In_ ULONG send_data_size,
	_Out_opt_ PVOID recv_buffer,
	_In_ ULONG recv_buffer_size,
	_Out_ PULONG written_bytes_to_recv_buffer
);