#include <ParticleEmitter.h>
#include <Shader.h>
#include <GLHeaders.h>

#include <MercuryVBO.h>
#include <Texture.h>

#include <list>
using namespace std;
//REGISTER_NODE_TYPE(ParticleBase);
REGISTER_NODE_TYPE(ParticleEmitter);

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

ParticleBase::ParticleBase()
	:m_age(0), m_lifespan(0), m_particleVobData(NULL), m_particleIndexData(NULL)
{
}

ParticleBase::~ParticleBase()
{
	m_particleVobData = NULL;
	m_particleIndexData = NULL;
}

void ParticleBase::Init()
{
	m_age = 0;
}

void ParticleBase::Update(float dTime)
{
	m_age += dTime;
	WriteAgeToVBO();

	if (m_age >= m_lifespan) Deactivate();
}

void ParticleBase::WriteAgeToVBO()
{
	m_emitter->SetDirtyVertices();
	for (uint8_t i = 0; i < 4; ++i)
		WriteFloatToVertices(m_age,i,3);
}

void ParticleBase::WriteLifespanToVBO()
{
	m_emitter->SetDirtyVertices();
	for (uint8_t i = 0; i < 4; ++i)
		WriteFloatToVertices(m_lifespan,i,4);
}

void ParticleBase::WriteRand1ToVBO()
{
	m_emitter->SetDirtyVertices();
	for (uint8_t i = 0; i < 4; ++i)
		WriteFloatToVertices(m_rand1,i,5);
}

void ParticleBase::WriteRand2ToVBO()
{
	m_emitter->SetDirtyVertices();
	for (uint8_t i = 0; i < 4; ++i)
		WriteFloatToVertices(m_rand2,i,6);
}

void ParticleBase::Activate()
{
//	printf("Activate\n");
	uint8_t i = 0;

	//upper left
	m_particleVobData[i++] = -0.5; m_particleVobData[i++] = 0.5; m_particleVobData[i++] = 0;
	i+=4; m_particleVobData[i++] = 1; m_particleVobData[i++] = 0; //skip color data and set U,V

	//lower left
	m_particleVobData[i++] = -0.5; m_particleVobData[i++] = -0.5; m_particleVobData[i++] = 0;
	i+=4; m_particleVobData[i++] = 0; m_particleVobData[i++] = 0; //skip color data and set U,V

	//lower right
	m_particleVobData[i++] = 0.5; m_particleVobData[i++] = -0.5; m_particleVobData[i++] = 0;
	i+=4; m_particleVobData[i++] = 0; m_particleVobData[i++] = 1; //skip color data and set U,V

	//upper right
	m_particleVobData[i++] = 0.5; m_particleVobData[i++] = 0.5; m_particleVobData[i++] = 0;
	i+=4; m_particleVobData[i++] = 1; m_particleVobData[i++] = 1; //skip color data and set U,V

	for (uint8_t i = 1; i < 4; ++i)
		m_particleIndexData[i] = m_particleIndexData[0]+i; //reconstruct indices

	m_emitter->SetDirtyVertices();
	m_emitter->SetDirtyIndices();
}

void ParticleBase::Deactivate()
{
	for (uint8_t i = 1; i < 4; ++i)
		m_particleIndexData[i] = m_particleIndexData[0]; //degenerate triangle renders nothing
	m_emitter->SetDirtyIndices();
}

void ParticleBase::WriteFloatToVertices(float v, uint8_t vertexIndex, uint8_t offset)
{
	*(m_particleVobData+((STRIDE*vertexIndex)+offset)) = v;
}

void ParticleBase::WriteToVBO()
{
	WriteAgeToVBO();
	WriteLifespanToVBO();
	WriteRand1ToVBO();
	WriteRand2ToVBO();
}

ParticleEmitter::ParticleEmitter()
	:base(), m_maxParticles(50), m_age(0), m_emitDelay(0.1f), m_lifespan(0),
	m_particlesEmitted(0), m_particleMinLife(0.1f), m_particleMaxLife(5.0f),
	m_particles(NULL), GenerateParticlesClbk(NULL),
	m_dirtyVBO(0)
{
	m_bufferID[0] = 0;
	m_iForcePasses = m_iForcePasses | (1<<15);
	m_iForcePasses = m_iForcePasses | (1<<7);
	Init();
}


ParticleEmitter::~ParticleEmitter()
{
	if (m_bufferID[0] > 0) { GLCALL( glDeleteBuffersARB(2, m_bufferID) ); }

	SAFE_DELETE_ARRAY(m_particles); //do we need to destroy each element????
	SAFE_DELETE(GenerateParticlesClbk);
}


void ParticleEmitter::Init()
{
	MercuryNode::Init();
	SAFE_DELETE_ARRAY(m_particles); //do we need to destroy each element????

	SetMaxParticleCount(m_maxParticles);
}

void ParticleEmitter::Update(float dTime)
{
	m_age += dTime;

	/* create particles until we meet the total number of
	   particles possible in terms of the age of the emitter */
	while (((m_age-(m_particlesEmitted*m_emitDelay)) > m_emitDelay) && (m_emitDelay>0))
	{
//		LOG.Write("Emit");
		++m_particlesEmitted;  //always increment even if the maximum number of particles exist
		ActivateParticle();
	}
	
	list< ParticleBase* >::iterator i = m_active.begin();
	while ( i!=m_active.end() )
	{
		ParticleBase *p = *i;
		p->Update(dTime);
		if ( !p->IsActive() )
		{
			m_active.erase(i++); //don't invalidate iterator before incrementing it
			m_inactive.push_back(p);
		}
		else
		{
			++i;
		}
	}
}

