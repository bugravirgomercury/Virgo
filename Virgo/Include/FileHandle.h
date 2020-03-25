#pragma once

#include "Handle.h"
#include <string>
#include <cstdint>
#include <stdexcept>

namespace Virgo
{
	/*
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

		/*
		 * Opens a file specified by path with specified access rights.
		 */
		static const HandleType& Open(const std::string& path, const std::string& mode)
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

			HANDLE hFile{ ::CreateFileA(path.c_str(), desiredAccess, fileShare, 
				NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL) };
			if (hFile == INVALID_HANDLE_VALUE)
			{
				throw NewException(::GetLastError());
			}
			
			return FileHandle{ hFile };
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