#include "Cu2.h"
#include <MercuryInput.h>
#include <MercuryMessageManager.h>
#include <MercuryWindow.h>
#include <MercuryLog.h>
#include <TextNode.h>

#include <GLHeaders.h>

///////////////////////////////////////COPPER 2 ELEMENT///////////////////////////////////////

Cu2Element::Cu2Element() : TransformNode(),
	m_pFocusNode(0), m_bCanTabStop( false ), m_iButtonMask(0), m_bWasMouseInThisFrame(0), 
	m_fX(0), m_fY(0), m_fW(100), m_fH(100),
	m_fOrigX(0), m_fOrigY(0), m_fOrigW(100), m_fOrigH(100)
{
}

void Cu2Element::LoadFromXML(const XMLNode& node)
{
	LOAD_FROM_XML( "x", m_fOrigX, StrToFloat );
	LOAD_FROM_XML( "y", m_fOrigY, StrToFloat );
	LOAD_FROM_XML( "width", m_fOrigW, StrToFloat );
	LOAD_FROM_XML( "height", m_fOrigH, StrToFloat );

	TransformNode::LoadFromXML( node );

	SetXY( m_fOrigX, m_fOrigY );
	SetSize( m_fOrigW, m_fOrigH );
}

void Cu2Element::SaveToXMLTag( MString & sXMLStream )
{
	if( GetX() != 0 ) sXMLStream += ssprintf( "x=\"%f\" ", GetX() );
	if( GetY() != 0 ) sXMLStream += ssprintf( "y=\"%f\" ", GetY() );
	if( GetW() != 0 ) sXMLStream += ssprintf( "width=\"%f\" ", GetW() );
	if( GetH() != 0 ) sXMLStream += ssprintf( "height=\"%f\" ", GetH() );

	TransformNode::SaveToXMLTag( sXMLStream );
}

void Cu2Element::ResetAttributes()
{
	SetXY( m_fOrigX, m_fOrigY );
	SetSize( m_fOrigW, m_fOrigH );
}

bool Cu2Element::MouseMotion( int x, int y, unsigned char iCurrentButtonMask )
{
	if(  IsHidden() )
		return false;

	bool bIsInside = ( x >= 0 && x < m_fW && y >= 0 && y < m_fH );

	if( m_bWasMouseInThisFrame && !bIsInside )
		MouseAction( x, y, MOVE_OUT, 0 );
	else if( !m_bWasMouseInThisFrame && bIsInside )
		MouseAction( x, y, MOVE_IN, 0 );

	m_bWasMouseInThisFrame = bIsInside;

	for( unsigned button = 0; button < 8; button++ )
	{
		unsigned char Mask = 1<<button;
		bool bWasDown = m_iButtonMask & Mask;
		bool bIsDown = iCurrentButtonMask & Mask;
		if( bWasDown && !bIsDown )
		{
			//XXX: When we release outside - we want to propogate that information, and that can be tricky..
			//So, instead, we choose to propogate that elsewhere...
			m_iButtonMask &= ~Mask;
			if( bIsInside )
				MouseAction( x, y, RELEASE_IN, button );
			else
				PropogateReleaseOut( x, y, button );
		}
		else if( !bWasDown && bIsDown && bIsInside )
		{
			m_iButtonMask |= Mask;
			MouseAction( x, y, PRESS_IN, button );
		}
	}

	if( bIsInside )
		for( MercuryNode* send = FirstChild(); send; send = NextChild( send ) )
		{
			Cu2Element * sa = dynamic_cast<Cu2Element*>(send);
			if( sa )
			{
				sa->MouseMotion( x - int(sa->m_fX), y - int(sa->m_fY), iCurrentButtonMask );
			}
		}

	return bIsInside;
}

void Cu2Element::PropogateReleaseOut( int x, int y, int iWhichButton )
{
	MouseAction( x, y, RELEASE_OUT, iWhichButton );

	for( MercuryNode* send = FirstChild(); send; send = NextChild( send ) )
	{
		Cu2Element * sa = dynamic_cast<Cu2Element*>(send);
		if( sa )
			sa->PropogateReleaseOut( x - int(sa->m_fX), y - int(sa->m_fY), iWhichButton );
	}
}

void Cu2Element::MouseAction( int x, int y, Cu2Action c, int iWhichButton )
{
	//nothing
}

