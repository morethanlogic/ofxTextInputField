//
//  textInput.h
//  textInput
//
//  Created by Elliot Woods on 09/12/2011.
//  Copyright 2011 Kimchi and Chips.
//
//  modified by James George 12/2/2011
//  modified by Momo the Monster 7/10/2012
//  swappable fonts added by James George 9/11/2012
//
//	MIT license
//	http://www.opensource.org/licenses/mit-license.php
//

#pragma once

#include "ofMain.h"

//For lack of a type abstraction, this lets you #define a font renderer so
//(like ofxFTGL or ofxFont)
//to use ofxFTGL use somethinglike this:
//#define OFX_TEXTFIELD_FONT_RENDERER ofxFTGLFont
//#define OFX_TEXTFIELD_FONT_RENDERER "ofxFTGLFont.h"

#ifndef OFX_TEXTFIELD_FONT_RENDERER
#define OFX_TEXTFIELD_FONT_RENDERER ofTrueTypeFont
#endif 

#ifdef OFX_TEXTFIELD_FONT_INCLUDE
#include OFX_TEXTFIELD_FONT_INCLUDE
#endif

#ifdef OF_VERSION_MINOR
#if OF_VERSION_MINOR>=8 || OF_VERSION_MAJOR>0
#define USE_GLFW_CLIPBOARD
#endif
#endif


// TODO: wrapping
#include "ofxTextInputFieldFontRenderer.h"

class ofxTextInputField {
  public:
    enum TextAlignment
    {
        TextAlignmentLeft,
        TextAlignmentCenter
    };
    
	ofxTextInputField();
	virtual ~ofxTextInputField();
	//swap in a font!
	void setFont(OFX_TEXTFIELD_FONT_RENDERER& font);
    
    void setup();
    
    void setTextAlignment(TextAlignment alignment);
    void setHorizontalPadding(int val);
    void setVerticalPadding(int val);
    void setCapsVerticalOffset(int val);
	
	void enable();
	void disable();
    bool getIsEnabled();
	
	bool getIsEditing();
	void beginEditing();
	void endEditing();
	
    //can be set manually or otherwise is controlled by enable/disable
    bool drawCursor;
    bool drawBounds;

    ofRectangle bounds;
	
    void draw();
	void clear();
	
	string text;
	string placeholderText;
	
	ofColor textColor;
	ofColor placeholderColor;
	ofColor selectionColor;
	ofColor boundsColor;
	
	int cursorPosition;
	int selectionBegin;
	int selectionEnd;
	bool selecting;
	
	ofEvent<void> editingBegan;
	ofEvent<void> editingEnded;
	ofEvent<string> textChanged;

	void keyPressed(ofKeyEventArgs &a);
    void keyReleased(ofKeyEventArgs &a);
	
	bool autoClear;
	bool autoTab;
	bool autoEdit;
	
	bool multiline;
    
#ifdef USE_GLFW_CLIPBOARD
    void setClipboard(string clippy);
    string getClipboard();
#endif
	
  protected:
    TextAlignment textAlignment;
    
	float lastTimeCursorMoved;
	int verticalPadding;
    int capsVerticalOffset;
	ofxTextInput::FontRenderer* fontRef;
	
    bool 	isEnabled;
	bool	isEditing;
	bool	mouseDownInRect;
	void    mousePressed(ofMouseEventArgs& args);
    void    mouseDragged(ofMouseEventArgs& args);
	void    mouseReleased(ofMouseEventArgs& args);
	
	
	//int getLineForPosition(int pos);

	//void setCursorPositionFromXY();
	//void setCursorFromMouse(int x, int y);
	//void setCursorXYFromPosition();
	void getCursorCoords(int pos, int &cursorX, int &cursorY);
	int getCursorPositionFromMouse(int x, int y);
    
    bool isShifted, isCommand;
    map<int, char> shiftMap;
};
