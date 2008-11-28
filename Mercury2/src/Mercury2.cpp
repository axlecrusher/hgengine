#include <MercuryWindow.h>
#include <Quad.h>
#include <RenderableNode.h>
#include <Viewport.h>
#include <TransformNode.h>

int main()
{
	unsigned long m_count = 0;
	unsigned long m_time;
	
	MercuryWindow* w = MercuryWindow::MakeWindow();
	MercuryNode* root = new MercuryNode();
	Viewport* vp = new Viewport();
	RenderableNode* r = new RenderableNode();
	TransformNode* t = new TransformNode();
	MAutoPtr< MercuryAsset > q( new Quad() );
	
	t->SetPosition( MercuryPoint(0,0,-1) );
	
	vp->Perspective(45,640.0f/480.0f,0.01,100);
	root->AddChild(vp);
	vp->AddChild(t);
	t->AddChild(r);
	r->AddAsset( q );
	r->AddRender( q );
	
	m_time = time(NULL);
	do
	{
		root->RecursiveUpdate(0.01f); //1500FPS
		RenderableNode::RecursiveRender(root); //870FPS
		w->SwapBuffers();
		++m_count;
		
		if (time(NULL) > m_time)
		{
			m_time = time(NULL);
			printf("FPS: %d\n", m_count);
			m_count = 0;
		}
	}
	while ( w->PumpMessages() );

	delete w;
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
