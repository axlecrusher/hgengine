#ifndef _CU2_H
#define _CU2_H

#include <TransformNode.h>
#include <MercuryValue.h>

///All the mouse actions Cu2 can transmit.
enum Cu2Action
{
	RESERVED,
	MOVE_IN,
	MOVE_OUT,
	PRESS_IN,
	RELEASE_IN,
	RELEASE_OUT,
	RELEASE_OUT_PROPOGATE,
};

///The base Cu2 UI Element.
class Cu2Element : public TransformNode
{
public:
	Cu2Element();

	///Load fromXML
	virtual void LoadFromXML(const XMLNode& node);

	///Save extra stuff to XML.
	virtual void SaveToXMLTag( MString & sXMLStream );

	///Handle a mouse action at a high level, typically this is overridden.
	///This will get called by Cu2Element::MouseMotion.  Generally, users do not call this.
	///For move events, iWhichButton is always 0.
	virtual void MouseAction( int x, int y, Cu2Action c, int iWhichButton );

	///Reset All attributes (will operate on all children in base class)
	virtual void ResetAttributes();

	///Push raw mouse event.  Generally, this calls MouseAction, and is only called internally.
	///You may override this if you want to take actions that require mouse motion.
	///Return value 2: Has hit a child.
	///Return value 1: Has hit self.
	///Return value 0: Has missed self.
	virtual int MouseMotion( int x, int y, unsigned char iCurrentButtonMask, unsigned char iLastButtonMask );

	///Called when a key is pressed - down the focus line.
	virtual void GetKeypress( int key, bool bDown, bool bRepeat );

	///Called whenever the element is resized.
	virtual void UpdateSize() { }

	//Below here - you run into functions that are seldom overloaded.

	///Handle updating of tab information
	virtual void AddChild(MercuryNode* n);

	///Handle updating of tab information
	virtual void RemoveChild(MercuryNode* n);

	///Also handle tab stopping correctly.
	virtual void SetHidden( bool bHide );

	///Does this node have focus?
	bool HasFocus();

	///Make this object get focus.
	void RaiseFocus();

	///Progress to the next tab.
	Cu2Element * NextTab();

	///Update this's tab
	void UpdateTab();

	///Enable/disable tab stopping on this node.
	void SetEnableTabStop( bool bStop );

	///Find out if tab stopping is allowed on this node.
	bool IsEnableTabStop( ) { return m_bCanTabStop; }

	///Propogate Release (You should not override this or modify it)
	void PropogateReleaseOut( int x, int y, int iWhichButton );

	///Set Position
	void SetXY( float fX, float fY ) { m_fX = fX; m_fY = fY; SetPosition( MercuryVertex( m_fX, m_fY, 0. ) ); }
	void SetX( float fX ) { m_fX = fX; SetPosition( MercuryVertex( m_fX, m_fY, 0 ) ); }
	void SetY( float fY ) { m_fY = fY; SetPosition( MercuryVertex( m_fX, m_fY, 0 ) ); }

	///Get X location
	float GetX() { return m_fX; }
	///Get Y location
	float GetY() { return m_fY; }

	///Set Width/Height
	void SetSize( float fW, float fH ) { m_fW = fW; m_fH = fH; UpdateSize(); }
	///Set Width
	void SetW( float fW ) { m_fW = fW; }
	///Set Height
	void SetH( float fH ) { m_fH = fH; }

	///Get Width
	float GetW() { return m_fW; }

	///Get Height
	float GetH() { return m_fH; }

protected:
	Cu2Element * m_pFocusNode;

	bool m_bCanTabStop;

	bool m_bWasMouseInThisFrame;

	float m_fX, m_fY, m_fW, m_fH;
	float m_fOrigX, m_fOrigY, m_fOrigW, m_fOrigH;
};

///Root Cu2 Window - this is usually full screen - when it is visible, mouse input grabbing is diabled, so you can use the cursor.
class Cu2Root : public Cu2Element
{
public:
	Cu2Root();
	virtual ~Cu2Root();

