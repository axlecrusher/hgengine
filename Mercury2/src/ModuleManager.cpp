#include <ModuleManager.h>
#include <XMLParser.h>

#ifndef WIN32
#include <dlfcn.h>
#else
#include <windows.h>

//These are only synonyms for the POSIX stuff.
#define RTLD_NOW 0
#define RTLD_GLOBAL 0

void * dlopen( const char * sFile, int dummy ) { return LoadLibrary( (LPCWSTR)sFile ); }
void dlclose( void * handle ) { FreeLibrary( (HMODULE)handle ); }
void * dlsym( void * handle, const char * sym ) { return GetProcAddress( (HMODULE) handle, sym ); }

#endif


extern "C"
{
typedef void * (*LoaderFunction)();	//Returns vtable pointer
};

ModuleManager::ModuleManager()
{
}

ModuleManager::~ModuleManager()
{
	MVector < MString > vOut;
	m_hAllHandles.VectorIndices( vOut );
	for( unsigned i = 0; i < vOut.size(); i++ )
	{
		dlclose( m_hAllHandles[vOut[i]] );
	}
}

ModuleManager & ModuleManager::GetInstance()
{
	static ModuleManager *instance = NULL;
	if (!instance)
		instance = new ModuleManager();
	return *instance;
}

void ModuleManager::InitializeAllModules()
{
	AutoMutexLock lock(m_mHandleMutex);

	XMLDocument* doc = XMLDocument::Load("modules.xml");
	XMLNode r = doc->GetRootNode();
	for (XMLNode child = r.Child(); child.IsValid(); child = child.NextNode())
	{
		if( child.Name() != "Module" )
		{
			fprintf( stderr, "Invalid element in modules: %s\n", child.Name().c_str() );
		}
		printf( "Loading: %s\n", child.Attribute( "obj" ).c_str() );
#ifdef WIN32
		MString ModuleName = child.Attribute( "obj" ) + ".dll";
#else
		MString ModuleName = child.Attribute( "obj" ) + ".so";
#endif

		MString LoadFunct = child.Attribute( "func" );

		m_hModuleMatching[child.Attribute( "obj" )] = child.Attribute( "class" );
		m_hClassMatching[child.Attribute( "class" )] = ModuleName;
		m_hClassMFunction[child.Attribute( "class" )] = LoadFunct;
		LoadModule( ModuleName, LoadFunct );
	}

	delete doc;
}

void ModuleManager::UnloadModule( const MString & ModuleName )
{
	if( m_hAllHandles[ModuleName] )
		dlclose( m_hAllHandles[ModuleName] );
}

void * ModuleManager::LoadModule( const MString & ModuleName, const MString & LoadFunction )
{
	{
		//scope mutex lock
		AutoMutexLock lock(m_mHandleMutex);

		if( m_hAllHandles[ModuleName] ) UnloadModule( ModuleName );

		void * v = dlopen( ModuleName.c_str(), RTLD_NOW | RTLD_GLOBAL );
		m_hAllHandles[ModuleName] = v;

		if( !m_hAllHandles[ModuleName] )
		{
#ifdef WIN32
			fprintf( stderr, "Error opening: %s\n", ModuleName.c_str() );
#else
			fprintf( stderr, "Error opening: %s (%s)\n", ModuleName.c_str(), dlerror() );
#endif

			return false;
		}
	}

	//If no load funct, just exit early.
	if( LoadFunction == "" )
		return 0;

	LoaderFunction T = (LoaderFunction)dlsym( m_hAllHandles[ModuleName], LoadFunction.c_str() );
	if( !T )
	{
		fprintf( stderr, "Error finding: %s() in %s\n", LoadFunction.c_str(), ModuleName.c_str() );
		return false;
	}

	void * ret = T();
	if( !ret )
	{
		fprintf( stderr, "Error executing (Returned error %p): %s() in %s\n", ret, LoadFunction.c_str(), ModuleName.c_str() );
		return false;
	}

	return ret;
}


#ifdef INSTANCE_WATCH

void ModuleManager::ReloadModule( const MString & sClass )
{
	AutoMutexLock lock(m_mHandleMutex);

	std::set< void * > & s = m_hAllInstances[sClass];
	std::set< void * >::iterator i = s.begin();

	void * newvtable = LoadModule( m_hClassMatching[sClass], m_hClassMFunction[sClass] );

	for( ; i != s.end(); i++ )
		*((void**)(*i)) = newvtable;
}

void ModuleManager::RegisterInstance( void * instance, const char * sClass )
{
	m_hAllInstances[sClass].insert( instance );
	m_pAllInstanceTypes[instance] = sClass;
}

void ModuleManager::UnregisterInstance( void * instance )
{
	std::map< void *, const char * >::iterator ni = m_pAllInstanceTypes.find( instance );

	//Object was never registered.
	if( ni == m_pAllInstanceTypes.end() )
		return;

	const char * sClass = ni->second;
	std::set< void * > & s = m_hAllInstances[sClass];
	std::set< void * >::iterator i = s.find( instance );

	if( i != s.end() )
		s.erase( i );
}

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
