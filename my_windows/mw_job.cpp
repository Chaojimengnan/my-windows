#include "mw_job.h"

namespace mw {

	bool is_current_process_in_job()
	{
		
		BOOL is_in_job = FALSE;
		IsProcessInJob(GetCurrentProcess(), nullptr, &is_in_job);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return is_in_job;
	}



	job::job():job_handle(nullptr)
	{
	}


	bool job::create(const std::tstring& job_name, LPSECURITY_ATTRIBUTES security_attribute)
	{
		if (!job_handle)
		{
			job_handle = CreateJobObject( security_attribute,
				tstring_to_pointer(job_name));
			GET_ERROR_MSG_OUTPUT(std::tcout);
			return job_handle;
		}
		else {
			return false;
		}
	}

	bool job::open(const std::tstring& job_name, bool inherit_handle, DWORD desired_access)
	{
		if (!job_handle)
		{
			job_handle = OpenJobObject(desired_access, 
				inherit_handle, job_name.c_str());
			GET_ERROR_MSG_OUTPUT(std::tcout);
			return job_handle;
		}
		else {
			return false;
		}
	}

	bool job::assign_process(HANDLE process)
	{
		auto val = AssignProcessToJobObject(job_handle, process);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	bool job::set_information(JOBOBJECTINFOCLASS information_type, LPVOID job_information, DWORD size_of_information)
	{
		auto val = SetInformationJobObject(job_handle, information_type,
			job_information, size_of_information);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	bool job::query_information(JOBOBJECTINFOCLASS information_type, LPVOID output_job_information, DWORD size_of_information)
	{
		auto val = QueryInformationJobObject(job_handle,
			information_type, output_job_information, size_of_information, nullptr);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	bool job::kill_all_process_in_job(UINT exit_code)
	{
		auto val = TerminateJobObject(job_handle, exit_code);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

}//mw