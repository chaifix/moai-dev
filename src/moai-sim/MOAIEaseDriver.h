// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAIEASEDRIVER_H
#define MOAIEASEDRIVER_H

#include <moai-sim/MOAITimer.h>

class MOAINode;

//================================================================//
// MOAIEaseDriverLink
//================================================================//
class MOAIEaseDriverLink {
public:
	
	MOAILuaSharedPtr < MOAINode >	mSource;
	MOAIAttrID						mSourceAttrID;
	
	MOAILuaSharedPtr < MOAINode >	mDest;
	MOAIAttrID						mDestAttrID;
	
	float							mV0;
	float							mV1;
	u32								mMode;
};

//================================================================//
// MOAIEaseDriver
//================================================================//
/**	@lua MOAIEaseDriver
	@text Action that applies simple ease curves to node attributes.
*/
class MOAIEaseDriver :
	public MOAITimer {
private:

	ZLLeanArray < MOAIEaseDriverLink > mLinks;

	//----------------------------------------------------------------//
	static int		_reserveLinks			( lua_State* L );
	static int		_setLink				( lua_State* L );

	//----------------------------------------------------------------//
	void			MOAIAction_Update		( double step );

public:

	DECL_LUA_FACTORY ( MOAIEaseDriver )

	//----------------------------------------------------------------//
					MOAIEaseDriver			();
					~MOAIEaseDriver			();
	u32				ParseForMove			( MOAILuaState& state, int idx, MOAINode* dest, u32 total, int mode, ... );
	u32				ParseForSeek			( MOAILuaState& state, int idx, MOAINode* dest, u32 total, int mode, ... );
	void			RegisterLuaClass		( MOAILuaState& state );
	void			RegisterLuaFuncs		( MOAILuaState& state );
	void			ReserveLinks			( u32 total );
	void			SetLink					( ZLIndex idx, MOAINode* dest, MOAIAttrID destAttrID, float v1, u32 mode );
	void			SetLink					( ZLIndex idx, MOAINode* dest, MOAIAttrID destAttrID, MOAINode* source, MOAIAttrID sourceAttrID, u32 mode );
};

#endif
