#ifndef _MERCURYTWEEN_H
#define _MERCURYTWEEN_H

#include <deque>
#include "MercuryUtil.h"

typedef float (*TweenFunction)( float, float );

template< typename T >
class MercuryTweenState
{
public:
	T		Target;

	TweenFunction	Function;
	float		FunctionP;

	float		Time;
};

template< typename T >
class MercuryTween
{
public:
	MercuryTween() : Current( 0 ), m_fInOnThisTween( 0 ){ }
	T * Current;

	void Attach( T * val )
	{
		Current = val;
		m_Last = *Current;
	}

	void Update( float fDelta )
	{
		if( !Current || !m_FutureTweens.size() )
			return;

		//The following few lines are incredibly convoluted.  I do not suggest trying to figure it out.
		m_fInOnThisTween += fDelta;

		while( m_FutureTweens.size() && m_fInOnThisTween > m_FutureTweens.front().Time )
		{
			m_Last = *Current;
			*Current = m_FutureTweens.front().Target;
			m_fInOnThisTween -= m_FutureTweens.front().Time;
			m_FutureTweens.pop_front();
		}

		if( !m_FutureTweens.size() )
		{
			m_fInOnThisTween = 0;
			m_Last = *Current;
			return;
		}

		MercuryTweenState<T> & ts = m_FutureTweens.front();

		SetTween( *Current, m_Last, ts.Target, ts.Function( m_fInOnThisTween/ts.Time, ts.FunctionP ) );
	}

	void Finish()
	{
		Current = m_FutureTweens.back().Target;
		m_FutureTweens.clear();
		m_fInOnThisTween = 0;
	}

	void Stop()
	{
		m_FutureTweens.clear();
		m_fInOnThisTween = 0;
	}

	void AddTween( MercuryTweenState< T > s )
	{
		m_FutureTweens.push_back( s );
	}

	void AddTween( MString & sTweenSet )
	{
		MVector < MString > out;
		SplitStrings( sTweenSet, out, ";\n", " \t", 2, 2 );
//		for( unsigned i = 0 
	}

private:
	float m_fInOnThisTween;
	T m_Last;
	std::deque< MercuryTweenState< T > > m_FutureTweens;
};



#endif


/* 
 * Copyright (c) 2010 Charles Lohr
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *	-	Redistributions of source code must retain the above
 *		copyright notice, this list of conditions and the following disclaimer.
 *	-	Redistributions in binary form must reproduce the above copyright
 *		notice, this list of conditions and the following disclaimer in
 *		the documentation and/or other materials provided with the distribution.
 *	-	Neither the name of the Mercury Engine nor the names of its
 *		contributors may be used to endorse or promote products derived from
 *		this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