void Cu2Element::AddChild(MercuryNode* n)
{
	if( !m_pFocusNode )
	{
		Cu2Element * sa = dynamic_cast< Cu2Element * >(n);
		if( sa && sa->IsEnableTabStop() && !sa->IsHidden() )
			m_pFocusNode = sa;
	}
	MercuryNode::AddChild( n );
}

void Cu2Element::RemoveChild(MercuryNode* n)
{
	if( m_pFocusNode == n )
	{
		m_pFocusNode->SetEnableTabStop( false );
		UpdateTab();
	}

	MercuryNode::RemoveChild( n );
}

Cu2Element * Cu2Element::NextTab()
{
	if( m_pFocusNode )
	{
		for( MercuryNode* send = NextChild( m_pFocusNode ); send; send = NextChild( send ) )
		{
			Cu2Element * sa = dynamic_cast<Cu2Element*>(send);
			if( sa && sa->IsEnableTabStop() && !sa->IsHidden() )
				return sa;
		}
	}

	for( MercuryNode* send = FirstChild(); send; send = NextChild( send ) )
	{
		Cu2Element * sa = dynamic_cast<Cu2Element*>(send);
		if( sa && sa->IsEnableTabStop() && !sa->IsHidden() )
			return sa;
	}
	return 0;
}

void Cu2Element::SetHidden( bool bHide )
{
	MercuryNode::SetHidden( bHide );
	Cu2Element * parent = dynamic_cast<Cu2Element *>(Parent());
	if( parent )
		parent->UpdateTab();
}

void Cu2Element::SetEnableTabStop( bool bStop )
{
	m_bCanTabStop = bStop;
	Cu2Element * parent = dynamic_cast<Cu2Element *>(Parent());
	if( parent )
		parent->UpdateTab();
}

void Cu2Element::UpdateTab()
{
	for( MercuryNode* send = m_pFocusNode; send; send = NextChild( send ) )
	{
		Cu2Element * sa = dynamic_cast<Cu2Element*>(send);
		if( sa && sa->IsEnableTabStop() && !sa->IsHidden() )
		{
			m_pFocusNode = sa;
			return;
		}
	}
	for( MercuryNode* send = FirstChild(); send; send = NextChild( send ) )
	{
		Cu2Element * sa = dynamic_cast<Cu2Element*>(send);
		if( sa && sa->IsEnableTabStop() && !sa->IsHidden() )
		{
			m_pFocusNode = sa;
			return;
		}
	}
	m_pFocusNode = 0;
}

void Cu2Element::GetKeypress( int key, bool bDown, bool bRepeat )
{
	if( m_pFocusNode )
		m_pFocusNode->GetKeypress( key, bDown, bRepeat );
}

REGISTER_NODE_TYPE(Cu2Element);




///////////////////////////////////////COPPER 2 ROOT///////////////////////////////////////
Cu2Root::Cu2Root()
{
	g_pCurrentInstance = this;
	REGISTER_MESSAGE_WITH_DELEGATE( INPUTEVENT_MOUSE, &Cu2Root::HandleMouseInput );
	REGISTER_MESSAGE_WITH_DELEGATE( INPUTEVENT_KEYBOARD, &Cu2Root::HandleKeyboardInput );
}

Cu2Root::~Cu2Root()
{
	g_pCurrentInstance = 0;
}

void Cu2Root::SetHidden( bool bHide )
{
	MercuryWindow::GetCurrentWindow()->SetGrabbedMouseMode( bHide );

	MercuryNode::SetHidden( bHide );
}

void Cu2Root::LoadFromXML(const XMLNode& node)
{
	Cu2Element::LoadFromXML( node );
	MercuryWindow::GetCurrentWindow()->SetGrabbedMouseMode( IsHidden() );
}


void Cu2Root::HandleMouseInput(const MessageData& data)
{
	const MouseInput& m( dynamic_cast<const MouseInput&>( data ) );

	//Don't pass on things if we're hidden.
	if( IsHidden() )
	{
		//Don't do anything	
	}
	else
	{
		MouseMotion( m.dx, MercuryWindow::GetCurrentWindow()->Height()-m.dy, m.buttons.data );
	}
}

void Cu2Root::HandleKeyboardInput(const MessageData& data)
{
	const KeyboardInput& m( dynamic_cast<const KeyboardInput&>( data ) );

	if( m.key == 27 && m.isDown && !m.isRepeat )
	{
		SetHidden( !IsHidden() );
	}

	//Don't pass on things if we're hidden.
	if( IsHidden() ) return;

	GetKeypress( m.key, m.isDown, m.isRepeat );
}

