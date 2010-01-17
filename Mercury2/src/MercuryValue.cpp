#include <MercuryMessageManager.h>
#include <MercuryValue.h>
#include <MessageHandler.h>


DelegateNotifierList::DelegateNotifierList( DeletionNotifier nf, MessageHandler * no ) :
	NotifyFunction( nf ), NotifyObject( no ), Next( NULL )
{
}

DelegateNotifierList::~DelegateNotifierList()
{
	SAFE_DELETE( Next );
}

void DelegateNotifierList::AddNotifier( DeletionNotifier nf, MessageHandler * no )
{
	DelegateNotifierList * nn = new DelegateNotifierList( nf, no );
	nn->Next = Next;
	Next = nn;
}

void DelegateNotifierList::DelNotifier( DeletionNotifier nf, MessageHandler * no, DelegateNotifierList * & ths )
{
	if( nf == NotifyFunction && no == NotifyObject )
	{
		ths = Next;
		delete this;
		return;
	}
	if( Next )
		Next->DelNotifier( nf, no, Next );
}

void DelegateNotifierList::Notify( MValue * v )
{
	(NotifyObject->*NotifyFunction)( v );
	if( Next )
		Next->Notify( v );
}





MValue::MValue( ) : m_References( NULL ), m_CurType( TYPE_UNDEF ), DLDelete( NULL ), DLModify( NULL )
{
	m_Data.v = NULL;
}

MValue::MValue( MVType t ) : m_References( NULL ), m_CurType( t ), DLDelete( NULL ), DLModify( NULL )
{
	m_Data.v = NULL;
}

MValue::~MValue()
{
	Cleanup();
	SAFE_DELETE( DLModify );
	SAFE_DELETE( DLDelete );
}

void MValue::Cleanup()
{
	if( DLDelete )
		DLDelete->Notify( this );
	SAFE_DELETE( DLDelete );

	if( m_CurType == TYPE_STRING )
		SAFE_DELETE( m_Data.dataS );
}

int MValue::ConvInt()
{
	switch( m_CurType )
	{
	case TYPE_INT: 	   return (int)m_Data.l;
	case TYPE_FLOAT:   return m_Data.f;
	case TYPE_STRING:  return StrToInt(*m_Data.dataS);
	default:           return 0;
	}
}

float MValue::ConvFloat()
{
	switch( m_CurType )
	{
	case TYPE_INT: 	   return (float)m_Data.l;
	case TYPE_FLOAT:   return m_Data.f;
	case TYPE_STRING:  return StrToFloat(*m_Data.dataS);
	default:           return 0;
	}
}


const MString MValue::ConvString()
{
	switch( m_CurType )
	{
	case TYPE_STRING:  return *m_Data.dataS;
	case TYPE_INT:     return ssprintf( "%d", m_Data.l );
	case TYPE_FLOAT:   return ssprintf( "%.2f", m_Data.f );
	case TYPE_PTR:     return ssprintf( "%p", m_Data.v );
	default:           return "(NIL)";
	}
}

void MValue::ConvString( MString & ret )
{
	switch( m_CurType )
	{
	case TYPE_STRING:  ret = *m_Data.dataS;
	case TYPE_INT:     ret = ssprintf( "%d", m_Data.l );
	case TYPE_FLOAT:   ret = ssprintf( "%.2f", m_Data.f );
	case TYPE_PTR:     ret = ssprintf( "%p", m_Data.v );
	default:           ret = "(NIL)";
	}
}


bool MValue::ConvBool()
{
	switch( m_CurType )
	{
	case TYPE_INT: 	   return m_Data.l != 0;
	case TYPE_FLOAT:   return m_Data.f != 0;
	case TYPE_STRING:  return StrToBool(*m_Data.dataS);
	default:           return 0;
	}
}



void MValue::AttachModifyDelegate( DeletionNotifier NotifyFunction, MessageHandler * NotifyObject )
{
	DelegateNotifierList * d = new DelegateNotifierList( NotifyFunction, NotifyObject );
	d->Next = DLModify;
	DLModify = d;
}

void MValue::DetachModifyDelegate( DeletionNotifier NotifyFunction, MessageHandler * NotifyObject )
{
	DLModify->DelNotifier( NotifyFunction, NotifyObject, DLModify );
}



MVRefBase::MVRefBase(const MString & sPath)
{
	mv = MESSAGEMAN.GetValue( sPath );
	MSemaphoreLock( &mv->m_Sema );
	mv->m_References++;
}


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

