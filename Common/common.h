#pragma once
#define COM_PORT_NAME "\\CoreGuardServicePort"
#define BUFFER_SIZE 4096

typedef struct _FILE_HANDLE_REQUEST {

	wchar_t path[BUFFER_SIZE / sizeof(wchar_t)];
	unsigned __int32 process;

} FILE_HANDLE_REQUEST, *PFILE_HANDLE_REQUEST;

typedef struct _FILE_HANDLE_REPLY {

	unsigned __int32 block;

} FILE_HANDLE_REPLY, *PFILE_HANDLE_REPLY;