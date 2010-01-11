#include <MercuryWindow.h>
#include <Quad.h>
//#include <RenderableNode.h>
#include <Viewport.h>
#include <TransformNode.h>

#include <XMLParser.h>

#include <RenderGraph.h>

#include <MercuryCrash.h>
#include <MercuryBacktrace.h>
#include <MercuryMessageManager.h>
#include <MercurySound.h>
#include <MercuryTimer.h>
#include <RenderGraph.h>
#include <Texture.h>
#include <GLHeaders.h>
#include <ModuleManager.h>
#include <MercuryFile.h>

#include <MercuryLog.h>

#define MULTIPASS

bool SHOWBOUNDINGVOLUME = false;
bool SHOWAXISES = false;
bool DOOCCLUSIONCULL = false;

MSemaphore UpdateLoopGo;
void* UpdateThread(void* node)
{
	while( UpdateLoopGo.Read() < 1 )
	{
		MercuryNode* n = (MercuryNode*)node;
		n->RecursiveUpdate(0.01f);
	}
	return NULL;
}

int SignalHandler( int signal )
{
	char buffer[2048];
	LOG.Write(ssprintf( "Fatal error encountered in Mercury 2:  %s", cn_get_crash_description( signal ) ));
	cnget_backtrace( 1, buffer, 2047 );

	//XXX: Sometimes we produce a crash, we get the crash report, and the next line doesn't work... This should be examined.
	LOG.Write( buffer );

	return 0;	//Continue regular crash.
}

MString g_SceneGraphToLoad;
void HandleCommandLineParameters( int argc, char ** argv )
{
	bool bPrintHelp = false;
	for( int i = 1; i < argc; i++ )
	{
		MString sParameter = argv[i];
		if( sParameter == "--help" || sParameter == "-h" )
			bPrintHelp = true;
		else if( sParameter.find( "--scenegraph=" ) == 0 )
			g_SceneGraphToLoad = sParameter.substr( 13 );
		else if( sParameter == "-s" )
		{
			i++;
			if( i >= argc )
			{
				fprintf( stderr, "No scene graph following -s. Type -h for help.\n" );
				exit(-1);
			}
			g_SceneGraphToLoad = argv[i];
		}
		else
		{
			fprintf( stderr, "Unknown command-line parameter: \"%s\" Type -h for help.\n", argv[i] );
			exit(-1);
		}
	}

	if( bPrintHelp )
	{
		printf( "\nMercury Game Engine 2.0 Copyright 2009 Joshua Allen and\n" );
		printf( "  Charles Lohr under the NewBSD license.  Code contributed\n" );
		printf( "  from other sources under the same license, contributers\n" );
		printf( "  and other sources may be found in accompanying documents.\n" );
		printf( "\n  Usage:  [mercury] [command-line parameters]\n\n" );
		printf( "  [--scenegraph=x] | [-s x] Select scene graph to use.\n" );
		printf( "    By default Mercury uses FILE:scenegraph.xml.  You may\n" );
		printf( "    specify another one here.  This only changes the startup\n" );
		printf( "    scene graph for debugging purposes.\n\n" );
		exit(0);
	}
}

int main( int argc, char** argv)
{
	unsigned long m_count = 0;

	g_SceneGraphToLoad = "FILE:scenegraph.xml";

	cnset_execute_on_crash( SignalHandler );
	HandleCommandLineParameters( argc, argv );

	//Sound first.
	SOUNDMAN->Init( "" );
	MercuryWindow* w = MercuryWindow::MakeWindow();

#ifdef WIN32
	SetupOGLExtensions();
#endif

	ModuleManager::GetInstance().InitializeAllModules();

	MercuryNode* root = new MercuryNode();
	
	XMLDocument* doc = XMLDocument::Load(g_SceneGraphToLoad);
	XMLNode r = doc->GetRootNode();
	root->LoadFromXML( r );

	SAFE_DELETE(doc);
	
	MercuryTimer timer;
	MercuryTimer fpsTimer;
	RenderGraph renderGraph;
		
	//uncomment the next 2 lines to use threads
//	MercuryThread updateThread;
//	updateThread.Create( UpdateThread, root, false);
	do
	{
		timer.Touch();
		MESSAGEMAN.PumpMessages( timer.MicrosecondsSinceInit() );

		root->RecursiveUpdate( timer.Age() ); //comment to use threads
	
		CURRENTRENDERGRAPH = &renderGraph;
		if ( MercuryNode::NeedsRebuild() )
		{
			renderGraph.Build(root);
		}

		w->Clear();

#ifdef MULTIPASS
		for( g_iPass = 2; g_iPass < 5; g_iPass++ )	//2,3,4
			if( root->GetPasses() & (1<<g_iPass) )
				root->RecursiveRender( );
#endif
		root->RecursivePreRender();

#ifndef MULTIPASS
		root->RecursiveRender();
#else		
		for( g_iPass = 5; g_iPass < 15; g_iPass++ )	//5..15
			if( root->GetPasses() & (1<<g_iPass) )
				root->RecursiveRender( );
#endif
		
		w->SwapBuffers();


		++m_count;
		
		fpsTimer.Touch(timer);
		if (fpsTimer.Age() > 1)
		{
			float batches = MercuryVBO::ResetBatchCount()/(float)m_count;
			float VBinds = MercuryVBO::ResetBindCount()/(float)m_count;
			float Tbinds = Texture::ReadAndResetBindCount()/(float)m_count;
			LOG.Write( ssprintf("FPS: %f, VBO batches %f, TBinds %f, VBinds %f", m_count/fpsTimer.Age(), batches, Tbinds, VBinds) );
#ifdef GL_PROFILE
			float GLcalls = GLCALLCOUNT/(float)m_count;
			GLCALLCOUNT = 0;
			LOG.Write( ssprintf("GL/f: %f", GLcalls) );
#endif
			m_count = 0;
			fpsTimer = timer;
		}

//Uncomment to enable scenegraph saving.
//		MString st;
//		root->SaveToXML( st );
//		StringToFile( "test.xml", st );
	}
	while ( w->PumpMessages() );
	
	//uncomment the next 2 lines to use threads
//	UpdateLoopGo.Increment();
//	updateThread.Wait();

	PrintGLFunctionCalls();

	SAFE_DELETE(root);
	SAFE_DELETE(w);
	
//	uint64_t totalWaited = UpdateWaited + RenderWaited;
//	printf("Update wait %%%f\n", (UpdateWaited/double(totalWaited))*100.0f);
//	printf("Render wait %%%f\n", (RenderWaited/double(totalWaited))*100.0f);
	
	return 0;
}





///XXXXXXXX STUB CODE XXXXXXXXXXX  THIS CODE SHOULD BE REMOVED AS SOON AS TESTING OF THE VARIABLE SYSTEM IS COMPLETE
class TestMouse
{
public:
	TestMouse()
	{
		MESSAGEMAN.GetValue( "TestMode" )->AttachModifyDelegate( (ValueDelegate)&TestMouse::ChangeX, (MessageHandler*)this );
	}

	void ChangeX( MValue * v )
	{
		printf( "Changed: %s\n", v->GetString().c_str() );
	}
} TM;
///XXXXXXX REMOVE THIS CODE BEFORE ANY RELEASE XXXXXXXXXXx



/* Copyright (c) 2008, Joshua Allen
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

