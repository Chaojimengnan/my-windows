#include "mw_process.h"
#include "mw_utility.h"
#include "mw_security.h"

namespace mw {

	
	HANDLE give_handle_to_other_process(HANDLE target_process, HANDLE handle_to_give, bool inherit_handle, DWORD desired_access, DWORD options)
	{
		HANDLE target_process_handle = nullptr;
		DuplicateHandle(GetCurrentProcess(), handle_to_give, target_process,
			&target_process_handle, desired_access, inherit_handle, options);
		GET_ERROR_MSG_OUTPUT(std::cout);
		return target_process_handle;
	}

	bool create_process(process_info& new_process_info, const std::string& command_line, const std::string& process_work_dir, 
		BOOL inherit_handle, DWORD creation_flags, LPSECURITY_ATTRIBUTES process_attributes,
		LPSECURITY_ATTRIBUTES thread_attributes,  
		LPVOID environment, LPSTARTUPINFOA startup_info)
	{
		STARTUPINFOA temp;
		PROCESS_INFORMATION proc;
		char* temp_str = new char[command_line.size() + 1];
		strcpy_s(temp_str, command_line.size() + 1, command_line.c_str());
		if (!startup_info)
		{
			ZeroMemory(&temp, sizeof(STARTUPINFOA));
			temp.cb = sizeof(STARTUPINFOA);
			startup_info = &temp;
		}
		
		auto is_ok = CreateProcessA( nullptr, temp_str, process_attributes,
			thread_attributes, inherit_handle, creation_flags,
			environment, string_to_pointer(process_work_dir), startup_info, &proc);
		GET_ERROR_MSG_OUTPUT(std::cout);
		delete[] temp_str;

		new_process_info.process_handle = mw::safe_handle(proc.hProcess);
		new_process_info.thread_handle = mw::safe_handle(proc.hThread);
		new_process_info.process_id = proc.dwProcessId;
		new_process_info.thread_id = proc.dwThreadId;

		return is_ok;
	}

	
	bool create_process_admin(const std::string& file, const std::string& command_line)
	{
		SHELLEXECUTEINFOA sei = { sizeof(SHELLEXECUTEINFOA) };
		sei.lpVerb = "runas";
		sei.lpFile = file.c_str();
		sei.nShow = SW_SHOWNORMAL;
		if (command_line != "")
			sei.lpParameters = command_line.c_str();

		auto val = ShellExecuteExA(&sei);
		GET_ERROR_MSG_OUTPUT(std::cout);
		return val;
	}

	
	bool is_current_process_admin()
	{
		auto admin_sid = mw::create_admin_sid();
		BOOL is_admin;
		CheckTokenMembership(nullptr, admin_sid.get(), &is_admin);
		GET_ERROR_MSG_OUTPUT(std::cout);
		return is_admin;
	}



}//mw