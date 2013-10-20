// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <contrib/utf8.h>
#include <moai-sim/MOAIAnimCurve.h>
#include <moai-sim/MOAIFont.h>
#include <moai-sim/MOAITextBox.h>
#include <moai-sim/MOAITextDesigner.h>
#include <moai-sim/MOAITextStyle.h>

//================================================================//
// MOAITextDesigner
//================================================================//

//----------------------------------------------------------------//
void MOAITextDesigner::AcceptLine () {

	this->mLayout->PushLine ( this->mLineSpriteID, this->mLineSize, this->mLineRect, this->mLineAscent );
			
	// end line
	this->mPen.mY += this->mLineRect.Height () + this->mLineSpacing;
	this->mPen.mY = ZLFloat::Floor ( this->mPen.mY + 0.5f );
	this->mLineRect.Init ( 0.0f, this->mPen.mY, 0.0f, this->mPen.mY );
	
	// next line
	this->mLineSize = 0;
	this->mLineIdx = this->mTokenIdx;
	this->mLineSpriteID = this->mTokenSpriteID;
	this->mLineAscent = 0.0f;
	
	this->mPrevGlyph = 0;
	
	if ( this->mTokenSize ){
		
		// slide the current token (if any) back to the origin
		for ( u32 i = 0; i < this->mTokenSize; ++i ) {
			MOAITextSprite& sprite = this->mLayout->mSprites [ this->mTokenSpriteID + i ];
			sprite.mX -= this->mTokenRect.mXMin;
			sprite.mY = this->mPen.mY;
		}
		
		this->mPen.mX -= this->mTokenRect.mXMin;
		this->mTokenRect.Init ( 0.0f, this->mPen.mY, this->mTokenRect.Width (), this->mPen.mY + this->mTokenRect.Height ());
	}
	else {
		this->mPen.mX = 0.0f;
		float scale = this->mGlyphScale * ( this->mStyle ? this->mStyle->mScale : 1.0f ) * this->mDeckScale;
		this->mTokenRect.Init ( 0.0f, this->mPen.mY, 0.0f, this->mPen.mY + this->mDeck->mHeight * scale );
	}
}

//----------------------------------------------------------------//
void MOAITextDesigner::AcceptToken () {

	if ( this->mTokenSize ) {
	
		if ( this->mLineSize == 0 ) {
			this->mLineIdx = this->mTokenIdx;
			this->mLineSpriteID = this->mTokenSpriteID;
		}
		
		this->mLineRect.Grow ( this->mTokenRect );
		this->mLineSize += this->mTokenSize;
		this->mLineAscent = this->mTokenAscent > this->mLineAscent ? this->mTokenAscent : this->mLineAscent;
		
		this->mTokenSize = 0;
		this->mTokenIdx = this->mPrevIdx;
		this->mTokenSpriteID = this->mLayout->mSprites.GetTop ();
	}
}

//----------------------------------------------------------------//
void MOAITextDesigner::Align () {

	//bool hasSprites = ( this->mLayout->mSprites.GetTop () > 0 );
	//
	//float yOff = this->mTextBox->mFrame.mYMin;
	//float layoutHeight = this->mLineRect.mYMax;

	//switch ( this->mVAlign ) {
	//	
	//	case MOAITextBox::CENTER_JUSTIFY:
	//		yOff = ( yOff + ( this->mHeight * 0.5f )) - ( layoutHeight * 0.5f );
	//		
	//	case MOAITextBox::LEFT_JUSTIFY:
	//		break;
	
	//	case MOAITextBox::RIGHT_JUSTIFY:
	//		yOff = this->mTextBox->mFrame.mYMax - layoutHeight;
	//}
	//
	//yOff = ZLFloat::Floor ( yOff + 0.5f );
	//
	//u32 totalLines = this->mLayout->mLines.GetTop ();
	//for ( u32 i = 0; i < totalLines; ++i ) {
	//	MOAITextLine& line = this->mTextBox->mLines [ i ];
	//	
	//	float xOff = this->mTextBox->mFrame.mXMin;
	//	float lineWidth = line.mRect.Width ();
	//	
	//	switch ( this->mHAlign ) {
	//	
	//		case MOAITextBox::CENTER_JUSTIFY:
	//			xOff = ( xOff + ( this->mWidth * 0.5f )) - ( lineWidth * 0.5f );
	//			
	//		case MOAITextBox::LEFT_JUSTIFY:
	//			break;

	//		case MOAITextBox::RIGHT_JUSTIFY:
	//			xOff = this->mTextBox->mFrame.mXMax - lineWidth;
	//	}
	//	
	//	xOff = ZLFloat::Floor ( xOff + 0.5f );
	//	
	//	line.mRect.Offset ( xOff, yOff );
	//	
	//	if ( hasSprites ) {
	//	
	//		float spriteYOff = yOff + line.mAscent;
	//		
	//		MOAIAnimCurve* curve = 0;
	//		if ( this->mCurves ) {
	//			curve = this->mCurves [ i % this->mTotalCurves ];
	//		}
	//		
	//		for ( u32 j = 0; j < line.mSize; ++j ) {	
	//			MOAITextSprite& sprite = this->mTextBox->mSprites [ line.mStart + j ];
	//			
	//			sprite.mX += xOff;
	//			
	//			if ( curve ) {
	//				sprite.mY += spriteYOff + curve->GetValue (( sprite.mX - this->mTextBox->mFrame.mXMin ) / this->mWidth );
	//			}
	//			else {
	//				sprite.mY += spriteYOff;
	//			}
	//		}
	//	}
	//}
}

