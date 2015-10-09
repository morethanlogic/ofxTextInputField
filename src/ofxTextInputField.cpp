//
//  textInput.cpp
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

#include "ofxTextInputField.h"
#include <regex>


ofxTextInputField::ofxTextInputField() {
    
    shiftMap[44] = '<';
    shiftMap[45] = '_';
    shiftMap[46] = '>';
    shiftMap[48] = ')';
    shiftMap[49] = '!';
    shiftMap[50] = '@';
    shiftMap[51] = '#';
    shiftMap[52] = '$';
    shiftMap[53] = '%';
    shiftMap[54] = '^';
    shiftMap[55] = '&';
    shiftMap[56] = '*';
    shiftMap[57] = '(';
    shiftMap[61] = '+';
    shiftMap[63] = '/';
    shiftMap[91] = '{';
    shiftMap[92] = '|';
    shiftMap[93] = '}';
    shiftMap[96] = '~';
    
	placeholderText = "";
	text = "";

	placeholderColor.set(ofColor::gray);
	textColor.set(ofColor::white);
	selectionColor.setHex(0x6988db);
	boundsColor.set(ofColor::white);

	multiline = false;
	autoEdit = true;
	autoTab = true;
	cursorPosition = 0;
	selectionBegin = 0;
	selectionEnd = 0;
	selecting = false;

	isCommand = false;
	isShifted = false;
	
	fontRef = NULL;
    isEnabled = false;
	isEditing = false;
    bounds = ofRectangle(0,0,100,22);
	
    drawCursor = false;
	drawBounds = true;
	autoClear = false;
	mouseDownInRect = false;

	fontRef = new ofxTextInput::BitmapFontRenderer();
    //isSetup = false;
	
    textAlignment = TextAlignmentLeft;
	verticalPadding = 12;
    capsVerticalOffset = 0;
	horizontalPadding = 10;
	lastTimeCursorMoved = ofGetElapsedTimef();
    
    bIsPhoneNumber = false;
}

ofxTextInputField::~ofxTextInputField(){
	if(isEnabled){
        disable();
    }

}

void ofxTextInputField::setup(){
	enable();
}


void ofxTextInputField::enable(){
	if(!isEnabled){
//		ofAddListener(ofEvents().mousePressed, this, &ofxTextInputField::mousePressed);
//		ofAddListener(ofEvents().mouseDragged, this, &ofxTextInputField::mouseDragged);
//		ofAddListener(ofEvents().mouseReleased, this, &ofxTextInputField::mouseReleased);
		isEnabled = true;
	}
}

void ofxTextInputField::disable(){
	if(isEditing){
		endEditing();
	}
	if(isEnabled){
//        ofRemoveListener(ofEvents().mousePressed, this, &ofxTextInputField::mousePressed);
//		ofRemoveListener(ofEvents().mouseDragged, this, &ofxTextInputField::mouseDragged);
//		ofRemoveListener(ofEvents().mouseReleased, this, &ofxTextInputField::mouseReleased);
		isEnabled = false;
    }
	
}
void ofxTextInputField::beginEditing() {
    if(!isEditing){
        ofAddListener(ofEvents().keyPressed, this, &ofxTextInputField::keyPressed);
        ofAddListener(ofEvents().keyReleased, this, &ofxTextInputField::keyReleased);
		ofNotifyEvent(editingBegan, this);
        isEditing = true;
        drawCursor = true;
		if(autoClear){
			clear();
		}
		else{


		}
    }
}

void ofxTextInputField::endEditing() {
    if(isEditing){
        ofRemoveListener(ofEvents().keyPressed, this, &ofxTextInputField::keyPressed);
        ofRemoveListener(ofEvents().keyReleased, this, &ofxTextInputField::keyReleased);
		ofNotifyEvent(editingEnded, this);
        isEditing = false;
        drawCursor = false;
    }
}

void ofxTextInputField::setFont(OFX_TEXTFIELD_FONT_RENDERER& font){
	if(fontRef->isBitmapFont()) {
		delete fontRef;
	}
	fontRef = new ofxTextInput::TypedFontRenderer(&font);
}

void ofxTextInputField::setTextAlignment(TextAlignment alignment){
    textAlignment = alignment;
}

void ofxTextInputField::setHorizontalPadding(int val){
    horizontalPadding = val;
}

void ofxTextInputField::setVerticalPadding(int val){
    verticalPadding = val;
}

void ofxTextInputField::setCapsVerticalOffset(int val){
    capsVerticalOffset = val;
}

