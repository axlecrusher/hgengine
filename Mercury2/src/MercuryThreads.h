#ifndef _MERCURY_THREADS_H
#define _MERCURY_THREADS_H

#ifdef HGENGINE
#include <MercuryString.h>
#else
#include <string>
typedef std::string MString;
#endif


#if !defined(WIN32)
#if !defined(_EE)
#include <pthread.h>
#else
#include <kernel.h>
#endif
#define __ThreadLocalStore __thread
#else
#include <WIndows.h>
#define __ThreadLocalStore __declspec(thread)
class StartThreadData;
#endif

#include <stdio.h>

///Thread object
class MercuryThread
{
public:
	MercuryThread();
	MercuryThread( const MString &name );
	~MercuryThread();

	///Create a thread of function fn and pass it data *data.
	int Create( void * (*fn)(void *), void *data, bool detach = true );

	///Wait for the thread to complete.
	int Wait( long lMilliseconds = 10000000 );

	///Terminate the thread early.  It's advised you only use this for terminating the thread.
	int Halt( bool kill = true );

	///Resume a halted thread.  This does NOT WORK IN LINUX.  It is recommended you do not use this.
	int Resume( );

	///Close and clean up a thread.  This is not necessiary.  It will be done on deletion as well.
	void Close( );

//	inline void Exit() { pthread_exit(NULL); }
	inline void HaltOnDestroy(bool t) { m_haltOnDestroy = t; }
	
	static unsigned long Current();
private:
	MString m_name;

	bool m_haltOnDestroy;
#if defined(WIN32)
	StartThreadData *mkThreadData;
	void * m_thread;
#else  
#if defined(_EE)
	s32 m_thread;
#else
	pthread_t m_thread;
#endif
#endif

};

///Mutual exclusion object
class MercuryMutex
{
public:
	MercuryMutex( );
	MercuryMutex( const MString &name );
	~MercuryMutex();
	
	/** These functions are dangerous because you need to be VERY careful to cover all
	unlock scenarios including errors and exceptions.
	Recommend using AutoMutexLock on the stack to avoid problems **/

	///Wait for a mutex to unlock (0xFFFFFF is infinate on windows)
	bool DangerousWait( long lMilliseconds = 0xFFFFFF ) { return Wait(lMilliseconds); } //return false on error
	/// Unlock a mutex for the next thing waiting in line.
	bool DangerousUnLock( ) { return UnLock( ); } //return false on error

	inline unsigned long HeldBy() const { return m_heldBy; }
	void SetName(const MString& name);
	
private:
	friend class AutoMutexLock;
	bool Wait( long lMilliseconds = 0xFFFFFF ); //return false on error
	bool UnLock( ); //return false on error
	
	///Start up a mutex.  You need to do this as well as UnLock() afterwards when in a constructor.
	int Open( );

	///Clean up a mutex.  This is done automatically on destruction of mutex.
	int Close( );

	MString m_name;
	int iLockCount;

	unsigned long m_heldBy;
#if defined( WIN32 )
	void * m_mutex;
#else
#if defined( _EE )
	s32 m_mutex;
#else
	pthread_mutex_t m_mutex;
#endif
#endif
};

///This is useful in locking a mutex in an area of code with many different exit paths.
class AutoMutexLock
{
	public:
		AutoMutexLock( MercuryMutex& mutex)
			:m_mutex(&mutex)
		{
			//loop until we get a lock but use a timeout so we are warned
			//of a held mutex indicating a possible deadlock
			bool l = m_mutex->Wait(1000);
			while (!l) m_mutex->Wait(1000);
		}

		~AutoMutexLock()
		{
			m_mutex->UnLock();
		}
	private:
		MercuryMutex* m_mutex;
};

//http://www.ddj.com/dept/cpp/184403766
//has some very good ideas in terms of threads and volatile variables

///This can be used to have the compile help us find code that is not threadsafe
template <typename T>
class MLockPtr
{
public:
	MLockPtr(volatile const T& obj, const MercuryMutex& mutex)
		:m_obj(const_cast<T*>(&obj)), m_mutex(const_cast<MercuryMutex*>(&mutex))
	{
		m_mutex->Wait();
	}
	~MLockPtr() { m_mutex->UnLock(); }
	inline T& operator*() { return *m_obj; }
	inline T* operator->() { return m_obj; }
private:
	T* m_obj;
	MercuryMutex* m_mutex;
	//Disallow
	MLockPtr(const MLockPtr&);
	MLockPtr& operator=(const MLockPtr&);
};

//Utility functions for conditional threading
// The purpose of this is to allow you to conditionally define whether or not a mutex
// exists, allowing one to benchmark the overhead of threading as well as removing 
// functionality if it is known that the system will not be using threads.

#if !defined( NOT_THREADSAFE )

#define COND_MUTEX( x ) \
	MercuryMutex x

#define SETUP_MUTEX( x ) \
	x.Open(); \
	x.UnLock()

#define LOCK_MUTEX( x ) \
	x.Wait()

#define UNLOCK_MUTEX( x ) \
	x.UnLock()

#else

#define COND_MUTEX( x )
#define SETUP_MUTEX( x )
#define LOCK_MUTEX( x )
#define UNLOCK_MUTEX( x )

#endif

#endif

