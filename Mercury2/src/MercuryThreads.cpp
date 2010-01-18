#include <MercuryThreads.h>

#if defined( WIN32 )
#include <windows.h>
#else
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>
#endif

//XXX WARNING in windows mutex of the same name are shared!!!
//we can not give mutexes a default name

#include <stdio.h>

MercuryThread::MercuryThread()
	:m_haltOnDestroy(true), m_thread(0)
{
#if defined( WIN32 )
	mkThreadData = NULL;
#endif
}

MercuryThread::MercuryThread( const MString &name )
	:m_name(name), m_haltOnDestroy(true), m_thread(0)
{
#if defined( WIN32 )
	mkThreadData = NULL;
#endif
}

MercuryThread::~MercuryThread( )
{
	Close();
}

#if defined( WIN32 )
class StartThreadData
{
public:
	void * (*m_pFunc)( void *pData );
	void *m_pData;
};

static DWORD WINAPI StartThread( LPVOID pData )
{
	StartThreadData * in = (StartThreadData *)pData;
	DWORD ret = (DWORD)in->m_pFunc( in->m_pData );
	delete in;
	return ret;
}
#endif

int MercuryThread::Create( void * (*fn)(void *), void *data, bool detach )
{
#if defined( WIN32 )
	mkThreadData = new StartThreadData;
	mkThreadData->m_pData = data;
	mkThreadData->m_pFunc = fn;
	m_thread = CreateThread( NULL, 0, &StartThread, mkThreadData, 0, NULL );
	return m_thread != NULL;
#else
	pthread_attr_t pthread_custom_attr;
	pthread_attr_init(&pthread_custom_attr);
	if ( pthread_create( &m_thread, &pthread_custom_attr, fn, data ) )
		return false;
	else
	{
		if (detach)
			pthread_detach( m_thread ); //reclaim memory on thread destruction
		return true;
	}
#endif
}

int MercuryThread::Halt( bool kill )
{
	if (m_thread)
	{
#if defined( WIN32 )
		if ( kill )
			return TerminateThread( m_thread, 0 );
		else
			return SuspendThread( m_thread );
#else
//		pthread_detach( m_thread ); //reclaim memory on thread destruction
		if (pthread_equal(pthread_self(), m_thread) != 0) { pthread_exit(NULL); }
		else { pthread_cancel( m_thread ); }
#endif
	}
	return 0;
}

int MercuryThread::Resume( )
{
#if defined( WIN32 )
	return ResumeThread( m_thread );
#else
	return 0;
#endif
}

int MercuryThread::Wait( long lMilliseconds )
{
#if defined( WIN32 )
	return WaitForSingleObject( m_thread, lMilliseconds ) == 0;
#else
	pthread_join( m_thread, NULL );
	return 0;
#endif
}

void MercuryThread::Close( )
{
	if ( m_thread &&  m_haltOnDestroy)
	{
		Halt( true );
#if defined( WIN32 )
		delete mkThreadData;
		mkThreadData = NULL;
//		SAFE_DELETE( mkThreadData );
		CloseHandle( m_thread );
		m_thread = NULL;
#else
#endif
	}
}

unsigned long MercuryThread::Current()
{
#ifdef WIN32
	return GetCurrentThreadId();
#else
	return pthread_self();
#endif
}


//Mutex functions
MercuryMutex::MercuryMutex( )
:iLockCount(0),m_heldBy(0)
{
	Open( );
}

MercuryMutex::MercuryMutex( const MString &name )
:m_name(name),iLockCount(0),m_heldBy(0)
{
	Open( );
}

MercuryMutex::~MercuryMutex( )
{
	Close( );
}

void MercuryMutex::SetName(const MString& name)
{
	m_name = name;
}

bool MercuryMutex::Wait( long lMilliseconds )
{
	int r = 0;
#if defined( WIN32 )
	r = WaitForSingleObject( m_mutex, lMilliseconds );

	switch( r )
	{
	case WAIT_TIMEOUT:
		fprintf(stderr, "Mutex held by thread ID 0x%x timed out (%d locks)\n", m_heldBy, iLockCount );
		return false;
	case WAIT_FAILED:
		fprintf(stderr, "Mutex held by thread ID 0x%x failed (%d locks)\n", m_heldBy, iLockCount );
		return false;
	}
#else
	if ( lMilliseconds < 0xFFFFFF )
	{
		timeval t;
		gettimeofday( &t, 0 ); //XXX I question the efficiency of this call -- Josh
		uint64_t ns = t.tv_sec*1000000000 + t.tv_usec*1000 + lMilliseconds*1000000;
		
		timespec timeout;
		timeout.tv_sec = ns/1000000000;
		timeout.tv_nsec = ns%1000000000;
		
		r = pthread_mutex_timedlock( &m_mutex, &timeout );
	}
	else
	{
		r = pthread_mutex_lock( &m_mutex );
	}
	
	switch (r)
	{
		case EBUSY:
			fprintf(stderr, "Mutex held by thread ID 0x%lx failed (%d locks)\n", m_heldBy, iLockCount );
			return false;
		case EINVAL:
			fprintf(stderr, "Invalid mutex or invalid timeout length %ldms\n", lMilliseconds );
			return true;
		case EAGAIN:
			fprintf(stderr, "Max Recursive Locks Reached\n");
			return false;
		case ETIMEDOUT:
			fprintf(stderr, "Mutex held by thread ID 0x%lx timed out (%d locks)\n", m_heldBy, iLockCount );
			return false;
	}
//	printf("%s locked\n", m_name.c_str());		
#endif
//	printf("Locked %s\n", m_name.c_str());
	m_heldBy = MercuryThread::Current();
	++iLockCount;
//	return r;
	return true;
}

bool MercuryMutex::UnLock( )
{
//	printf("Unlocked %s\n", m_name.c_str());
	--iLockCount;
	if (iLockCount==0) m_heldBy = 0;

	int r, error;
#if defined( WIN32 )
	r = ReleaseMutex( m_mutex ); //nonzero on success
	if (r==0)
	{	
		error = GetLastError();
		fprintf(stderr, "Failed to release mutex %s, error %d!!\n", m_name.c_str(), error);
	}
	return r!=0;
#else
	error = r = pthread_mutex_unlock( &m_mutex ); //0 on success
	if (r!=0) fprintf(stderr, "Failed to release mutex %s, error %d!!\n", m_name.c_str(), error);
	return r==0;
#endif
}

int MercuryMutex::Open( )
{
#if defined( WIN32 )
	SECURITY_ATTRIBUTES *p = ( SECURITY_ATTRIBUTES* ) malloc( sizeof( SECURITY_ATTRIBUTES ) );
	p->nLength = sizeof( SECURITY_ATTRIBUTES );
	p->bInheritHandle = true;
	p->lpSecurityDescriptor = NULL;
	if ( m_name.empty() )
		m_mutex = CreateMutex( p, true, NULL );
	else
		m_mutex = CreateMutex( p, true, (LPCWSTR)m_name.c_str() );
	free( p );
	ReleaseMutex( m_mutex ); //windows API locks mutex on creation
	return (int)m_mutex;
#else
	pthread_mutexattr_t attr;
	pthread_mutexattr_init( &attr );
	pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE );
	pthread_mutex_init( &m_mutex, &attr );
	
//pthread_mutex_lock( &m_mutex ); //test deadlock

	return 0;
#endif
}

int MercuryMutex::Close( )
{
	for( int i = 0; i < iLockCount; ++i )
		UnLock();
#if defined( WIN32 )
	return CloseHandle( m_mutex );
#else
	pthread_mutex_destroy( &m_mutex );
	return 0;
#endif
}
