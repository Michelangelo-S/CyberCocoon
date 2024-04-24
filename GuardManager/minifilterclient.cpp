#include "minifilterclient.h"

bool MiniFilterClient::Initialize(std::wstring& port_name) {
    HRESULT result = FilterConnectCommunicationPort(
        port_name.c_str(),
        0,
        nullptr,
        0,
        nullptr,
        &port_handle
    );

    if (IS_ERROR(result)) {
        std::string exception_msg(EXCEPTION_MSG_SIZE, L'\0');
        std::format_to_n(exception_msg.data(), exception_msg.size(),
            L"FilterConnectCommunicationPort failed (0x{:x})", result);
        error_message = exception_msg;
        return false;
    }

    return true;
}

std::unique_ptr<DrvToUserMessage> MiniFilterClient::ReceiveMessage() {
    auto new_message = std::make_unique<DrvToUserMessage>();

    HRESULT result = FilterGetMessage(
        port_handle,
        &new_message->header,
        sizeof(DrvToUserMessage),
        nullptr
    );

    if (IS_ERROR(result)) {
        std::string exception_msg(EXCEPTION_MSG_SIZE, L'\0');
        std::format_to_n(exception_msg.data(), exception_msg.size(),
            L"FilterGetMessage failed (0x{:x})", result);
        error_message = exception_msg;
        return nullptr;
    }

    return new_message;
}

bool MiniFilterClient::SendReply(UserToDrvMessage* reply)
{
    HRESULT result = FilterReplyMessage(
        port_handle,
        &reply->header,
        sizeof(reply->header) + sizeof(reply->data)
    );

    if (IS_ERROR(result)) {
        std::string exception_msg(EXCEPTION_MSG_SIZE, L'\0');
        std::format_to_n(exception_msg.data(), exception_msg.size(),
            L"FilterReplyMessage failed (0x{:x})", result);
        error_message = exception_msg;
        return false;
    }

    return true;
}

MiniFilterClient::MiniFilterClient() : port_handle(nullptr) {}

MiniFilterClient::~MiniFilterClient() {
    if (port_handle) {
        FilterClose(port_handle);
    }
}