bool ofxTextInputField::getIsEditing(){
    return isEditing;
}

bool ofxTextInputField::getIsEnabled(){
	return isEnabled;
}

void ofxTextInputField::draw(bool bUseBoundsCoord) {
    
	if (drawBounds) {
		ofPushStyle();
		ofNoFill();
		ofSetColor(boundsColor);
		ofRect(bounds);
		ofPopStyle();
	}

	ofPushMatrix();
    
    if (bUseBoundsCoord) {
        ofTranslate(bounds.x, bounds.y);
    }

	if(selecting) {
		ofPushStyle();
		// argh, splitting all the time.
		vector<string> lines = ofSplitString(text, "\n");
		int beginCursorX, beginCursorY;
		int endCursorX, endCursorY;
		getCursorCoords(selectionBegin, beginCursorX, beginCursorY);
		getCursorCoords(selectionEnd, endCursorX, endCursorY);

		float startX = fontRef->stringWidth(lines[beginCursorY].substr(0,beginCursorX));
		float endX = fontRef->stringWidth(lines[endCursorY].substr(0, endCursorX));

		ofSetColor(selectionColor);
		ofFill();
		
		if(beginCursorY==endCursorY) {
			// single line selection
			ofRect(horizontalPadding + startX, verticalPadding + fontRef->getLineHeight()*beginCursorY,
				   endX - startX, fontRef->getLineHeight());
		} else {
			
			// multiline selection.
			// do first line to the end
			ofRect(horizontalPadding + startX, verticalPadding + fontRef->getLineHeight()*beginCursorY,
				   fontRef->stringWidth(lines[beginCursorY]) - startX,
				   fontRef->getLineHeight()
			);
			
			// loop through entirely selected lines
			for(int i = beginCursorY + 1; i < endCursorY; i++) {
				ofRect(horizontalPadding, verticalPadding + fontRef->getLineHeight()*i,
					   fontRef->stringWidth(lines[i]),
					   fontRef->getLineHeight()
				);
			}
			// do last line up to endX
			ofRect(horizontalPadding, verticalPadding + fontRef->getLineHeight()*endCursorY,
					endX, fontRef->getLineHeight()
			);
		}
		ofPopStyle();
		
	} 
	//draw cursor line
    else if(drawCursor) {
		ofPushStyle();
		// cursor should only blink when its been idle, and animation
		// should be a clipped sine wave
        float timeFrac = 0.5 * ofClamp(cos(6.0f * (ofGetElapsedTimef()-lastTimeCursorMoved))*4, -1, 1) + 0.5;
        
        ofSetColor(textColor.r * timeFrac, textColor.g * timeFrac, textColor.b * timeFrac, textColor.a);
		
		
		// argh, splitting all the time.
		vector<string> lines = ofSplitString(text, "\n");
		
		
		// calculate this every loop.
		int cursorX, cursorY;
        getCursorCoords(cursorPosition, cursorX, cursorY);
        
        
	//	printf("Pos: %d    X: %d   Y: %d\n", cursorPosition, cursorX, cursorY);
        
        float textWidth = 0;
        if (text.length() > 0) {
            textWidth = fontRef->stringWidth(text);
        }
		int cursorPos = horizontalPadding + fontRef->stringWidth(lines[cursorY].substr(0,cursorX));
        
        if (cursorPos < 0) {
            cursorPos = 0;
        }
        
        if (textAlignment == TextAlignmentCenter) {
            cursorPos += bounds.width/2 - textWidth * 0.5f;
        }
        
		int cursorTop = verticalPadding + fontRef->getLineHeight()*cursorY;
        int cursorBottom = bounds.height - verticalPadding;
		
		ofSetLineWidth(1.0f);
		//TODO: multiline with fontRef
        ofLine(cursorPos, cursorTop,
			   cursorPos, cursorBottom);
        ofPopStyle();
    }
	else if (placeholderText != "" && text == "") {
		ofSetColor(placeholderColor);
        
        if (textAlignment == TextAlignmentLeft) {
            fontRef->drawString(placeholderText, horizontalPadding, fontRef->getLineHeight()+verticalPadding);
        }
        else if (textAlignment == TextAlignmentCenter) {
            float placeholderTextWidth = fontRef->stringWidth(placeholderText);
            fontRef->drawString(placeholderText, bounds.width/2 - placeholderTextWidth * 0.5f , fontRef->getLineHeight()+verticalPadding);
        }
	}
	
	ofSetColor(textColor);
    
    if (textAlignment == TextAlignmentLeft) {
        fontRef->drawString(text, horizontalPadding, fontRef->getLineHeight()+verticalPadding + capsVerticalOffset);
    }
    else if (textAlignment == TextAlignmentCenter) {
        float textWidth = fontRef->stringWidth(text);
        fontRef->drawString(text, bounds.width/2 - textWidth * 0.5f , fontRef->getLineHeight()+verticalPadding);
    }
    
	ofPopMatrix();
}

