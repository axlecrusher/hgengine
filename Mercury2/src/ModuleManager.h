#ifndef _MODULE_LOADER_H
#define _MODULE_LOADER_H

#include <MercuryUtil.h>
#include <MercuryHash.h>
#include <MercuryThreads.h>
#include <InstanceCounter.h>

#ifdef INSTANCE_WATCH
#include <set>
#include <map>
#endif

/* This is the module loader mechanism.  This allows for run-time loading of
   new modules.  Eventually, it will allow for run-time re-loading of modules.
   For now, it uses modules to determine what to load at startup. */

class ModuleManager
{
public:
	ModuleManager();
	~ModuleManager();
	static ModuleManager & GetInstance();
	void InitializeAllModules();

	void * LoadModule( const MString & ModuleName, const MString & LoadFunction );
	void UnloadModule( const MString & ModuleName );

#ifdef INSTANCE_WATCH
	void ReloadModule( const MString & sClass );

	void RegisterInstance( void * instance, const char * sClass );
	void UnregisterInstance( void * instance );
	const char * GetInstanceType( void * inst ) { return m_pAllInstanceTypes[inst]; }
#endif

private:

	MercuryMutex m_mHandleMutex;

#ifdef INSTANCE_WATCH
	MHash< std::set< void * > > m_hAllInstances;
	std::map< void *, const char * > m_pAllInstanceTypes;
#endif

	MHash< void * > m_hAllHandles;
	MHash< MString > m_hModuleMatching;
	MHash< MString > m_hClassMatching;
	MHash< MString > m_hClassMFunction;
};

static InstanceCounter<ModuleManager> ModMancounter("ModuleManager");

#ifdef INSTANCE_WATCH

#define NEW_INSTANCE( node, t ) ModuleManager::GetInstance().RegisterInstance( node, t );
#define DEL_INSTANCE( node ) ModuleManager::GetInstance().UnregisterInstance( node );

#else

#define NEW_INSTANCE( t )
#define DEL_INSTANCE( t )

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

#endif
