#ifndef _MERCURY_VALUE_H
#define _MERCURY_VALUE_H

#include <MSemaphore.h>

///Types for the values
enum MVType
{
	TYPE_UNDEF = 0,
	TYPE_STRING,
	TYPE_INT,
	TYPE_FLOAT,
	TYPE_PTR,
};

class MValue;

///Delegate for changes to MValues
typedef void (MessageHandler::*ValueDelegate)( MValue * );

///Delegate to notify an object that it should delete itself
typedef void (MessageHandler::*DeletionNotifier)( MValue * );

///Delegate List Notifier.  This contains a list of delegates to get called back when an event happens.
class DelegateNotifierList
{
public:
	DelegateNotifierList( DeletionNotifier NotifyFunction, MessageHandler * NotifyObject );
	~DelegateNotifierList();

	void AddNotifier( DeletionNotifier NotifyFunction, MessageHandler * NotifyObject );
	void DelNotifier( DeletionNotifier NotifyFunction, MessageHandler * NotifyObject, DelegateNotifierList * & ths );
	void Notify( MValue * v );

	DeletionNotifier NotifyFunction;
	MessageHandler * NotifyObject;
	DelegateNotifierList * Next;
};

///Variable for general purpose use - generally global scope or passed around. Similar to PSElement.
/** You usually don't want to make and delete a lot of these.  It's best to keep them around and use references. */
class MValue
{
public:

	MValue( MVType t );
	~MValue();

	int GetInt() { MSemaphoreLock( &this->m_Sema ); return (m_CurType == TYPE_INT)?m_Data.l:ConvInt(); }
	float GetFloat() { MSemaphoreLock( &this->m_Sema ); return (m_CurType == TYPE_FLOAT)?m_Data.f:ConvFloat(); }
	const MString & GetString() { MSemaphoreLock( &this->m_Sema ); return (m_CurType == TYPE_STRING)?*m_Data.dataS:ConvString(); }
	bool GetBool() { MSemaphoreLock( &this->m_Sema ); return (m_CurType == TYPE_INT)?m_Data.l:ConvBool(); }
	void * GetPtr() { MSemaphoreLock( &this->m_Sema ); return (m_CurType == TYPE_PTR)?m_Data.v:0; }

	void SetInt( int iv ) { MSemaphoreLock( &this->m_Sema ); Cleanup(); m_Data.l = iv; m_CurType = TYPE_INT; Notify();}
	void SetFloat( float iv ) { MSemaphoreLock( &this->m_Sema ); Cleanup(); m_Data.f = iv; m_CurType = TYPE_FLOAT; Notify(); }
	void SetString( const MString & iv )
	{
		MSemaphoreLock( &this->m_Sema );
		if( m_CurType != TYPE_STRING )
		{
			Cleanup();
			m_Data.dataS = new MString( iv );
			m_CurType = TYPE_STRING;
		}
		else
			(*m_Data.dataS) = iv;
		Notify();
	}
	void SetBool( bool iv ){ MSemaphoreLock( &this->m_Sema ); m_Data.l = iv; m_CurType = TYPE_INT; Notify(); }
	void SetPtr( void * ptr, DelegateNotifierList * dl )
	{
		MSemaphoreLock( &this->m_Sema ); 
		Cleanup();
		SAFE_DELETE( DLDelete );
		DLDelete = dl;
		m_CurType = TYPE_PTR;
		m_Data.v = ptr;
		Notify();
	}

	MVType GetType() { return m_CurType; }
private:
	//Conv functions are not thread protected - this is because the caller of these functions should be.
	int ConvInt();
	float ConvFloat();
	const MString & ConvString();
	bool ConvBool();

	//Cleanup (to be done when object is deleted or switching types)
	void Cleanup();

	//A change happened.
	void Notify() { if( DLModify ) DLModify->Notify( this ); }

	unsigned short m_References;
	MVType m_CurType : 8;

	union ParmData
	{
		float	f;
		int	l;
		void *	v;
		MString * dataS;
	} m_Data;

	//XXX: Performance warning: One could simply operate on these
	//values atomically.  A full extra semaphore isn't necessairly
	//needed for everything
	MSemaphore m_Sema;


	DelegateNotifierList * DLDelete;
	DelegateNotifierList * DLModify;

	friend class MVRefBase;
};

//Sadly for a number of reasons, these cannot be templated or macro'd... Well, macro'd cleanly.

///Reference base - don't use this.
/** Be sure not to let anything here become virtual otherwise there will be a slow down potentially. */
class MVRefBase
{
public:
	MVRefBase(MValue * m) : mv(m) { MSemaphoreLock( &mv->m_Sema ); mv->m_References++; }
	MVRefBase(const MString & sPath);	//Special - get values from MESSAGEMAN
	~MVRefBase() {
		//If out of references, bail.
		mv->m_Sema.Wait();
		mv->m_References--;
		if( mv->m_References <= 0 )
		{
			delete mv;
			return;
		}
		mv->m_Sema.UnLock();
	}
protected:
	MValue * mv;
};

///Value Reference for Int objects.
class MVRefInt : public MVRefBase
{
public:
	int Get() { return mv->GetInt(); } 
	void Set( int iv ) { mv->SetInt( iv ); }
};

///Value Reference for Float objects.
class MVRefFloat : public MVRefBase
{
public:
	float Get() { return mv->GetFloat(); } 
	void Set( float fv ) { mv->SetFloat( fv ); }
};

///Value Reference for Float objects.
class MVRefString : public MVRefBase
{
public:
	const MString & Get() { return mv->GetString(); } 
	void Set( const MString & sv ) { mv->SetString( sv ); }
};

template< typename T >
class MVRefPtr : public MVRefBase
{
public:
	T * Get() {
		return (T*)mv->GetPtr();
	}
	void Set( T* iv, DelegateNotifierList * del = 0 ) { mv->SetPtr( iv, del ); }
};

#endif


/****************************************************************************
 *   Copyright (C) 2010 by Charles Lohr                                     *
 *                                                                          *
 *                                                                          *
 *   All rights reserved.                                                   *
 *                                                                          *
 *   Redistribution and use in source and binary forms, with or without     *
 *   modification, are permitted provided that the following conditions     *
 *   are met:                                                               *
 *     * Redistributions of source code must retain the above copyright     *
 *      notice, this list of conditions and the following disclaimer.       *
 *     * Redistributions in binary form must reproduce the above            *
 *      copyright notice, this list of conditions and the following         *
 *      disclaimer in the documentation and/or other materials provided     *
 *      with the distribution.                                              *
 *     * Neither the name of the Mercury Engine nor the names of its        *
 *      contributors may be used to endorse or promote products derived     *
 *      from this software without specific prior written permission.       *
 *                                                                          *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS    *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT      *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR  *
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT   *
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  *
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT       *
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,  *
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY  *
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT    *
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE  *
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.   *
 ***************************************************************************/