Cu2Root * Cu2Root::g_pCurrentInstance;

REGISTER_NODE_TYPE(Cu2Root);

///////////////////////////////////////COPPER 2 BUTTON///////////////////////////////////////

Cu2Button::Cu2Button() : Cu2Element()
{
	m_pText = (TextNode*)NODEFACTORY.Generate( "TextNode" );
	AddChild( m_pText );
	m_bAutoSize = true;
	m_bDown = false;
}	

void Cu2Button::LoadFromXML(const XMLNode& node)
{
	LOAD_FROM_XML( "clickMessage", m_sMessageToSend, );
	LOAD_FROM_XML( "text", m_sText,  );
	LOAD_FROM_XML( "autoSize",  m_bAutoSize, StrToBool );

	if( m_pText )
	{
		m_pText->LoadFromXML( node );
		m_pText->SetShiftAbsolute( true );
		m_pText->SetShiftX( 5 );
		m_pText->SetShiftY( 5 );
	}

	Cu2Element::LoadFromXML( node );

	Refresh();
}

void Cu2Button::SaveToXMLTag( MString & sXMLStream )
{
	if( m_sMessageToSend.length() ) sXMLStream += ssprintf( "clickMessage=\"%s\" ", m_sMessageToSend.c_str() );
	if( m_bAutoSize ) sXMLStream += ssprintf( "autoSize=\"%d\" ", m_bAutoSize );

	if( !m_pText )
		m_pText->SaveToXMLTag( sXMLStream );

	Cu2Element::SaveToXMLTag( sXMLStream );
}

void Cu2Button::MouseAction( int x, int y, Cu2Action c, int iWhichButton )
{
	if( c == PRESS_IN )
		m_bDown = true;
	if( c == RELEASE_IN )
	{
		if( m_bDown )
			Click();
		m_bDown = false;
	}

	if( c == RELEASE_OUT )
	{
		m_bDown = false;
	}
}

void Cu2Button::Refresh()
{
	if( !m_pText )
	{
		LOG.Write( "Warning: Cu2Button \"" + GetName() + "\" does not have valid Text box associated." );
		return;
	}

	m_pText->SetText( m_sText );
	m_pText->RenderText();

	if( m_bAutoSize )
	{
		SetSize( m_pText->GetRMaxX() + 8, m_pText->GetRMaxY() + 8 );
		printf( "Size: %f %f\n", m_pText->GetRMaxX() + 8, m_pText->GetRMaxY() + 8 );
	}
}

void Cu2Button::Click()
{
	if( m_sMessageToSend.length() )
		MESSAGEMAN.BroadcastMessage( m_sMessageToSend, new PointerDataMessage( this ) );
}

void Cu2Button::Render( const MercuryMatrix& m )
{
	glDisable( GL_TEXTURE_2D );
	if( m_bDown )
		glColor3f( 0.3, 0.3, 0.3 );
	else
		glColor3f( 0.5, 0.5, 0.5 );

	glBegin( GL_QUADS );
	glVertex2f( 1., 1. );
	glVertex2f( GetW()-1, 1 );
	glVertex2f( GetW()-1, GetH()-1);
	glVertex2f( 1., GetH()-1 );
	glEnd();

	glLineWidth( 2 );
	glBegin( GL_LINES );
	if( m_bDown )
		glColor3f( 0.1, 0.1, 0.1 );
	else
		glColor3f( 0.7, 0.7, 0.7 );
	glVertex2f( 1, 1 );
	glVertex2f( 1, GetH()-1 );
	glVertex2f( 1, GetH()-1 );
	glVertex2f( GetW()-2, GetH()-1 );
	if( !m_bDown )
		glColor3f( 0.1, 0.1, 0.1 );
	else
		glColor3f( 0.7, 0.7, 0.7 );
	glVertex2f( GetW()-1, GetH()-2 );
	glVertex2f( GetW()-1, 1 );
	glVertex2f( GetW()-1, 1 );
	glVertex2f( 1, 1 );
	glEnd();
	glEnable( GL_TEXTURE_2D );

	glColor3f( 1., 1., 1. );

	TransformNode::Render( m );
}

REGISTER_NODE_TYPE(Cu2Button);


/****************************************************************************
 *   Copyright (C) 2008-2009 by Joshua Allen                                *
 *                              Charles Lohr                                *
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

