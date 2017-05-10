#pragma once
#include "interface/gui_window.h"

class main_editor_window : public gui_window
{
public:
	//-----------------------------------------------------------------------------
	//  Name : main_editor_window ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	main_editor_window();
	
	//-----------------------------------------------------------------------------
	//  Name : main_editor_window ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	main_editor_window(sf::VideoMode mode, const std::string& title, std::uint32_t style = sf::Style::Default);
	
	//-----------------------------------------------------------------------------
	//  Name : ~ProjectManagerWindow (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual ~main_editor_window();

	//-----------------------------------------------------------------------------
	//  Name : on_gui (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void on_gui(std::chrono::duration<float> dt);

	//-----------------------------------------------------------------------------
	//  Name : on_menubar (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void on_menubar();

	//-----------------------------------------------------------------------------
	//  Name : on_toolbar (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void on_toolbar();

	//-----------------------------------------------------------------------------
	//  Name : render_dockspace (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void render_dockspace();

	//-----------------------------------------------------------------------------
	//  Name : on_start_page (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void on_start_page();

	//-----------------------------------------------------------------------------
	//  Name : show_start_page ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void show_start_page(bool show) { _show_start_page = show; }

private:
	///
	bool _show_start_page = true;
};