void ofxTextInputField::getCursorCoords(int pos, int &cursorX, int &cursorY) {
	vector<string> lines = ofSplitString(text, "\n");
	
	
	int c = 0;
	
	
	for(int i = 0; i < lines.size(); i++) {
		if(pos<=c+lines[i].size()) {
			cursorY = i;
			cursorX = pos - c;
			return;
		}
		c += lines[i].size() + 1;
	}

}

/*
void ofxTextInputField::setCursorPositionFromXY() {
	cursorPosition = cursorx;
	vector<string> parts = ofSplitString(text, "\n");
	for(int i = 0 ; i < cursory; i++) {
		cursorPosition += parts[i].size() + i; // for carriage returns
	}
}

*/
int ofxTextInputField::getCursorPositionFromMouse(int x, int y) {
	int cursorX = 0;
	int cursorY = 0;
	float pos = y - bounds.y - verticalPadding;
	pos /= fontRef->getLineHeight();
	int line = pos;
	cursorY = line;
	
	vector<string> lines = ofSplitString(text, "\n");
	if(cursorY>=lines.size()-1) cursorY = lines.size()-1;
	if(lines.size()>0) {
		cursorX = fontRef->getPosition(lines[cursorY], x - horizontalPadding - bounds.x);
	}
	int c = 0;
	for(int i = 0; i < cursorY; i++) {
		c += lines[i].size() + 1;
	}
	c += cursorX;
	return c;
}


void ofxTextInputField::mousePressed(ofMouseEventArgs& args){
	mouseDownInRect = bounds.inside(args.x, args.y);
	if(mouseDownInRect) {
		cursorPosition = getCursorPositionFromMouse(args.x, args.y);
		lastTimeCursorMoved = ofGetElapsedTimef();
		selecting = false;
	}
}


void ofxTextInputField::mouseDragged(ofMouseEventArgs& args) {
	if(bounds.inside(args.x, args.y)) {
		int pos = getCursorPositionFromMouse(args.x, args.y);
		if(pos!=cursorPosition) {
			selecting = true;
			selectionBegin = MIN(pos, cursorPosition);
			selectionEnd = MAX(pos, cursorPosition);
			
		} else {
			selecting = false;
		}
	}
}

void ofxTextInputField::mouseReleased(ofMouseEventArgs& args){
	if (!autoEdit) return;

    if(bounds.inside(args.x, args.y)) {
        if(!isEditing && mouseDownInRect){
	        beginEditing();
        }
    }
    else if(isEditing){
		endEditing();
	}
}

#ifdef USE_GLFW_CLIPBOARD

#if (_MSC_VER)
#include <GLFW/glfw3.h>
#else
#include "GLFW/glfw3.h"
#endif


void ofxTextInputField::setClipboard(string clippy)
{
#if defined(TARGET_OSX)
	glfwSetClipboardString( (GLFWwindow*) ofGetWindowPtr()->getCocoaWindow(), clippy.c_str());
#elif defined(TARGET_WIN32)
	glfwSetClipboardString( (GLFWwindow*) ofGetWindowPtr()->getWin32Window(), clippy.c_str());
#endif
}

string ofxTextInputField::getClipboard()
{
#if defined(TARGET_OSX)
	const char *clip = glfwGetClipboardString((GLFWwindow*) ofGetWindowPtr()->getCocoaWindow());
#elif defined(TARGET_WIN32)
	const char *clip = glfwGetClipboardString((GLFWwindow*) ofGetWindowPtr()->getWin32Window());
#endif
	if(clip!=NULL) {
		return string(clip);
	} else {
		return "";
	}

}

#endif

