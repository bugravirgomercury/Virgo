#pragma once

#include "Handle.h"
#include <string>
#include <cstdint>

namespace Virgo
{
	/*
	 * Handle class for handling file handles.
	 */
	class FileHandle : public Handle<FileHandle>
	{
	public:
		explicit FileHandle(const std::string& path, const std::string& mode)
			: HandleType(FileHandle::Open(path, mode))
		{
		}

		/*
		 * Opens a file specified by path with specified access rights.
		 */
		static HandleType& Open(const std::string& path, const std::string& mode)
		{
			// TODO: Implement this function.
		}

		

		/* 
		 * Closes the file.
		 */
		void Close()
		{
			::CloseHandle(this);
		}
	};
}