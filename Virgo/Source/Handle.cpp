#include <Handle.h>
template<class THandleImpl>
Virgo::Handle<THandleImpl>::HandleType& Virgo::Handle<THandleImpl>::operator=(const Virgo::Handle<THandleImpl>::HandleType& other)
{
	if (this != &other)
	{
		DupHandle(other.handle_, handle_);
	}

	return *this;
}

template<class THandleImpl>
Virgo::Handle<THandleImpl>::HandleType& Virgo::Handle<THandleImpl>::operator=(Virgo::Handle<THandleImpl>::HandleType&& other)
{
	if (this != &other)
	{
		if (handle_ != INVALID_HANDLE_VALUE)
		{
			// TODO: Check for result and throw exception accordingly.
			other.Close();
		}

		handle_ = other.handle_;

		other.handle_ = INVALID_HANDLE_VALUE;
	}

	return *this;
}

template<class THandleImpl>
Virgo::Handle<THandleImpl>::~Handle()
{
	try
	{
		Close();
	}
	catch (std::system_error const& ex)
	{
#ifdef _DEBUG
		std::stringstream stream{};
		stream << "Error occurred whilst closing handle - error #" << std::hex << ex.code();
		OutputDebugStringA(static_cast<LPCSTR>(stream.str().c_str()));
#endif
	}
}

template<class THandleImpl>
void Virgo::Handle<THandleImpl>::Close()
{
	static_cast<THandleImpl&>(*this)->Close();
}

template<class THandleImpl>
Virgo::Handle<THandleImpl>::operator HANDLE &() const
{
	return handle_;
}

template<class THandleImpl>
HANDLE & Virgo::Handle<THandleImpl>::GetHandle()
{
	return handle_;
}

template<class THandleImpl>
const HANDLE & Virgo::Handle<THandleImpl>::GetHandle() const
{
	return handle_;
}

template<class THandleImpl>
inline void Virgo::Handle<THandleImpl>::DupHandle(const HANDLE & a, HANDLE & b)
{
	DWORD dwFlags{ 0 };
	BOOL getHandleInformationResult{ ::GetHandleInformation(a, &dwFlags) };
	BOOL duplicateHandleResult{ ::DuplicateHandle(
		::GetCurrentProcess(), a,
		::GetCurrentProcess(), &b, 0,
		dwFlags & HANDLE_FLAG_INHERIT, DUPLICATE_SAME_ACCESS)
	};

	// TODO: Throw exception depending on GetLastError().
	if (!duplicateHandleResult)
	{
		throw NewException(::GetLastError());
	}
}