	///Overload hiddent to find out when the user 
	virtual void SetHidden( bool bHide );

	virtual void LoadFromXML( const XMLNode & node );

	///Return current Cu2Root.
	///Note: This will misbehave if you load more than one.  If no instance is loaded, it will return 0.
	static Cu2Root * GetCurrent() { return g_pCurrentInstance; }

	void HandleMouseInput(const MessageData& data);
	void HandleKeyboardInput(const MessageData& data);

	static MVRefBool CursorGrabbed;

	GENRTTI( Cu2Root );
private:
	static Cu2Root * g_pCurrentInstance;
	unsigned char m_iLastButtonMask;
};

class TextNode;

///Standard button
class Cu2Button : public Cu2Element
{
public:
	Cu2Button();

	virtual int MouseMotion( int x, int y, unsigned char iCurrentButtonMask, unsigned char iLastButtonMask );
	virtual void MouseAction( int x, int y, Cu2Action c, int iWhichButton );
	///This function gets called whenever the button is clicked, you should abstract from this.
	virtual void Click( int x, int y );

	virtual void LoadFromXML(const XMLNode& node);
	virtual void SaveToXMLTag( MString & sXMLStream );


	virtual void Render( const MercuryMatrix& m );

	void SetText( const MString & sText ) { m_sText = sText; Refresh(); }
	void SetAutoSize( bool bAutoSize ) { m_bAutoSize = bAutoSize; Refresh(); }
	void Refresh();

	MString & Payload() { return m_sValueToSend; }

	TextNode * GetTextNodeHandle() { return m_pText; }

	GENRTTI( Cu2Button );
private:
	MString m_sAssociatedValue;
	MString m_sAssociatedValueSet;
	MString m_sAssociatedValueX;
	MString m_sAssociatedValueY;

	MString m_sMessageToSend;
	MString m_sValueToSend;
	MString m_sText;

	MString m_sxRangeMin;
	MString m_sxRangeMax;
	MString m_syRangeMin;
	MString m_syRangeMax;

	bool m_bAutoSize;
	bool m_bDown;
	TextNode * m_pText;
};

///Standard label (doesn't do anything but display something)
class Cu2Label : public Cu2Element
{
public:
	Cu2Label();
	~Cu2Label();

	virtual void LoadFromXML(const XMLNode& node);
	virtual void SaveToXMLTag( MString & sXMLStream );

	virtual void Render( const MercuryMatrix& m );

	void SetText( const MString & sText ) { m_sText = sText; Refresh(); }
	void SetAutoSize( bool bAutoSize ) { m_bAutoSize = bAutoSize; Refresh(); }
	void Refresh();

	MString & Payload() { return m_sValueToSend; }
	TextNode * GetTextNodeHandle() { return m_pText; }

	void ChangeValue( MValue * v );

	GENRTTI( Cu2Button );
private:
	MString m_sAssociatedValue;
	MString m_sMessageToSend;
	MString m_sValueToSend;
	MString m_sText;
	bool m_bAutoSize;
	bool m_bDown;
	TextNode * m_pText;
};

///Dialog box (for putting other things into and being able to drag around)
class Cu2Dialog : public Cu2Element
{
public:
	Cu2Dialog();
	GENRTTI( Cu2Dialog );

	TextNode * GetTextNodeHandle() { return m_pTitle; }

	virtual void LoadFromXML(const XMLNode& node);
	virtual void SaveToXMLTag( MString & sXMLStream );
	virtual void Render( const MercuryMatrix& m );
	virtual int MouseMotion( int x, int y, unsigned char iCurrentButtonMask, unsigned char iLastButtonMask );
	virtual void MouseAction( int x, int y, Cu2Action c, int iWhichButton );
	virtual void UpdateSize();

	void SetText( const MString & sText );

private:
	bool m_bDragging;
	int m_iClickX, m_iClickY;
	TextNode * m_pTitle;
	MString m_sTitle;
};


#endif


/****************************************************************************
 *   Copyright (C) 2009 by Charles Lohr                                     *
 *                         Joshua Allen                                     *
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