//----------------------------------------------------------------//
void MOAITextDesigner::BuildLayout () {
	
	if ( !this->mTextBox ) return;
	
	bool more = true;
	while ( more ) {
	
		u32 c = this->NextChar ();
		
		float scale = this->mGlyphScale * ( this->mStyle ? this->mStyle->mScale : 1.0f ) * this->mDeckScale;
		
		if ( MOAIFont::IsControl ( c )) {
		
			if ( c == '\n' ) {
				
				this->AcceptToken ();
				
				if ( !this->mLineRect.Height ()) {
					this->mLineRect.mYMax += this->mDeck->mHeight * scale;
				}
				
				this->AcceptLine ();	
			}
			else if ( c == 0 ) {
			
				this->AcceptToken ();
				this->AcceptLine ();
			
				this->mMore = false;
				more = false;
			}
		}
		else {
			MOAIGlyph* glyph = this->mDeck->GetGlyph ( c );
			if ( !glyph ) continue;
			if ( glyph->mCode == MOAIGlyph::NULL_CODE_ID ) continue;
			
			// apply kerning
			if ( this->mPrevGlyph ) {
				MOAIKernVec kernVec = this->mPrevGlyph->GetKerning ( glyph->mCode );
				this->mPen.mX += kernVec.mX * scale;
			}
			
			this->mPrevGlyph = glyph;
			
			if ( MOAIFont::IsWhitespace ( c )) {
				this->AcceptToken ();
			}
			else {
				
				float glyphBottom = this->mPen.mY + ( this->mDeck->mHeight * scale );
				float glyphRight = this->mPen.mX + ( glyph->mBearingX + glyph->mWidth ) * scale ;
				
				// handle new token
				if ( this->mTokenSize == 0 ) {
					this->mTokenIdx = this->mPrevIdx;
					this->mTokenSpriteID = this->mLayout->mSprites.GetTop ();
					this->mTokenRect.Init ( this->mPen.mX, this->mPen.mY, this->mPen.mX, glyphBottom );
					this->mTokenAscent = this->mDeck->mAscent * scale;
				}
				
				bool overrun = this->mWidth < glyphRight; // the right side of this glype will fall outside of the text box
				bool discard = ( this->mLineSize == 0 ) && overrun; // this is the first token in the line *and* we have overrun
				
				// if we're the first token in a line *and* have overrun, don't attempt to split the token - just
				// discard the extra glyphs. later on this will bethe place to implement fancy/custom token splitting.
				if ( !discard ) {
				
					// push the sprite
					this->mLayout->PushSprite ( this->mPrevIdx, *glyph, *this->mStyle, this->mPen.mX, this->mPen.mY, scale );
					this->mTokenRect.mXMax = glyphRight;
					this->mTokenSize++;
				}
				
				if ( overrun ) {
					this->AcceptLine ();
				}
				
				if ( this->mWordBreak == MOAITextBox::WORD_BREAK_CHAR ) {
					this->AcceptToken ();
				}
			}
			
			// advance the pen
			this->mPen.mX += glyph->mAdvanceX * scale;
		}
		
		// if we overrun this->mHeight, then back up to the start of the current line
		if ( this->mTokenRect.mYMax > this->mHeight ) {
			
			this->mLayout->mSprites.SetTop ( this->mLineSpriteID );
			
			// if we're ending on an empty line (i.e. a newline) then throw it away
			// else back up so the next page will start on the line
			if ( this->mLineSize ) {
				this->mIdx = this->mLineIdx;
			}
			else if ( this->mTokenSize ) {
				this->mIdx = this->mTokenIdx;
			}
			else {
				this->mIdx = this->mIdx;
			}
			
			more = false;
		}
	}

	this->Align ();
}

