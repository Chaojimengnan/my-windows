#pragma once

namespace mw {
	
	/// <summary>
	/// 创建或打开文件或I/O设备，如文件、文件流、目录、物理磁盘、卷、控制台缓冲区、磁带驱动器、通信资源、邮槽和管道
	/// </summary>
	/// <remarks>
	/// 使用前请务必查看文档
	/// creation_disposition可为CREATE_ALWAYS，CREATE_NEW，OPEN_ALWAYS，OPEN_EXISTING，TRUNCATE_EXISTING
	/// </remarks>
	/// <param name="file_name">要创建或打开的文件或设备的名称。您可以在此名称中使用正斜杠(/)或反斜杠(\),在UNICODE版本可以超过MAX_PATH个字符(看文档)</param>
	/// <param name="desired_access">请求的对文件或设备的访问权限，可以是GENERIC_READ和GENERIC_WRITE或它们的组合，若为0则表示只想改变设备的属性和配置</param>
	/// <param name="creation_disposition">对存在或不存在的文件或设备采取的操作，对于不是文件的设备，该参数通常置为OPEN_EXISTING，其可选值请看文档</param>
	/// <param name="share_mode">请求的文件或设备的共享模式，它可以是0或者FILE_SHARE开头的宏，其中READ和WRITE可以组合，若为0表示独占文件或设备</param>
	/// <param name="flags_and_attributes">它可以是FILE_ATTRIBUTE(用于文件)和FILE_FLAG的组合,若不是创建新文件并且template_file为NULL，ATTRIBUTE没有作用</param>
	/// <param name="template_file">具有GENERIC_READ访问权限的模板文件的有效句柄。模板文件为创建的文件提供文件属性和扩展属性,若是打开现有文件，该参数被忽略</param>
	/// <param name="file_attributes">文件或I/O设备的安全属性</param>
	/// <returns>则返回值是指定文件、设备、命名管道或邮槽的打开句柄，若失败返回INVALID_HANDLE_VALUE</returns>
	inline HANDLE create_file(const std::tstring& file_name, DWORD desired_access = GENERIC_WRITE|GENERIC_READ,
		DWORD creation_disposition = CREATE_NEW, DWORD share_mode = FILE_SHARE_READ, 
		DWORD flags_and_attributes = FILE_FLAG_SEQUENTIAL_SCAN|FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED, 
		HANDLE template_file = nullptr, LPSECURITY_ATTRIBUTES file_attributes = nullptr)
	{
		auto val = CreateFile(file_name.c_str(), desired_access, share_mode, 
			file_attributes, creation_disposition, flags_and_attributes, template_file);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}


