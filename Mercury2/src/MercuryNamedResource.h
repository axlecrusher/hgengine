#ifndef _MERCURY_NAMED_RESOURCE
#define _MERCURY_NAMED_RESOURCE

#include <MercuryString.h>

///Base class for several named resources (I.e. Preferences, Theme, etc.)
class MercuryNamedResource
{
public:
	///Get a Value
	/** This function _must_ be overloaded by the base class, as there is no
	    mechanism for the named resource to figure this out. */
	virtual bool GetValue( const MString & sDataPointer, MString & sReturn ) = 0;

	///Set a named value.
	/** This function should be overloaded by the base class, unless you have no
	    means to write back.  For instance, the theme will simply ignore this
	    function. */
	virtual void SetValueS( const MString & sDataPointer, const MString & sValToSet ) { }

	///Retrieve a named value
	/** This function can be overloaded by the base class, otherwise, it
	    just makes approprate calls to GetValue */
	virtual MString GetValueS( const MString & sDataPointer, MString sDefaultValue, bool bSetValue = false );

	///Shorthand for retrieving a named value (calls GetValueS with null Parameters)
	/** This function may be overloaded if you wish to improve performance slightly
	    when users are simply doing reads and do not intend to set default values */
	virtual MString GetValueS( const MString & sDataPointer );

	///Retrieve a named value and convert it to a float.
	float GetValueF( const MString & sDataPointer, float fDefaultValue = 0.0f, bool bSetValue = false );

	///Set a float value on a named resource.
	void  SetValueF( const MString & sDataPointer, float fValue );

	///Retrieve a named value and convert it to a boolean.
	bool GetValueB( const MString & sDataPointer, bool bDefaultValue = false, bool bSetValue = false );

	///Set a float value on a named resource.
	void SetValueB( const MString & sDataPointer, bool bValue );

	///Retrieve a named value and convert it to a float.
	int  GetValueI( const MString & sDataPointer, int iDefaultValue = 0, bool bSetValue = false );

	///Set a float value on a named resource.
	void SetValueI( const MString & sDataPointer, int iValue );
};

#endif

/****************************************************************************
 *   Copyright (C) 2009 by Charles Lohr                                     *
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