//----------------------------------------------------------------//
void MOAITextDesigner::Init ( MOAITextLayout& layout, MOAITextStyler& styler, cc8* str, u32 idx ) {
	
	this->mLayout = 0;
	this->mStyler = 0;
	
	if ( styler.CountSpans () == 0 ) return;
	
	this->mLayout = &layout;
	this->mStyler = &styler;
	
	this->mLoc.mX = 0.0f;
	this->mLoc.mY = 0.0f;
	
	this->mWidth = 0.0f;
	this->mHeight = 0.0f;
	
	this->mLimitWidth = false;
	this->mLimitHeight = false;
	
	this->mHAlign = LEFT_JUSTIFY;
	this->mVAlign = LEFT_JUSTIFY;
	
	this->mWordBreak = WORD_BREAK_NONE;
	
	this->mGlyphScale = 1.0f;
	
	this->mStr = &( str [ idx ]);
	this->mIdx = 0;
	
	//this->mStr = textBox.mText;
	//this->mIdx = textBox.mCurrentPageIdx;
	
	this->mPrevIdx = 0;
	this->mStyleSpan = 0;
	this->mStyle = 0;
	
	this->mLineIdx = this->mIdx;
	this->mLineSpriteID = 0;
	this->mLineSize = 0;
	this->mLineAscent = 0.0f;
	
	this->mLineRect.Init ( 0.0f, 0.0f, 0.0f, 0.0f );
	
	this->mTokenIdx = this->mIdx;
	this->mTokenSpriteID = 0;
	this->mTokenSize = 0;
	this->mTokenAscent = 0.0f;
	
	this->mTokenRect.Init ( 0.0f, 0.0f, 0.0f, 0.0f );
	
	this->mPen.Init ( 0.0f, 0.0f );
	this->mPrevGlyph = 0;
	
	this->mMore = true;
}

//----------------------------------------------------------------//
MOAITextDesigner::MOAITextDesigner () {
}

//----------------------------------------------------------------//
MOAITextDesigner::~MOAITextDesigner () {
}

//----------------------------------------------------------------//
u32 MOAITextDesigner::NextChar () {

	bool newSpan = false;

	if ( !this->mStyleSpan ) {
		this->mStyleSpan = &this->mStyler->mStyleMap.Elem ( 0 );
		this->mSpanIdx = 0;
		newSpan = true;
	}

	if ( this->mIdx >= this->mStyleSpan->mTop ) {
		
		this->mStyleSpan = 0;
		
		u32 totalStyles = this->mStyler->mStyleMap.GetTop ();
		for ( this->mSpanIdx++; this->mSpanIdx < totalStyles; this->mSpanIdx++ ) {
			MOAITextStyleSpan& styleSpan = this->mStyler->mStyleMap.Elem ( this->mSpanIdx );
			
			if ( this->mIdx < styleSpan.mTop ) {
				this->mStyleSpan = &styleSpan;
				newSpan = true;
				break;
			}
		}
	}
	
	if ( this->mStyleSpan ) {
	
		if ( newSpan ) {
		
			if ( this->mIdx < this->mStyleSpan->mBase ) {
				this->mIdx = this->mStyleSpan->mBase;
			}
		
			this->mStyle = this->mStyleSpan->mStyle;
			assert ( this->mStyle );
			
			MOAIFont* font = this->mStyle->mFont;
			assert ( font );
			
			this->mDeck = font->GetGlyphSet ( this->mStyle->mSize );
			this->mDeckScale = this->mDeck && ( this->mStyle->mSize > 0.0f ) ? this->mStyle->mSize / this->mDeck->GetSize () : 1.0f;
		}
		
		this->mPrevIdx = this->mIdx;
		u32 c = u8_nextchar ( this->mStr, &this->mIdx );
		return c;
	}
	return 0;
}

//----------------------------------------------------------------//
void MOAITextDesigner::SetCurves ( MOAIAnimCurve** curves, u32 totalCurves ) {

	this->mCurves = curves;
	this->mTotalCurves = totalCurves;
}
