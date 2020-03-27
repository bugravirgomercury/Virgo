#include <FileHandle.h>

const std::tuple<DWORD, DWORD> Virgo::FileHandle::GetFileShareDesiredAccessPairFromModeString(const std::string & mode)
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

std::size_t Virgo::FileHandle::Write(const void * buffer, std::size_t offsetInBuffer, std::size_t size)
{
	/*
	 * NOTE: Try to refactor void* into something more C++.
	 */

	const void* offsetedBuffer{ static_cast<const char*>(buffer) + offsetInBuffer };
	DWORD dwWritten{ 0 };
	if (!::WriteFile(GetHandle(), offsetedBuffer, size - offsetInBuffer, &dwWritten, NULL))
		throw NewException(::GetLastError());
	return dwWritten;
}

std::size_t Virgo::FileHandle::Read(void * buffer, std::size_t offsetInBuffer, std::size_t size)
{
	/*
	 * NOTE: Try to refactor void* into something more C++.
	 */

	void* offsetedBuffer{ static_cast<char*>(buffer) + offsetInBuffer };
	DWORD dwRead{ 0 };
	if (!::ReadFile(GetHandle(), offsetedBuffer, size - offsetInBuffer, &dwRead, NULL))
		throw NewException(::GetLastError());
	return dwRead;
}

void Virgo::FileHandle::Flush()
{
	if (!::FlushFileBuffers(GetHandle()))
		throw NewException(::GetLastError());
}

std::size_t Virgo::FileHandle::Seek(std::int64_t offset, FileHandleWhence whence)
{
	LARGE_INTEGER distanceToMove{ 0ULL };
	LARGE_INTEGER liOffset{ 0ULL };
	DWORD moveMethod{ static_cast<DWORD>(whence) };

	distanceToMove.QuadPart = offset;

	if (!::SetFilePointerEx(GetHandle(), distanceToMove, &liOffset, moveMethod))
		throw NewException(::GetLastError());
	return static_cast<std::size_t>(liOffset.QuadPart);
}

std::size_t Virgo::FileHandle::Tell()
{
	return Seek(0, FileHandleWhence::FromCurrentPosition);
}

bool Virgo::FileHandle::IsEndOfFile()
{
	char buffer[1];

	try
	{
		Read(&buffer, 0, 1);
		Seek(-1, FileHandleWhence::FromCurrentPosition);
		return true;
	}
	catch (std::system_error x)
	{
		return false;
	}
}

void Virgo::FileHandle::Close()
{
	::CloseHandle(GetHandle());
}