void ofxTextInputField::keyPressed(ofKeyEventArgs& args) {
    
	//ew: add charachter (non unicode sorry!)
	//jg: made a step closer to this with swappable renderers and ofxFTGL -- but need unicode text input...
	lastTimeCursorMoved = ofGetElapsedTimef();
	int key = args.key;
		
    if(key == OF_KEY_SHIFT || key == OF_KEY_LEFT_SHIFT || key == OF_KEY_RIGHT_SHIFT) {
        isShifted = true;
        return;
    }

#if defined(TARGET_OSX)
	if(key == OF_KEY_COMMAND) {  // EZ: or 4532, test this on Mac
#elif defined(TARGET_WIN32)
	if(key == OF_KEY_CONTROL) {
#endif
		isCommand = true;
    }
	
#ifdef USE_GLFW_CLIPBOARD
    if(key == 'c' && isCommand ) {
        setClipboard(text.substr(selectionBegin, selectionEnd - selectionBegin));
        return;
    }
	
    if(key == 'v' && isCommand ) {
        text.insert(cursorPosition, getClipboard());
        return;
    }
#endif
    
	if ((key >=32 && key <=126) || key=='\t' || key==OF_KEY_RETURN) {
		if(selecting) {
			text.erase(text.begin() + selectionBegin,
					   text.begin() + selectionEnd
					   );
			cursorPosition = selectionBegin;
			selecting = false;
		}
	}
			
			
	if (key == OF_KEY_RETURN) {
		if(!multiline) {
			endEditing();
			return;
		}
		text.insert(text.begin()+cursorPosition, '\n');
		cursorPosition++;
		

		if(autoTab) {
			// how much whitespace is there on the previous line?
			int xx, yy;
			getCursorCoords(cursorPosition, xx, yy);
			vector<string> lines = ofSplitString(text, "\n");
			if(yy>0) {
				
				// collect all the whitespace on the previous line.
				string previousWhitespace = "";
				string previousLine = lines[yy-1];
				int pos = 0;
				for(int i = 0; i < previousLine.size(); i++) {
					if(previousLine[i]==' ' || previousLine[i]=='\t') {
						previousWhitespace += previousLine[i];
					} else {
						break;
					}
				}
				// if we have a curly brace as the last char on the previous line
				// increase the indentation
				if(previousLine[previousLine.size()-1]=='{') {
					if(previousWhitespace=="") {
						previousWhitespace = "\t";
					} else {
						previousWhitespace += previousWhitespace[previousWhitespace.size()-1];
					}
				}
				text = text.insert(cursorPosition, previousWhitespace);
				cursorPosition += previousWhitespace.size();
			}
		}

		ofNotifyEvent(textChanged, text, this);
        return;
	}
	
	
	if (key==OF_KEY_BACKSPACE) {
		if(selecting) {
			text.erase(text.begin() + selectionBegin,
					   text.begin() + selectionEnd
			);
			cursorPosition = selectionBegin;
			selecting = false;
		} else {
			if (cursorPosition>0) {
				text.erase(text.begin()+cursorPosition-1);
				--cursorPosition;
			}
		}
        
        if (bIsPhoneNumber) {
            if (text.size() == 13) {
                string cleanedPhoneStr = text;
                
                regex regExDigit("[^0-9]");
                cleanedPhoneStr = regex_replace(cleanedPhoneStr, regExDigit, "");
                
                regex regExLeadindSpaces("^\\s+");
                cleanedPhoneStr = regex_replace(cleanedPhoneStr, regExLeadindSpaces, "");
                
                regex regExTrailingSpaces("\\s+$");
                cleanedPhoneStr = regex_replace(cleanedPhoneStr, regExTrailingSpaces, "");
                

                cleanedPhoneStr.insert(3, "-");
                cleanedPhoneStr.insert(7, "-");
                text = cleanedPhoneStr;
                --cursorPosition;
            }
        }

		ofNotifyEvent(textChanged, text, this);
		return;
	}
	
	if (key==OF_KEY_DEL) {
		if(selecting) {
			text.erase(text.begin() + selectionBegin,
					   text.begin() + selectionEnd
					   );
			cursorPosition = selectionBegin;
			selecting = false;
		} else {
			if (text.size() > cursorPosition) {
				text.erase(text.begin()+cursorPosition);
			}
		}
        
        if (bIsPhoneNumber) {
            if (text.size() == 13) {
                string cleanedPhoneStr = text;
                
                regex regExDigit("[^0-9]");
                cleanedPhoneStr = regex_replace(cleanedPhoneStr, regExDigit, "");
                
                regex regExLeadindSpaces("^\\s+");
                cleanedPhoneStr = regex_replace(cleanedPhoneStr, regExLeadindSpaces, "");
                
                regex regExTrailingSpaces("\\s+$");
                cleanedPhoneStr = regex_replace(cleanedPhoneStr, regExTrailingSpaces, "");
                
                
                cleanedPhoneStr.insert(3, "-");
                cleanedPhoneStr.insert(7, "-");
                text = cleanedPhoneStr;
                --cursorPosition;
            }
        }

		ofNotifyEvent(textChanged, text, this);
		return;
	}
	
	if (key==OF_KEY_LEFT){
		if(selecting) {
			cursorPosition = selectionBegin;
			selecting = false;
			
		} else {
			if (cursorPosition>0){
				--cursorPosition;
			}
		}
		return;
	}
	if (key==OF_KEY_RIGHT){
		if(selecting) {
			cursorPosition = selectionEnd;
			selecting = false;
		} else {
			if (cursorPosition<text.size()){
				++cursorPosition;
			}
		}
		return;
	}
	if (key==OF_KEY_UP){
		if(selecting) {
			cursorPosition = selectionBegin;
			selecting = false;
			
		} else {
			if (cursorPosition>0) {
				int xx, yy;
				getCursorCoords(cursorPosition, xx, yy);
				if(yy>0) {
					yy--;
					vector<string> lines = ofSplitString(text, "\n");
					xx = MIN(lines[yy].size()-1, xx);
					cursorPosition = xx;
					for(int i = 0; i < yy; i++) cursorPosition += lines[i].size()+1;
					printf("Cursor position: %d\n", cursorPosition);
				} else {
					cursorPosition = 0;
				}
			}
		}
		return;
	}
	if (key==OF_KEY_DOWN){
		if(selecting) {
			cursorPosition = selectionEnd;
			selecting = false;
		} else {
			int xx, yy;
			getCursorCoords(cursorPosition, xx, yy);
			vector<string> lines = ofSplitString(text, "\n");
			yy++;
			if(yy<lines.size()-1) {
				
				xx = MIN(lines[yy].size()-1, xx);
				cursorPosition = xx;
				for(int i = 0; i < yy; i++) cursorPosition += lines[i].size()+1;
				printf("Cursor position: %d\n", cursorPosition);
			} else {
				cursorPosition = text.size()-1;
			}
		}
		return;
	}

	
	
	//if ((key >=32 && key <=126) || key=='\t') {
 //       
 //       if(isShifted) {
 //           
 //           char toInsert;
 //           if( !(key > 96 && key < 123) && !(key > 65 && key < 90) && shiftMap.find(key) != shiftMap.end() ) {
 //               toInsert = shiftMap[key];//toInsert = key - 32;
 //           } else {
 //               toInsert = key;
 //           }
 //           
 //           text.insert(text.begin()+cursorPosition, toInsert);
 //       } else {
        
        if ((bIsPhoneNumber && text.size() >= 13) ||
            (bIsPhoneNumber && (key < 48 || key > 57))) {
            return;
        }
        
        text.insert(text.begin()+cursorPosition, key);
        cursorPosition++;

        if (bIsPhoneNumber) {
            if (text.size() == 3 || text.size() == 7) {
                text.insert(text.begin()+cursorPosition, 45);
                cursorPosition++;
            }
            
            //
            if (text.size() == 13) {
                string cleanedPhoneStr = text;
                
                regex regExDigit("[^0-9]");
                cleanedPhoneStr = regex_replace(cleanedPhoneStr, regExDigit, "");
                
                regex regExLeadindSpaces("^\\s+");
                cleanedPhoneStr = regex_replace(cleanedPhoneStr, regExLeadindSpaces, "");
                
                regex regExTrailingSpaces("\\s+$");
                cleanedPhoneStr = regex_replace(cleanedPhoneStr, regExTrailingSpaces, "");
                
                cleanedPhoneStr.insert(1, " ");
                cleanedPhoneStr.insert(5, "-");
                cleanedPhoneStr.insert(9, "-");
                text = cleanedPhoneStr;
                cursorPosition++;
            }
        }
        //}

		ofNotifyEvent(textChanged, text, this);
	//}
}

void ofxTextInputField::keyReleased(ofKeyEventArgs &a)
{
    
#if defined(TARGET_OSX)
	if(a.key == OF_KEY_COMMAND) {  // EZ: or 4532, test this on Mac
#elif defined(TARGET_WIN32)
	if(a.key == OF_KEY_CONTROL) {
#endif
        isCommand = false;
    }

    if(a.key == OF_KEY_SHIFT) {
        isShifted = false;
    }
}

void ofxTextInputField::clear() {
	text.clear();
	cursorPosition = 0;
}
    
void ofxTextInputField::setIsPhoneNumber(bool val)
{
    bIsPhoneNumber = val;
}
