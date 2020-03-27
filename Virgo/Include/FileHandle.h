#pragma once

#include "Handle.h"
#include <string>
#include <cstdint>
#include <stdexcept>
#include <tuple>

namespace Virgo
{
	/**
	 * File handle whence values.
	 */
	enum class FileHandleWhence : DWORD
	{
		/**
		 * Offset from file start.
		 */
		FromBeginning = FILE_BEGIN,

		/**
		 * Offset from current position.
		 */
		FromCurrentPosition = FILE_CURRENT,

		/**
		 * Offset from file end.
		 */
		FromEnd = FILE_END
	};

	/**
	 * Handle class for handling file handles.
	 */
	class FileHandle : public Handle<FileHandle>
	{
	public:
		explicit FileHandle(HANDLE& other)
			: HandleType(other)
		{
		}

		explicit FileHandle(const std::string& path, const std::string& mode)
			: HandleType(FileHandle::Open(path, mode))
		{
		}

		/**
		 * Interprets mode string into fileShare/desiredAccess pair.
		 */
		static const std::tuple<DWORD, DWORD> GetFileShareDesiredAccessPairFromModeString(const std::string& mode);
		

		/**
		 * Opens a file specified by path with specified access rights.
		 */
		static const HandleType& Open(const std::string& path, const std::string& mode);

		/**
		 * Writes the buffer into file.
		 */
		std::size_t Write(const void* buffer, std::size_t offsetInBuffer, std::size_t size);

		/**
		 * Reads from file into buffer.
		 */
		std::size_t Read(void* buffer, std::size_t offsetInBuffer, std::size_t size);

		/**
		 * Flushes the caches into disk.
		 */
		void Flush();

		/**
		 * Seeks into specific offset in file.
		 */
		std::size_t Seek(std::int64_t offset, FileHandleWhence whence);

		/**
		 * Tells the offset of file handle, relative to file beginning.
		 */
		std::size_t Tell();

		/**
		 * Checks if end of file has been reached.
		 */
		bool IsEndOfFile();

		/**
		 * Closes the file.
		 */
		void Close();
	};
}