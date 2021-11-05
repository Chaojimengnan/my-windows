#include "mw_job.h"
#include "mw_utility.h"

namespace mw {

	bool is_current_process_in_job()
	{
		BOOL is_in_job = FALSE;
		IsProcessInJob(GetCurrentProcess(), NULL, &is_in_job);
		GET_ERROR_MSG_OUTPUT(std::cout)
		return is_in_job;
	}



	job::job():job_handle(NULL)
	{
	}

	job::~job()
	{
		CloseHandle(job_handle);
		GET_ERROR_MSG_OUTPUT(std::cout)
	}


	bool job::create(const std::string& job_name, LPSECURITY_ATTRIBUTES security_attribute)
	{
		if (!job_handle)
		{
			job_handle = CreateJobObjectA(security_attribute,
				string_to_pointer(job_name));
			GET_ERROR_MSG_OUTPUT(std::cout)
			return job_handle;
		}
		else {
			return false;
		}
	}

	bool job::open(const std::string& job_name, bool inherit_handle, DWORD desired_access)
	{
		if (!job_handle)
		{
			job_handle = OpenJobObjectA(desired_access, inherit_handle, job_name.c_str());
			GET_ERROR_MSG_OUTPUT(std::cout)
				return job_handle;
		}
		else {
			return false;
		}
	}

	bool job::assign_process(HANDLE process)
	{
		auto is_ok = AssignProcessToJobObject(job_handle, process);
		GET_ERROR_MSG_OUTPUT(std::cout)
		return is_ok;
	}

	bool job::set_information(JOBOBJECTINFOCLASS information_type, LPVOID job_information, DWORD size_of_information)
	{
		auto is_ok = SetInformationJobObject(job_handle, information_type,
			job_information, size_of_information);
		GET_ERROR_MSG_OUTPUT(std::cout)
		return is_ok;
	}

	bool job::query_information(JOBOBJECTINFOCLASS information_type, LPVOID output_job_information, DWORD size_of_information)
	{
		auto is_ok = QueryInformationJobObject(job_handle, 
			information_type, output_job_information, size_of_information, NULL);
		GET_ERROR_MSG_OUTPUT(std::cout)
		return is_ok;
	}

	bool job::kill_all_process_in_job(UINT exit_code)
	{
		auto is_ok = TerminateJobObject(job_handle, exit_code);
		GET_ERROR_MSG_OUTPUT(std::cout)
		return is_ok;
	}




}//mw