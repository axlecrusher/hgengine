#include <ParticleEmitter.h>
#include <Shader.h>
#include <GLHeaders.h>

#include <MercuryVBO.h>
#include <Texture.h>
#include <Shader.h>

#include <list>
using namespace std;
//REGISTER_NODE_TYPE(ParticleBase);
REGISTER_NODE_TYPE(ParticleEmitter);

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

ParticleBase::ParticleBase()
	:m_startTime(0), m_currentTime(0), m_lifespan(0), m_particleDynamicData(NULL), m_particleIndexData(NULL)
{
}

ParticleBase::~ParticleBase()
{
	m_particleDynamicData = NULL;
	m_particleIndexData = NULL;
}

void ParticleBase::Init(float time)
{
	//absolute time
	m_startTime = m_currentTime = time;
}

void ParticleBase::Update(float currentTime)
{
	m_currentTime = currentTime;
	if ( !IsActive() ) Deactivate();
}

void ParticleBase::WriteAgeToVBO()
{
	for (uint8_t i = 0; i < 4; ++i)
		WriteFloatToVertices(m_startTime,i,0);
}

void ParticleBase::WriteLifespanToVBO()
{
	for (uint8_t i = 0; i < 4; ++i)
		WriteFloatToVertices(m_lifespan,i,1);
}

void ParticleBase::WriteRand1ToVBO()
{
	for (uint8_t i = 0; i < 4; ++i)
		WriteFloatToVertices(m_rand1,i,2);
}

void ParticleBase::WriteRand2ToVBO()
{
	for (uint8_t i = 0; i < 4; ++i)
		WriteFloatToVertices(m_rand2,i,3);
}

void ParticleBase::Activate()
{
	for (uint8_t i = 1; i < 4; ++i)
		m_particleIndexData[i] = m_particleIndexData[0]+i; //reconstruct indices
	m_emitter->SetIndexUpdateRange(m_particleIndexData,m_particleIndexData+4);
}

void ParticleBase::Deactivate()
{
	for (uint8_t i = 1; i < 4; ++i)
		m_particleIndexData[i] = m_particleIndexData[0]; //degenerate triangle renders nothing
	m_emitter->SetIndexUpdateRange(m_particleIndexData,m_particleIndexData+4);
}

