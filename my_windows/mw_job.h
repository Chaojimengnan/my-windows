#pragma once



namespace mw {
	class job;

	/// <summary>
	/// 当前线程所在进程是否在任何一个作业中
	/// </summary>
	/// <returns>当前进程是否在作业中</returns>
	MW_API bool is_current_process_in_job();

	/// <summary>
	/// 创建并返回一个job智能指针
	/// </summary>
	/// <param name="...args">构造函数参数</param>
	/// <returns>返回一个job智能指针</returns>
	template <typename... Args>
	inline std::shared_ptr<job> make_job(Args&&... args)
	{
		return std::shared_ptr<job>(new job(std::forward<Args>(args)...));
	}

	/// <summary>
	/// 作业类，用于控制和使用windows的"作业"
	/// </summary>
	class MW_API job
	{
	public:
		job();
		~job() { CloseHandle(job_handle); GET_ERROR_MSG_OUTPUT(std::tcout);}

	public:
		job(const job&) = delete;
		job(job&&) = delete;
		job& operator=(const job&) = delete;
		job& operator=(job&&) = delete;

	public:

		/// <summary>
		/// 创建一个作业
		/// </summary>
		/// <param name="job_name">作业名字</param>
		/// <param name="security_attribute">安全属性</param>
		/// <returns>是否创建成功</returns>
		bool create(const std::tstring& job_name, 
			LPSECURITY_ATTRIBUTES security_attribute = nullptr);

		/// <summary>
		/// 打开一个作业
		/// </summary>
		/// <param name="job_name">作业名字</param>
		/// <param name="inherit_handle">该进程以后创建的子进程是否能继承该作业句柄</param>
		/// <param name="desired_access">新句柄的访问权限</param>
		/// <returns>是否打开成功</returns>
		bool open(const std::tstring& job_name, bool inherit_handle = FALSE, 
			DWORD desired_access = JOB_OBJECT_ALL_ACCESS);

		/// <summary>
		/// 获取作业句柄，在之前请确认是否打开或创建了作业句柄
		/// </summary>
		/// <returns>作业句柄</returns>
		HANDLE get_handle() { return job_handle; }

		/// <summary>
		/// 将进程装入该作业中
		/// </summary>
		/// <param name="process">目标进程</param>
		/// <returns>操作是否成功</returns>
		bool assign_process(HANDLE process);

		/// <summary>
		/// 设置作业信息
		/// </summary>
		/// <param name="information_type">信息类型</param>
		/// <param name="job_information">信息结构的指针</param>
		/// <param name="size_of_information">信息结构的大小</param>
		/// <returns>操作是否成功</returns>
		bool set_information(JOBOBJECTINFOCLASS information_type, 
			LPVOID job_information, DWORD size_of_information);

		/// <summary>
		/// 查询作业信息
		/// </summary>
		/// <param name="information_type">信息类型</param>
		/// <param name="output_job_information">[out]信息结构的指针(不可以传入NULL)</param>
		/// <param name="size_of_information">信息结构的大小</param>
		/// <returns>操作是否成功</returns>
		bool query_information(JOBOBJECTINFOCLASS information_type, 
			LPVOID output_job_information, DWORD size_of_information);

		/// <summary>
		/// 杀死该作业中的所有进程
		/// </summary>
		/// <param name="exit_code">进程的退出代码</param>
		/// <returns>操作是否成功</returns>
		bool kill_all_process_in_job(UINT exit_code);
	private:
		HANDLE job_handle;
	};

}//mw