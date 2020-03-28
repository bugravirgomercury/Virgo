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
		static const std::tuple<DWORD, DWORD> GetFileShareDesiredAccessPairFromModeString(const std::string& mode)
		{
			// Read file permission (r)
			static constexpr const size_t FLAG_READ = 1;
			// Write file permission (r)
			static constexpr const size_t FLAG_WRITE = 2;
			// Exclusive file flag (x)
			static constexpr const size_t FLAG_EXCLUSIVE = 4;

			size_t flags{ 0 };
			for (auto chr = mode.cbegin(); chr != mode.cend(); ++chr)
			{
				switch (*chr)
				{
				case 'r':
					flags |= FLAG_READ;
					break;
				case 'w':
					flags |= FLAG_WRITE;
					break;
				case 'x':
					flags |= FLAG_EXCLUSIVE;
					break;
				default:
					throw std::invalid_argument("Invalid mode string");
				}
			}

			DWORD desiredAccess{ 0 };
			DWORD fileShare{ 0 };

			if (flags & FLAG_READ)
				desiredAccess |= GENERIC_READ;
			if (flags & FLAG_WRITE)
				desiredAccess |= GENERIC_WRITE;
			if (flags & FLAG_EXCLUSIVE)
				fileShare = 0;
			else
				fileShare = FILE_SHARE_READ | FILE_SHARE_WRITE;

			return std::make_tuple(fileShare, desiredAccess);
		}
		

		/**
		 * Opens a file specified by path with specified access rights.
		 */
		static const HandleType& Open(const std::string& path, const std::string& mode)
		{
			DWORD desiredAccess{ 0 };
			DWORD fileShare{ 0 };
			std::tie(fileShare, desiredAccess) = GetFileShareDesiredAccessPairFromModeString(mode);

			HANDLE hFile{ ::CreateFileA(path.c_str(), desiredAccess, fileShare, 
				NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL) };
			if (hFile == INVALID_HANDLE_VALUE)
			{
				throw NewException(::GetLastError());
			}
			
			return FileHandle{ hFile };
		}

		/**
		 * Writes the buffer into file.
		 */
		std::size_t Write(const void* buffer, std::size_t offsetInBuffer, std::size_t size)
		{
			/*
			 * NOTE: Try to refactor void* into something more C++.
			 */

			const void* offsetedBuffer{ static_cast<const char*>(buffer) + offsetInBuffer };
			DWORD dwWritten{ 0 };
			if (!::WriteFile(this, offsetedBuffer, size - offsetInBuffer, &dwWritten, NULL))
				throw NewException(::GetLastError());
			return dwWritten;
		}

		/**
		 * Reads from file into buffer.
		 */
		std::size_t Read(void* buffer, std::size_t offsetInBuffer, std::size_t size)
		{
			/*
			 * NOTE: Try to refactor void* into something more C++.
			 */

			void* offsetedBuffer{ static_cast<char*>(buffer) + offsetInBuffer };
			DWORD dwRead{ 0 };
			if (!::ReadFile(this, offsetedBuffer, size - offsetInBuffer, &dwRead, NULL))
				throw NewException(::GetLastError());
			return dwRead;
		}

		/**
		 * Flushes the caches into disk.
		 */
		void Flush()
		{
			if (!::FlushFileBuffers(this))
				throw NewException(::GetLastError());
		}

		/**
		 * Seeks into specific offset in file.
		 */
		std::size_t Seek(std::int64_t offset, FileHandleWhence whence)
		{
			LARGE_INTEGER distanceToMove{ offset };
			LARGE_INTEGER liOffset{ 0 };
			DWORD moveMethod{ static_cast<DWORD>(whence) };
			if (!::SetFilePointerEx(this, distanceToMove, &liOffset, moveMethod))
				throw NewException(::GetLastError());
			return static_cast<std::size_t>(liOffset.QuadPart);
		}

		/**
		 * Tells the offset of file handle, relative to file beginning.
		 */
		std::size_t Tell()
		{
			return Seek(0, FileHandleWhence::FromCurrentPosition);
		}

		/**
		 * Checks if end of file has been reached.
		 */
		bool IsEndOfFile()
		{
			char buffer[1];

			try
			{
				Read(&buffer, 0, 1);
				Seek(-1, FileHandleWhence::FromCurrentPosition);
				return true;
			}
			catch (std::system_error const& x)
			{
				return false;
			}
		}

		/**
		 * Closes the file.
		 */
		void Close()
		{
			::CloseHandle(this);
		}
	};
}