void ParticleEmitter::ActivateParticle()
{
	if (!m_inactive.empty())
	{
		ParticleBase* p = m_inactive.front();
		m_inactive.pop_front();
		
		p->Init();
		p->Activate();
		
		p->m_lifespan = m_particleMinLife;
		p->m_lifespan += (rand()%(int(m_particleMaxLife*1000) - int(m_particleMinLife*1000)))/1000.0f;
		p->m_rand1 = float(rand()%100000);
		p->m_rand2 = float(rand()%100000);
//		+((rand()%((m_particleMaxLife*1000)-(m_particleMinLife*1000)))/1000.0f);


		p->WriteAgeToVBO();
		p->WriteLifespanToVBO();
		p->WriteRand1ToVBO();
		p->WriteRand2ToVBO();

		//add to the active list
//		printf("push %p\n", p);
		m_active.push_back(p);
	}
}

void ParticleEmitter::LoadFromXML(const XMLNode& node)
{
	base::LoadFromXML(node);
/*	
	XMLNode particleXML;
	for (XMLNode n = node.Child(); n.IsValid(); n=n.NextNode())
		if (n.Name() == "particle")
			particleXML = n;
	
	for (uint32_t i = 0; (i < m_maxParticles) && m_particles; ++i)
		m_particles[i].LoadFromXML(particleXML);
		*/
}

void ParticleEmitter::SetMaxParticleCount(uint16_t count)
{
	//3 floats for position
	//1 age, 1 lifespan, 2 random
	//7 floats total per particle

	m_maxParticles = count;
	m_vertexData.Allocate(m_maxParticles*ParticleBase::STRIDE*4);
	m_indexData.Allocate(m_maxParticles*4);

	SAFE_DELETE_ARRAY(m_particles);
//	if (GenerateParticlesClbk) m_particles = (*GenerateParticlesClbk)(m_maxParticles);
	m_particles = new ParticleBase[m_maxParticles];
//	m_inactiveParticles = m_particles;
//	InitNewParticles(m_particles, 0, ParticleBase::STRIDE*4, m_vertexData.Buffer());

	m_inactive.clear();
	m_active.clear();
	
	for (uint32_t i = 0; i < m_maxParticles; ++i)
	{
		ParticleBase* p = m_particles+i;
		p->m_emitter = this;
		p->m_particleVobData = m_vertexData.Buffer()+(ParticleBase::STRIDE*4*i);
		p->m_particleIndexData = m_indexData.Buffer() + (4*i);
		p->m_particleIndexData[0]=p->m_particleIndexData[1]=p->m_particleIndexData[2]=p->m_particleIndexData[3]=i*4; //initial degenerate
		p->Deactivate();
//		printf("addr1 %p\n", p);
		m_inactive.push_back( p );
	}
}

void ParticleEmitter::PreRender(const MercuryMatrix& matrix)
{
	MercuryNode::PreRender(matrix);
	SetCulled(false);
}

void ParticleEmitter::Render(const MercuryMatrix& matrix)
{
	GLCALL( glPushAttrib(GL_ENABLE_BIT|GL_CURRENT_BIT) );
	GLCALL( glDisable(GL_CULL_FACE) );
	
	if (m_bufferID[0]==0)
	{
		GLCALL( glGenBuffersARB(2, m_bufferID) );
	}

	GLCALL( glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_bufferID[0]) );
	GLCALL( glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_bufferID[1]) );
//	GLCALL( glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, 0) );

	MercuryVBO::SetLastRendered(this);

	if (m_dirtyVBO&&0x1)
	{
		GLCALL( glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_vertexData.LengthInBytes(), m_vertexData.Buffer(), GL_STREAM_DRAW_ARB) );
	}
	if (m_dirtyVBO&&0x2)
	{
		GLCALL( glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_indexData.LengthInBytes(), m_indexData.Buffer(), GL_STREAM_DRAW_ARB) );
	}
	m_dirtyVBO = 0;
	GLCALL( glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT) );

	//do render stuff here
	Texture::ApplyActiveTextures(ParticleBase::STRIDE*sizeof(float), 7*sizeof(float));

	GLCALL( glEnableClientState(GL_VERTEX_ARRAY) );
	GLCALL( glEnableClientState( GL_COLOR_ARRAY ) ); //used for attributes
//	GLCALL( glDisableClientState( GL_NORMAL_ARRAY ) );

	GLCALL( glVertexPointer(3, GL_FLOAT, ParticleBase::STRIDE*sizeof(float), BUFFER_OFFSET( 0*sizeof(float) ) ) );
	GLCALL( glColorPointer(4, GL_FLOAT, ParticleBase::STRIDE*sizeof(float), BUFFER_OFFSET( 3*sizeof(float) ) ) );

//	GLCALL( glDrawArrays(GL_QUADS, 0, m_maxParticles*4) );
//	GLCALL( glDrawRangeElements( GL_QUADS, 0, m_maxParticles*4,m_maxParticles*4,  GL_UNSIGNED_SHORT, NULL) );
	GLCALL( glDrawElements(GL_QUADS,m_maxParticles*4,GL_UNSIGNED_SHORT,0) );

	GLCALL( glPopClientAttrib() );
	GLCALL( glPopAttrib() );

	m_particlesDrawn+=m_maxParticles;
	MercuryVBO::IncrementBatches();

	base::Render(matrix);
}

uint32_t ParticleEmitter::ResetDrawnCount()
{
	uint32_t t = m_particlesDrawn;
	m_particlesDrawn = 0;
	return t;
}

uint32_t ParticleEmitter::m_particlesDrawn = 0;

/****************************************************************************
 *   Copyright (C) 2009 by Joshua Allen                                     *
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
