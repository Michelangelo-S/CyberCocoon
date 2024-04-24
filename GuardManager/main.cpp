#include "main.h"

int main(int argc, char* argv[]) {
	// Convert com port name to wstring
	std::wstring wide_name = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(COM_PORT_NAME);

	// Read the config for this file
	std::vector<FileAction> file_actions = ConfigReader::read_config("config.json");
	if (!file_actions.size())
		return 1;

	// Create the mini filter communication client
	auto miniflt_client = std::make_unique<MiniFilterClient>();
	if (!miniflt_client->Initialize(wide_name)) {
		std::cout << miniflt_client->GetErrorMessage();
		return 1;
	}

	std::cout << "Connection established!" << std::endl;

	while (true) {
		// Wait for a new message from the minifilter
		auto new_msg = miniflt_client->ReceiveMessage();

		// New msg received. Create a new reply obj, and copy the msg id.
		UserToDrvMessage reply;
		reply.header.MessageId = new_msg->header.MessageId;
		reply.data.block = false;

		// Get the file path as a wstring
		auto file_path = std::wstring(new_msg->data.path);
		DWORD pid = new_msg->data.process;

		// Iterate through all registered file rules
		for (const auto& file_action : file_actions) {
			// If no action is registered for this file skip
			if (file_path.find(file_action.file) == std::wstring::npos)
				continue;

			// Print a message notifying of a new file access
			std::wcout << file_path << L" has been requested by " << GetProcessName(pid) << " (" << pid << "). ";

			// If file access should be denied notify the driver to do so
			if (file_action.action == ActionType::ACCESS_DENIED) {
				reply.data.block = true;
				std::wcout << "File has been blocked.";
			}
			else {
				std::wcout << "File has been allowed.";
			}

			std::wcout << std::endl;
			break;
		}

		// Send the reply back to the minifilter
		if (!miniflt_client->SendReply(&reply)) {
			std::cout << miniflt_client->GetErrorMessage();
			return 1;
		}
	}

	return 0;
}

std::wstring GetProcessName(DWORD process_id) {
	std::wstring process_name;

	// Create a snapshot of the current processes
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	// Check if the snapshot creation was successful
	if (snapshot != INVALID_HANDLE_VALUE) {
		PROCESSENTRY32 process_entry;
		process_entry.dwSize = sizeof(process_entry);

		// Retrieve information about the first process in the snapshot
		if (Process32First(snapshot, &process_entry)) {
			// Iterate through the processes in the snapshot
			do {
				// Check if the current process has the specified process ID
				if (process_entry.th32ProcessID == process_id) {
					process_name = process_entry.szExeFile;
					break;
				}
			} while (Process32Next(snapshot, &process_entry));
		}
		// Close the handle to the snapshot
		CloseHandle(snapshot);
	}
	return process_name;
}