void ParticleBase::WriteFloatToVertices(float v, uint8_t vertexIndex, uint8_t offset)
{
	unsigned long o = (4*vertexIndex)+offset;
	*(m_particleDynamicData+o) = v;
	m_emitter->SetDynamicUpdateRange(m_particleDynamicData+o,m_particleDynamicData+o+1);
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
	m_fLastD(0),m_fLastI(0),
	m_dBegin(0), m_dEnd(0),
	m_iBegin(0), m_iEnd(0),
	m_shdrAttrEmitterTime(NULL)
{
	m_bufferID[0] = 0;
	m_iForcePasses = m_iForcePasses | (1<<15);
	m_iForcePasses = m_iForcePasses | (1<<7);
	Init();
}


ParticleEmitter::~ParticleEmitter()
{
	if (m_bufferID[0] > 0) { GLCALL( glDeleteBuffersARB(3, m_bufferID) ); }

	SAFE_DELETE_ARRAY(m_particles);
	SAFE_DELETE(GenerateParticlesClbk);
}


void ParticleEmitter::Init()
{
	MercuryNode::Init();
	SAFE_DELETE_ARRAY(m_particles);

	SetMaxParticleCount(m_maxParticles);
}

void ParticleEmitter::InitGeometry(float* g)
{
	uint8_t i = 0;

	//upper left
	g[i++] = -0.5; g[i++] = 0.5; g[i++] = 0;
	g[i++] = 1; g[i++] = 0; //U,V

	//lower left
	g[i++] = -0.5; g[i++] = -0.5; g[i++] = 0;
	g[i++] = 0; g[i++] = 0; //U,V

	//lower right
	g[i++] = 0.5; g[i++] = -0.5; g[i++] = 0;
	g[i++] = 0; g[i++] = 1; //U,V

	//upper right
	g[i++] = 0.5; g[i++] = 0.5; g[i++] = 0;
	g[i++] = 1; g[i++] = 1; //U,V
}

void ParticleEmitter::Update(float dTime)
{
	m_age += dTime;

	++m_fLastD;
	++m_fLastI;

	/* create particles until we meet the total number of
	   particles possible in terms of the age of the emitter */
	while (((m_age-(m_particlesEmitted*m_emitDelay)) > m_emitDelay) && (m_emitDelay>0))
	{
//		LOG.Write("Emit");
		++m_particlesEmitted;  //always increment even if the maximum number of particles exist
		ActivateParticle(m_age);
	}
	
	list< ParticleBase* >::iterator i = m_active.begin();
	while ( i!=m_active.end() )
	{
		ParticleBase *p = *i;
		p->Update(m_age);
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

void ParticleEmitter::ActivateParticle(float genTime)
{
	if (!m_inactive.empty())
	{
		ParticleBase* p = m_inactive.front();
		m_inactive.pop_front();
		
		p->Init(genTime);
		p->Activate();
		
		p->m_lifespan = m_particleMinLife;
		p->m_lifespan += (rand()%(int(m_particleMaxLife*1000) - int(m_particleMinLife*1000)))/1000.0f;
		p->m_rand1 = float(rand()%100000);
		p->m_rand2 = float(rand()%100000);

		p->WriteToVBO();

		//add to the active list
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
	m_maxParticles = count;

	m_vertexDynamicData.Allocate(m_maxParticles*4*4);
	m_geometryData.Allocate(m_maxParticles*4*5);
	m_indexData.Allocate(m_maxParticles*4);

	for (uint16_t i = 0; i < m_maxParticles*4*5; i+=(5*4))
		InitGeometry(m_geometryData.Buffer()+i);

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
		p->m_particleDynamicData = m_vertexDynamicData.Buffer()+(4*4*i);
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

void ParticleEmitter::UpdateVBO()
{
	if ( ( m_iBegin > 0 ) || ( m_dBegin > 0 ))
	{
//		LOG.Write( ssprintf("%d %d",m_fLastD,m_fLastI) );
//		LOG.Write( ssprintf("%p %p, %p %p,  %d %d\n", m_dBegin, m_dEnd, m_iBegin, m_iEnd, (unsigned long)m_dEnd-(unsigned long)m_dBegin, (unsigned long)m_iEnd-(unsigned long)m_iBegin) );
	}

	if ( m_dBegin > 0 )
	{
		unsigned long l = (unsigned long)m_dEnd-(unsigned long)m_dBegin;
		unsigned long s = (unsigned long)m_dBegin - (unsigned long)m_vertexDynamicData.Buffer();
		GLCALL( glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_vertexDynamicData.LengthInBytes(), NULL, GL_DYNAMIC_DRAW_ARB) );
		GLCALL( glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, s, l, m_dBegin));
		m_fLastD = 0;
	}
	if ( m_iBegin > 0 )
	{
		unsigned long l = (unsigned long)m_iEnd-(unsigned long)m_iBegin;
		unsigned long s = (unsigned long)m_iBegin - (unsigned long)m_indexData.Buffer();
		GLCALL( glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_vertexDynamicData.LengthInBytes(), NULL, GL_DYNAMIC_DRAW_ARB) );
		GLCALL( glBufferSubDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, s, l, m_iBegin));
		m_fLastI = 0;
	}

	m_dBegin = m_dEnd = m_iBegin = m_iEnd = 0;
}

void ParticleEmitter::Render(const MercuryMatrix& matrix)
{
	if (m_shdrAttrEmitterTime==NULL)
		m_shdrAttrEmitterTime = Shader::GetAttribute("EmitterTime");

	m_shdrAttrEmitterTime->type = ShaderAttribute::TYPE_FLOAT;
	m_shdrAttrEmitterTime->value.fFloat = m_age;

	GLCALL( glPushAttrib(GL_ENABLE_BIT|GL_CURRENT_BIT) );
	GLCALL( glDisable(GL_CULL_FACE) );
	
	if (m_bufferID[0]==0)
	{
		GLCALL( glGenBuffersARB(3, m_bufferID) );

		GLCALL( glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_bufferID[0]) ); //geometry VBO
		GLCALL( glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_geometryData.LengthInBytes(), m_geometryData.Buffer(), GL_STATIC_DRAW_ARB) );
		
		GLCALL( glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_bufferID[1]) ); //dynamic data (age and randoms)
		GLCALL( glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_vertexDynamicData.LengthInBytes(), m_vertexDynamicData.Buffer(), GL_DYNAMIC_DRAW_ARB) );
	
		GLCALL( glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_bufferID[2]) );
		GLCALL( glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_indexData.LengthInBytes(), m_indexData.Buffer(), GL_DYNAMIC_DRAW_ARB) );

		m_dBegin = m_dEnd = m_iBegin = m_iEnd = 0;
	}

	MercuryVBO::SetLastRendered(this);

	GLCALL( glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_bufferID[0]) ); //geometry VBO contains XYZUV
	Texture::ApplyActiveTextures(5*sizeof(float), 3*sizeof(float));
	GLCALL( glEnableClientState(GL_VERTEX_ARRAY) );
	GLCALL( glVertexPointer(3, GL_FLOAT, 5*sizeof(float), BUFFER_OFFSET( 0*sizeof(float) ) ) );

	GLCALL( glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_bufferID[1]) ); //dynamic data (age and randoms)
	GLCALL( glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_bufferID[2]) );

	UpdateVBO();

	GLCALL( glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT) );

	//do render stuff here

	GLCALL( glEnableClientState( GL_COLOR_ARRAY ) ); //used for attributes

	GLCALL( glColorPointer(4, GL_FLOAT, 4*sizeof(float), BUFFER_OFFSET( 0*sizeof(float) ) ) );

	GLCALL( glDrawElements(GL_QUADS,m_maxParticles*4,GL_UNSIGNED_SHORT,0) );

	GLCALL( glPopClientAttrib() );
	GLCALL( glPopAttrib() );

	m_particlesDrawn+=m_maxParticles;
	MercuryVBO::IncrementBatches();

	base::Render(matrix);
}

void ParticleEmitter::SetDynamicUpdateRange(void* begin, void* end)
{
	m_dBegin = m_dBegin==0?begin:m_dBegin;
	m_dEnd = m_dEnd==0?end:m_dEnd;

	m_dBegin = begin<m_dBegin?begin:m_dBegin;
	m_dEnd = end>m_dEnd?end:m_dEnd;
}

void ParticleEmitter::SetIndexUpdateRange(void* begin, void* end)
{
	m_iBegin = m_iBegin==0?begin:m_iBegin;
	m_iEnd = m_iEnd==0?end:m_iEnd;

	m_iBegin = begin<m_iBegin?begin:m_iBegin;
	m_iEnd = end>m_iEnd?end:m_iEnd;
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
