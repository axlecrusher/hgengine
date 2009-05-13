#include <MercuryThreads.h>

#if defined( WIN32 )
#include <windows.h>
#endif

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

//Mutex functions
MercuryMutex::MercuryMutex( )
:m_name("(null)")
{
	iLockCount = 0;
	Open( );
	UnLock();
}

MercuryMutex::MercuryMutex( const MString &name )
:m_name(name)
{
	iLockCount = 0;
	Open( );
	UnLock();
}

MercuryMutex::~MercuryMutex( )
{
	Close( );
}

int MercuryMutex::Wait( long lMilliseconds )
{
	int r = 0;
#if defined( WIN32 )
	r = WaitForSingleObject( m_mutex, lMilliseconds );
#else
/*	timespec abstime;
	abstime.tv_sec = 0;
	abstime.tv_nsec = lMilliseconds;
	
	pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

	pthread_cond_timedwait( &cond, &m_mutex, &abstime ); */
//	if (pthread_mutex_trylock( &m_mutex ) != 0)
//	{
//		printf("%s waiting\n", m_name.c_str());
		pthread_mutex_lock( &m_mutex );
//		printf("%s locked\n", m_name.c_str());		
//	}
#endif
//	printf("Locked %s\n", m_name.c_str());
	++iLockCount;
	return r;
}

int MercuryMutex::UnLock( )
{
//	printf("Unlocked %s\n", m_name.c_str());
	--iLockCount;
#if defined( WIN32 )
	return ReleaseMutex( m_mutex );
#else
	pthread_mutex_unlock( &m_mutex );
	return 0;
#endif
}

int MercuryMutex::Open( )
{
	iLockCount++;
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
	return (int)m_mutex;
#else
	pthread_mutexattr_t attr;
	pthread_mutexattr_init( &attr );
	pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE );
	pthread_mutex_init( &m_mutex, &attr );
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
