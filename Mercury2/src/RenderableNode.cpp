#include <RenderableNode.h>
#include <assert.h>
#include <GL/gl.h>
#include <TransformNode.h>

using namespace std;

REGISTER_NODE_TYPE(RenderableNode);

uint64_t RenderWaited = 0;
uint64_t UpdateWaited = 0;

RenderableNode::RenderableNode()
	:m_hidden(false)
{
}

RenderableNode::~RenderableNode()
{
	m_prerender.clear();
	m_render.clear();
	m_postrender.clear();
}

void RenderableNode::Update(float dTime)
{
	static unsigned long waitTime = 0;
	MSemaphoreIncOnDestroy s( &m_semaphore );
//	if ( Spinlock(0) ) ++UpdateWaited;	
//	if ( SpinlockWait(0, 100000) ) ++UpdateWaited;
//	UpdateWaited += Spinlock(0);
	
	int unsigned long waited = SpinlockWait(0, waitTime);
	if (waited > 0)
		waitTime = (waitTime<1000000)?waitTime+1000:waitTime;
	else
		waitTime = (waitTime!=0)?waitTime-1000:0;
	
	UpdateWaited += waited;
}

void RenderableNode::Render()
{
	list< MercuryAsset* >::iterator i;
	
	if (m_hidden || IsCulled()) return;
	
	MercuryMatrix m = FindGlobalMatrix();
	m.Transpose();
	glLoadMatrixf( m.Ptr() );
	
	for (i = m_prerender.begin(); i != m_prerender.end(); ++i )
		(*i)->PreRender(this);

	for (i = m_render.begin(); i != m_render.end(); ++i )
		(*i)->Render(this);
	
	for (i = m_postrender.begin(); i != m_postrender.end(); ++i )
		(*i)->PostRender(this);
}

const MercuryMatrix& RenderableNode::FindGlobalMatrix() const
{
	MercuryNode* n = NULL;
	TransformNode* tn;
	for (n = Parent(); n; n = n->Parent())
		if ( (tn = TransformNode::Cast(n)) )
			return tn->GetGlobalMatrix();

	return MercuryMath::IdentityMatrix;
}

void RenderableNode::AddPreRender(MercuryAsset* asset)
{
#ifdef MCHECKASSETS
	if ( !IsInAssetList(asset) ) //yell and scream
		assert(!"Asset does not exist in list!");
#endif
		
	m_prerender.push_back(asset);
}

void RenderableNode::AddRender(MercuryAsset* asset)
{
#ifdef MCHECKASSETS
	if ( !IsInAssetList(asset) ) //yell and scream
		assert(!"Asset does not exist in list!");
#endif
	
	m_render.push_back(asset);
}
void RenderableNode::AddPostRender(MercuryAsset* asset)
{
#ifdef MCHECKASSETS
	if ( !IsInAssetList(asset) ) //yell and scream
		assert(!"Asset does not exist in list!");
#endif
	
	m_postrender.push_back(asset);
}

bool RenderableNode::IsInAssetList( MercuryAsset* asset ) const
{
	std::list< MAutoPtr< MercuryAsset > >::const_iterator i;
	for (i = m_assets.begin(); i != m_assets.end(); ++i )
		if ( (*i) == asset ) return true;
	return false;
}

void RenderableNode::RecursiveRender( MercuryNode* n )
{
	static unsigned long waitTime = 0;
	RenderableNode* rn;
	if ( rn = Cast(n) )
	{
		MSemaphoreDecOnDestroy s( &(rn->m_semaphore) );
		
		int unsigned long waited = rn->SpinlockWait(1, waitTime);
		if (waited > 0)
			waitTime = (waitTime<1000000)?waitTime+1000:waitTime;
		else
			waitTime = (waitTime!=0)?waitTime-1000:0;
	
		RenderWaited += waited;

//		++RenderWaited += rn->Spinlock(1);
		rn->Render();
	}
	
	const list< MercuryNode* >& children = n->Children();
	list< MercuryNode* >::const_iterator i;
	
	for (i = children.begin(); i != children.end(); ++i )
		RenderableNode::RecursiveRender( *i );
}

void RenderableNode::LoadFromXML(const XMLNode& node)
{	
	if ( !node.Attribute("hidden").empty() )
		m_hidden = node.Attribute("hidden")=="true"?true:false;
	
	for (XMLNode child = node.Child(); child.IsValid(); child = child.NextNode())
	{
		if ( child.Name() == "asset" )
		{
			MAutoPtr< MercuryAsset > asset( AssetFactory::GetInstance().Generate(child.Attribute("type") ) );
			if ( asset.IsValid() )
			{
				asset->LoadFromXML( child );
				this->AddAsset( asset );
				asset->Init( this );
			}
		}
	}
	
	MercuryNode::LoadFromXML( node );
}

unsigned long RenderableNode::Spinlock( unsigned long value )
{
	unsigned long waited = 0;
	while (m_semaphore.ReadValue() != value) ++waited;
	return waited;
}

unsigned long RenderableNode::SpinlockWait( unsigned long value, unsigned long usec )
{
	unsigned long waited = 0;
	while (m_semaphore.ReadValue() != value)
	{
//		waited=true;
		++waited;
		if (usec>0) usleep(usec);
	}
	return waited;
}



/***************************************************************************
 *   Copyright (C) 2008 by Joshua Allen   *
 *      *
 *                                                                         *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: *
 *     * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. *
 *     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. *
 *     * Neither the name of the <ORGANIZATION> nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission. *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR *
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR *
 *   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, *
 *   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,   *
 *   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR    *
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF *
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING  *
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS    *
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.          *
 ***************************************************************************/
