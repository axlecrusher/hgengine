#ifndef _VARIABLE_REGISTER_H
#define _VARIABLE_REGISTER_H

class VariableReceiver
{
public:
	void VariableDelegate( const MString & sChangedVariable, const MString & sChangedValue );
};

typedef void (VariableReceiver::*VariableDelegate)( const MString & sChangedVariable, const MString & sChangedValue );

///Variable Register
/**This is a tool that can be used by programmers and the like to control global variables.
   It is quite useful in situations where you want to be able to change variables without
   use of code, and be able to catch changes. */
class VariablesRegister
{
public:
	void RegisterListener( const MString & sVariable, VariableDelegate d );
	const MString & GetVariable( const MString & var ) { MString * s = m_mVariables->Get( var ); return s?*s:""; }
	static MercuryMessageManager& GetInstance();
private:
	MHash< MString > m_mVariables;

	struct Sender
	{
		VariableDelegate * d;
		
	};
	MHash< VariableDelegate > m_mDelegates;
	MHash< 
};



static InstanceCounter<VariablesRegister> MMcounter("VariablesRegister");

#define VARIABLES VariablesRegister::GetInstance()


#endif

/****************************************************************************
 *   Copyright (C) 2008 by Charles Lohr                                     *
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

