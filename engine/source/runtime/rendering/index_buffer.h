#pragma once

#include "graphics/graphics.h"

struct index_buffer
{
	//-----------------------------------------------------------------------------
	//  Name : ~index_buffer ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	~index_buffer();
	//-----------------------------------------------------------------------------
	//  Name : dispose ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void dispose();

	//-----------------------------------------------------------------------------
	//  Name : is_valid ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool is_valid() const;


	//-----------------------------------------------------------------------------
	//  Name : populate ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void populate(const gfx::Memory* _mem, std::uint16_t _flags = BGFX_BUFFER_NONE);

	/// Internal handle
	gfx::IndexBufferHandle handle = { gfx::invalidHandle };
};