	/// <summary>
	/// 获取指定文件或I/O设备的类型
	/// </summary>
	/// <param name="file_handle">文件或I/O设备的句柄</param>
	/// <returns>返回值是FILE_TYPE_开头的宏</returns>
	inline DWORD get_file_type(HANDLE file_handle) 
	{
		auto val = GetFileType(file_handle);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 获取指定文件的逻辑大小
	/// </summary>
	/// <param name="file_handle">文件的句柄，该句柄必须具有FILE_READ_ATTRIBUTES访问权限，或者调用线程必须对包含该文件的目录具有足够的权限</param>
	/// <param name="file_size">[out]用于接收文件大小的LARGE_INTEGER联合，以字节为单位</param>
	/// <returns>操作是否成功</returns>
	inline BOOL get_file_size(HANDLE file_handle, LARGE_INTEGER& file_size)
	{
		auto val = GetFileSizeEx(file_handle, &file_size);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 获取指定文件名的文件的实际大小(实际磁盘存储的字节数)
	/// </summary>
	/// <param name="file_name">文件的名称。您可以在此名称中使用正斜杠(/)或反斜杠(\),在UNICODE版本可以超过MAX_PATH个字符</param>
	/// <returns>返回实际大小，需要调用GetLastError来确定是否出错</returns>
	inline ULARGE_INTEGER get_compressed_file_size(const std::tstring& file_name)
	{
		ULARGE_INTEGER temp = {0};
		temp.LowPart = GetCompressedFileSize(file_name.c_str(), &temp.HighPart);
		GET_ERROR_MSG_OUTPUT();
		return temp;
	}

	 
	/// <summary>
	/// 从指定的文件或输入/输出 (I/O) 设备读取数据。如果设备支持，读取发生在文件指针指定的位置。该函数用于同步I/O(未指定FILE_FLAG_OVERLAPPED)，或使用I/O完成端口的异步
	/// </summary>
	/// <param name="file_handle">设备句柄，该句柄必须已经具有读取权限创建，若要使用异步操作，该句柄必须具有标志FILE_FLAG_OVERLAPPED</param>
	/// <param name="buffer">[out]指向接收从文件或设备读取的数据的缓冲区的指针，该缓冲区必须在读取期间有效，且在读取完毕之前不得使用该缓冲区</param>
	/// <param name="bytes_to_read">要读取的最大字节数</param>
	/// <param name="bytes_read">[out,opt]用于接收使用该同步函数读取的字节数的变量的指针，当是异步IO时，该参数应该为NULL</param>
	/// <param name="overlapped">[in,out,opt]指向OVERLAPPED数据结构的指针，该结构提供要在异步（重叠）文件读取操作期间使用的数据，该结构必须在读取期间可用</param>
	/// <returns>若函数成功，返回TRUE，若函数失败，或正在异步完成，返回值为FALSE。GetLastError得到的ERROR_IO_PENDING不是失败值(异步)</returns>
	inline BOOL read_file(HANDLE file_handle, LPVOID buffer, DWORD bytes_to_read, LPDWORD bytes_read, LPOVERLAPPED overlapped = nullptr)
	{
		auto val = ReadFile(file_handle, buffer, bytes_to_read, bytes_read, overlapped);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 从指定的文件或输入/输出 (I/O) 设备读取数据。它异步报告其完成状态，在读取完成或取消并且调用线程处于可警报等待状态时调用指定的完成例程。
	/// </summary>
	/// <remarks>
	/// 若函数成功，则调用线程有一个异步I/O操作挂起：从文件中重叠读取操作。
	/// 当这个I / O操作完成，并调用线程被阻塞在报警等待状态时，系统调用指定的完成例程；
	/// 
	/// 如果函数成功，文件读取操作完成，但调用线程未处于可报警等待状态，
	/// 系统将完成例程调用排队，保持调用直到调用线程进入可报警等待状态。
	/// 
	/// 对于支持字节偏移的文件，您必须指定开始写入文件的字节偏移。您可以通过设置OVERLAPPED结构的Offset和 OffsetHigh成员来 指定此偏移量。
	/// </remarks>
	/// <param name="file_handle">设备句柄，该句柄必须已经具有读取权限创建，并且它应该具有FILE_FLAG_OVERLAPPED标记(异步标记)</param>
	/// <param name="buffer">[out,opt]指向接收从文件或设备读取的数据的缓冲区的指针，该缓冲区必须在读取期间有效，且在读取完毕之前不得使用该缓冲区</param>
	/// <param name="bytes_to_read">要读取的字节数</param>
	/// <param name="overlapped">[in,out]指向OVERLAPPED数据结构的指针，该结构提供要在异步（重叠）文件读取操作期间使用的数据，该结构必须在读取期间可用</param>
	/// <param name="completion_routine">当读取操作完成并且调用线程处于可警告的等待状态时，指向要调用的完成例程的指针</param>
	/// <returns>操作是否成功</returns>
	inline BOOL read_file_async(HANDLE file_handle, LPVOID buffer, DWORD bytes_to_read, 
		LPOVERLAPPED overlapped, LPOVERLAPPED_COMPLETION_ROUTINE completion_routine)
	{
		auto val = ReadFileEx(file_handle, buffer, bytes_to_read, overlapped, completion_routine);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	
	/// <summary>
	/// 将数据写入指定的文件或输入/输出 (I/O) 设备。该函数用于同步I/O(未指定FILE_FLAG_OVERLAPPED)，或使用I/O完成端口的异步I/O
	/// </summary>
	/// <param name="file_handle">设备句柄，该句柄必须已经具有写权限创建，若要使用异步操作，该句柄必须具有标志FILE_FLAG_OVERLAPPED</param>
	/// <param name="buffer">指向包含要写入文件或设备的数据的缓冲区的指针，该缓冲区必须在写操作期间有效，且在写操作完毕之前不得使用该缓冲区</param>
	/// <param name="bytes_to_write">要写入文件或设备的字节数。零值指定空写操作。空写操作的行为取决于底层文件系统或通信技术</param>
	/// <param name="bytes_written">[out,opt]用于接收使用该同步函数写入的字节数的变量的指针，当是异步IO时，该参数应该为NULL</param>
	/// <param name="overlapped">[in,out,opt]指向OVERLAPPED数据结构的指针，该结构提供要在异步（重叠）文件写入操作期间使用的数据，该结构必须在写入期间可用</param>
	/// <returns>若函数成功，返回TRUE，若函数失败，或正在异步完成，返回值为FALSE。GetLastError得到的ERROR_IO_PENDING不是失败值(异步)</returns>
	inline BOOL write_file(HANDLE file_handle, LPCVOID buffer, DWORD bytes_to_write, LPDWORD bytes_written, LPOVERLAPPED overlapped = nullptr)
	{
		auto val = WriteFile(file_handle, buffer, bytes_to_write, bytes_written, overlapped);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}


	/// <summary>
	/// 将数据写入指定的文件或输入/输出 (I/O) 设备。它异步报告其完成状态，在写入完成或取消并且调用线程处于可警报等待状态时调用指定的完成例程。
	/// </summary>
	/// <remarks>
	/// 若函数成功，则调用线程有一个异步I/O操作挂起：从文件中重叠读取操作。
	/// 当这个I / O操作完成，并调用线程被阻塞在报警等待状态时，系统调用指定的完成例程；
	/// 
	/// 如果函数成功，文件读取操作完成，但调用线程未处于可报警等待状态，
	/// 系统将完成例程调用排队，保持调用直到调用线程进入可报警等待状态。
	/// 
	/// 对于支持字节偏移的文件，您必须指定开始写入文件的字节偏移。您可以通过设置OVERLAPPED结构的Offset和 OffsetHigh成员来 指定此偏移量。
	/// 要写入文件末尾，请将OVERLAPPED结构的Offset和OffsetHigh成员指定为 0xFFFFFFFF(即(DWORD)-1)。
	/// 这在功能上等同于先前调用CreateFile函数以使用FILE_APPEND_DATA访问打开hFile。
	/// </remarks>
	/// <param name="file_handle">设备句柄，该句柄必须已经具有写操作权限创建，并且它应该具有FILE_FLAG_OVERLAPPED标记(异步标记),不要将I/O完成端口与此句柄相关联</param>
	/// <param name="buffer">[opt]指向接收从文件或设备读取的数据的缓冲区的指针，该缓冲区必须在读取期间有效，且在读取完毕之前不得使用该缓冲区</param>
	/// <param name="bytes_to_write">要写入文件或设备的字节数。零值指定空写操作。空写操作的行为取决于底层文件系统或通信技术</param>
	/// <param name="overlapped">[in,out]指向OVERLAPPED数据结构的指针，该结构提供要在异步（重叠）文件读取操作期间使用的数据，该结构必须在写操作期间可用</param>
	/// <param name="completion_routine">当写操作完成并且调用线程处于可警告的等待状态时，指向要调用的完成例程的指针</param>
	/// <returns>操作是否成功</returns>
	inline BOOL write_file_async(HANDLE file_handle, LPCVOID buffer, DWORD bytes_to_write, 
		LPOVERLAPPED overlapped, LPOVERLAPPED_COMPLETION_ROUTINE completion_routine)
	{
		auto val = WriteFileEx(file_handle, buffer, bytes_to_write, overlapped, completion_routine);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 移动指定文件的文件指针，此函数返回的文件指针不用于重叠(异步)的读写操作。要指定重叠操作的偏移量，请使用OVERLAPPED结构的Offset和 OffsetHigh成员 
	/// </summary>
	/// <remarks>多线程设置文件指针时要小心，你必须同步对共享资源的访问，例如，线程共享文件句柄，更新文件指针和读取文件的应用程序必须使用同步机制来保护此序列</remarks>
	/// <param name="file_handle">文件的句柄。必须使用GENERIC_READ或GENERIC_WRITE访问权限创建文件句柄</param>
	/// <param name="distance_to_move">移动文件指针的字节数。正值将指针在文件中向前移动，负值将文件指针向后移动</param>
	/// <param name="move_method">文件指针移动的起点，它可以是FILE_BEGIN，FILE_CURRENT，FILE_END(与标准库的文件操作对应)</param>
	/// <param name="new_file_pointer">[out,opt]一个指向接收新文件指针的变量的指针。如果此参数为 NULL，则不返回新文件指针</param>
	/// <returns>操作是否成功</returns>
	inline BOOL set_file_pointer(HANDLE file_handle, LARGE_INTEGER distance_to_move, DWORD move_method, PLARGE_INTEGER new_file_pointer = nullptr)
	{
		auto val = SetFilePointerEx(file_handle, distance_to_move, new_file_pointer, move_method);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 将指定文件的物理文件大小设置为文件指针的当前位置，物理文件大小也称为文件结尾。该函数可以用来截断或扩展的文件。请使用SetFileValidData函数设置文件逻辑结尾
	/// </summary>
	/// <param name="file_handle">要扩展或截断的文件的句柄,必须使用GENERIC_WRITE访问权限创建文件句柄</param>
	/// <returns>操作是否成功</returns>
	inline BOOL set_end_of_file(HANDLE file_handle)
	{
		auto val = SetEndOfFile(file_handle);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 刷新指定文件的缓冲区并将所有缓冲数据写入文件
	/// </summary>
	/// <remarks>
	/// 如果file_handle是通信设备的句柄，则该函数仅刷新传输缓冲区
	/// 如果hFile是命名管道的服务器端的句柄，则在客户端从管道读取所有缓冲数据之前，该函数不会返回。
	/// 如果hFile是控制台输出的句柄，则该函数失败。那是因为控制台输出没有缓冲
	/// 
	/// 其他备注请看文档
	/// </remarks>
	/// <param name="file_handle">打开文件的句柄，该句柄必须具有GENERIC_WRITE访问权限</param>
	/// <returns>操作是否成功</returns>
	inline BOOL flush_file_buffers(HANDLE file_handle)
	{
		auto val = FlushFileBuffers(file_handle);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 将指定线程发出的挂起 *同步* I/O 操作标记为已取消
	/// </summary>
	/// <param name="thread_handle">指定发出同步I/O的线程句柄，该句柄必须具有THREAD_TERMINATE访问权限</param>
	/// <returns>操作是否成功</returns>
	inline BOOL cancle_synchronous_io(HANDLE thread_handle)
	{
		auto val = CancelSynchronousIo(thread_handle);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 取消调用线程为指定文件发出的所有挂起的异步输入和输出 (I/O) 操作。该函数不会取消其他线程为文件句柄发出的异步 I/O 操作。
	/// </summary>
	/// <remarks>所有被取消的 I/O 操作都以错误ERROR_OPERATION_ABORTED完成，并且 I/O 操作的所有完成通知都正常发生。</remarks>
	/// <param name="file_handle">该函数取消调用线程发送给此文件句柄的所有挂起 I/O 操作</param>
	/// <returns>操作是否成功</returns>
	inline BOOL cancle_io(HANDLE file_handle)
	{
		auto val = CancelIo(file_handle);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 为指定的文件句柄标记任何未完成的 I/O 操作。该函数只取消当前进程中的 I/O 操作，而不管哪个线程创建了 I/O 操作(即当前进程的所有线程)。
	/// </summary>
	/// <remarks>线程可以使用GetOverlappedResult函数来确定 I/O 操作本身何时完成。</remarks>
	/// <param name="file_handle">文件的句柄</param>
	/// <param name="overlapped">包含用于异步 I/O 的数据的OVERLAPPED数据结构的指针，若为NULL，则取消对指定文件句柄的所有I/O请求，否则只取消该参数指定的I/O</param>
	/// <returns>若成功返回非零值，若失败返回0，若找不到取消请求，返回0，注意，该函数不会等待所有取消操作完成，所以在所有取消操作完成之前，确保OVERLAPPED结构完好</returns>
	inline BOOL cancle_io_ex(HANDLE file_handle, LPOVERLAPPED overlapped = nullptr)
	{
		auto val = CancelIoEx(file_handle, overlapped);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}
	
	/// <summary>
	/// 创建一个尚未与文件句柄关联的I/O完成端口，或创建一个I/O完成端口并与指定文件句柄关联，或使指定存在的I/O完成端口与指定文件句柄关联
	/// </summary>
	/// <remarks>
	/// 将打开的文件句柄的实例与 I/O 完成端口相关联允许进程接收涉及该文件句柄的异步 I/O 操作完成的通知。
	/// 注意，文件不仅仅可以是磁盘上的文件，可以是任何支持重叠I/O的系统对象(如TCP套接字，命名管道和邮件槽等等..)
	/// </remarks>
	/// <param name="file_handle">可以是以重叠I/O打开的设备句柄，或者INVALID_HANDLE_VALUE，前者将与指定完成端口关联，后者不关联</param>
	/// <param name="existing_completion_port">现有I/O完成端口或nullptr，后者将创建一个新I/O完成端口，若file_handle不为INVALID_HANDLE_VALUE则与其关联</param>
	/// <param name="completion_key">用户定义的完成键，包含在指定文件句柄的每个 I/O 完成数据包中</param>
	/// <param name="number_of_concurrent_threads">允许并发处理I/O完成端口最大线程数，若为0则允许与系统中处理器数量相同的值，若existing_completion_port不为nullptr，则忽略该参数</param>
	/// <returns>若成功，返回值是I/O完成端口的句柄(新创建的或已经存在的)，若失败返回NULL</returns>
	inline HANDLE create_io_completion_port(HANDLE file_handle = INVALID_HANDLE_VALUE, 
		HANDLE existing_completion_port = nullptr, ULONG_PTR completion_key = 0 , DWORD number_of_concurrent_threads = 0)
	{
		auto val = CreateIoCompletionPort(file_handle, existing_completion_port, completion_key, number_of_concurrent_threads);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}


	/// <summary>
	/// 尝试从指定的 I/O 完成端口弹出一个 I/O 完成数据包。如果没有完成数据包排队，该函数将等待与完成端口关联的 I/O 操作完成
	/// </summary>
	/// <param name="completion_port">要监视的完成端口的句柄</param>
	/// <param name="bytes_to_transferred">[out]用于接收完成的 I/O 操作中传输的字节数</param>
	/// <param name="completion_key">[out]用于接收在完成端口与指定设备关联时指定的完成键(用它可以辨识是哪一个设备的I/O操作完成了)</param>
	/// <param name="overlapped">[out]接收在I/O操作开始时指定的OVERLAPPED结构体的指针(将hEvent低位设为1可以防止I/O完成后插入完成端口队列中)</param>
	/// <param name="milliseconds">调用线程愿意等待完成数据包出现在完成端口的毫秒数，若超时，返回FALSE，并将overlapped设为NULL，可以是INFINITE</param>
	/// <returns>操作是否成功</returns>
	inline BOOL get_queued_completion_status(HANDLE completion_port, 
		DWORD& bytes_to_transferred, ULONG_PTR& completion_key, OVERLAPPED*& overlapped, DWORD milliseconds = INFINITE)
	{
		auto val = GetQueuedCompletionStatus(completion_port, &bytes_to_transferred, &completion_key, &overlapped, milliseconds);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}


	/// <summary>
	/// 同时弹出多个完成端口数据包。它等待与指定完成端口关联的 I/O 操作完成。若要一次一个弹出，使用非ex版本
	/// </summary>
	/// <param name="completion_port">要监视的完成端口的句柄</param>
	/// <param name="completion_port_entries">[out]它应该是一个预先分配的OVERLAPPED_ENTRY结构数组，用于接收多个完成端口数据包</param>
	/// <param name="count">该值可以小于等于completion_port_entries数组的长度，即指定要弹出的最大数量</param>
	/// <param name="num_entries_removed">[out]用于接收实际弹出的数据包数量</param>
	/// <param name="milliseconds">调用线程愿意等待完成数据包出现在完成端口的毫秒数，若超时，返回FALSE，并将overlapped设为NULL，可以是INFINITE</param>
	/// <param name="alertable">是否是可警告的(可提醒的)，若为TRUE，当I/O完成例程或APC排队到线程时，线程返回</param>
	/// <returns>操作是否成功</returns>
	inline BOOL get_queued_completion_status_ex(HANDLE completion_port, LPOVERLAPPED_ENTRY completion_port_entries, 
		ULONG count, ULONG& num_entries_removed, DWORD milliseconds = INFINITE, BOOL alertable = true)
	{
		auto val = GetQueuedCompletionStatusEx(completion_port, completion_port_entries,
			count, &num_entries_removed, milliseconds, alertable);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 投递一个 I/O 完成数据包给指定的 I/O 完成端口，该函数可用于模拟一条已完成的I/O请求，以实现对线程池线程的通信
	/// </summary>
	/// <param name="completion_port">要投递的指定 I/O 完成端口句柄，指定数据包将发送到该端口</param>
	/// <param name="bytes_to_transferred">它将是GetQueuedCompletionStatus的bytes_to_transferred参数</param>
	/// <param name="completion_key">它将是GetQueuedCompletionStatus的completion_key参数</param>
	/// <param name="overlapped">它将是GetQueuedCompletionStatus的overlapped参数</param>
	/// <returns>操作是否成功</returns>
	inline BOOL post_queued_completion_status(HANDLE completion_port, DWORD bytes_to_transferred = 0, 
		ULONG_PTR completion_key = 0, LPOVERLAPPED overlapped = nullptr)
	{
		auto val = PostQueuedCompletionStatus(completion_port, bytes_to_transferred, completion_key, overlapped);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}



};//mw