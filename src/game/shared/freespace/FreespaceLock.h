#ifndef SM_API_TESTAPPCONSOLE_MUTEX_H
#define SM_API_TESTAPPCONSOLE_MUTEX_H
 
 
namespace Freespace
{
	// A very simple mutex class for sample code purposes. 
	// It is recommended that you use the boost threads library.
	class Mutex
	{
	public:
		Mutex()
		{
			if (!InitializeCriticalSectionAndSpinCount(&_cs,0x80000400)) 
			{
				throw std::runtime_error("Failed to initialize Mutex");
			}
		}
		~Mutex()
		{
			DeleteCriticalSection(&_cs);
		}
		void lock() const
		{
			EnterCriticalSection(&_cs); 
		}
		void unlock() const
		{
			LeaveCriticalSection(&_cs); 
		}
	private:
		// Noncopyable
		Mutex(const Mutex &);
		Mutex &operator=(const Mutex &);
	private:
		mutable CRITICAL_SECTION _cs;
	};
}
#endif


#ifndef SM_API_TESTAPPCONSOLE_LOCK_H
#define SM_API_TESTAPPCONSOLE_LOCK_H
 
namespace Freespace
{
	// A very simple scoped-lock class for sample code purposes. 
	// It is recommended that you use the boost threads library.
	class Lock
	{
	public:
		Lock(const Mutex &mutex): _mutex(mutex)
		{
			_mutex.lock();
		}
		~Lock()
		{
			_mutex.unlock();
		}
	private:
		// Noncopyable
		Lock(const Lock &);
		Lock &operator=(const Lock &);
	private:
		const Mutex &_mutex;
	};
}
#endif
