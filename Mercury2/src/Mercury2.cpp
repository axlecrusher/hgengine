#include <MercuryWindow.h>
#include <Quad.h>
#include <RenderableNode.h>
#include <Viewport.h>
#include <TransformNode.h>

#include <XMLParser.h>

#include <RenderableNode.h>

#include <MercuryCrash.h>
#include <MercuryBacktrace.h>
#include <MercuryMessageManager.h>

#include <MercuryTimer.h>
#include <RenderGraph.h>
#include <Texture.h>

bool SHOWBOUNDINGVOLUME = false;

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
	printf( "Fatal error encountered in Mercury 2:  %s\n", cn_get_crash_description( signal ) );
	cnget_backtrace( 1, buffer, 2047 );
	printf( "%s\n", buffer );

	return 0;	//Continue regular crash.
}

int main()
{
	unsigned long m_count = 0;
	
	cnset_execute_on_crash( SignalHandler );

	MercuryWindow* w = MercuryWindow::MakeWindow();
	MercuryNode* root = new MercuryNode();
	
	XMLDocument* doc = XMLDocument::Load("scenegraph.xml");
	
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
		MESSAGEMAN::GetInstance().PumpMessages( timer.MicrosecondsSinceInit() );
		root->RecursiveUpdate( timer.Age() ); //comment to use threads
		
		if ( MercuryNode::NeedsRebuild() )
		{
			renderGraph.Build(root);
		}
		
		
		renderGraph.Render();
//		RenderableNode::RecursiveRender(root);
		w->SwapBuffers();
		++m_count;
		
		fpsTimer.Touch(timer);
		if (fpsTimer.Age() > 1)
		{
			float batches = MercuryVBO::ResetBatchCount()/(float)m_count;
			float VBinds = MercuryVBO::ResetBindCount()/(float)m_count;
			float Tbinds = Texture::ReadAndResetBindCount()/(float)m_count;
			printf("FPS: %f, VBO batches %f, TBinds %f, VBinds %f\n", m_count/fpsTimer.Age(), batches, Tbinds, VBinds);
			m_count = 0;
			fpsTimer = timer;
		}
	}
	while ( w->PumpMessages() );
	
	//uncomment the next 2 lines to use threads
//	UpdateLoopGo.Increment();
//	updateThread.Wait();

	SAFE_DELETE(root);
	SAFE_DELETE(w);
	
//	uint64_t totalWaited = UpdateWaited + RenderWaited;
//	printf("Update wait %%%f\n", (UpdateWaited/double(totalWaited))*100.0f);
//	printf("Render wait %%%f\n", (RenderWaited/double(totalWaited))*100.0f);
	
	return 0;
}

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

