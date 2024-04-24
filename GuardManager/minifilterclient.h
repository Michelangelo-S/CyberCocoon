#pragma once
#include <Windows.h>
#include <fltUser.h>
#include <format>
#include <exception>

#include "../common/common.h"

struct DrvToUserMessage{

    FILTER_MESSAGE_HEADER header;
    FILE_HANDLE_REQUEST data;

};

struct UserToDrvMessage {

    FILTER_REPLY_HEADER header;
    FILE_HANDLE_REPLY data;

};

class MiniFilterClient
{
public:
    MiniFilterClient();

    ~MiniFilterClient();

    bool Initialize(std::wstring& port_name);

    std::unique_ptr<DrvToUserMessage> ReceiveMessage();

    bool SendReply(UserToDrvMessage* reply);

    const std::string& GetErrorMessage() const {
        return error_message;
    }

private:
	HANDLE port_handle;
	constexpr static size_t EXCEPTION_MSG_SIZE = 128;
    std::string error_message;
};

