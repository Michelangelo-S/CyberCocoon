#pragma once
#include <memory>
#include <iostream>
#include <codecvt>

#include <Windows.h>
#include <tlhelp32.h>

#include "config.h"
#include "minifilterclient.h"

int main(int argc, char* argv[]);

std::wstring GetProcessName(DWORD process_id);