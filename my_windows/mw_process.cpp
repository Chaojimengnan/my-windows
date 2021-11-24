#include "mw_process.h"
#include "mw_utility.h"
#include "mw_security.h"

namespace mw {
	
	bool create_process_admin(const std::tstring& file, const std::tstring& command_line)
	{
		SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
		sei.lpVerb = _T("runas");
		sei.lpFile = file.c_str();
		sei.nShow = SW_SHOWNORMAL;
		if (command_line != _T(""))
			sei.lpParameters = command_line.c_str();

		auto val = ShellExecuteEx(&sei);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	
	bool is_current_process_admin()
	{
		auto admin_sid = mw::create_admin_sid();
		BOOL is_admin;
		CheckTokenMembership(nullptr, admin_sid.get(), &is_admin);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return is_admin;
	}



}//mw