/* (c) copyright 2006 Laurent Baum <lorijho@users.berlios.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
 
 
#ifndef MAIN_H_
#define MAIN_H_
#include "lcdcom.h"
#include "lcdwidgets.h"
#include "mpdcontrol.h"
#include <wx/thread.h>
class MyApp : public wxApp
{
public:
	
	virtual bool 	OnInit();
	virtual int		OnRun();
	virtual int		OnExit();
	int				MainLoop();
	void			ExitMainLoop();
	bool			IsMainLoopRunning();
	
	void 			IntroduceClient(const wxString & strName);
	bool			GetDimensionsFromLcdproc(const wxString & str);	
	bool			AreLcdDimensionsValide();
	void			SetFormats();
	void			ProcessKeyPress(const wxString& str);
	void			ProcessMenuEvents(const wxString& str);
private:
	LcdComStatus	* dm_lcdComStatus;
	LcdComMenu		* dm_lcdComMenu;
	LcdWidgetsBase	* dm_lcdWidgets;
	MpdControl		* dm_mpdControl;
	bool			dm_runningMainLoop;
	int				dm_w;
	int				dm_h;
};

//set this in the other app headers
DECLARE_APP(MyApp) // macro that enables ::wxGetApp()
#endif /*MAIN_H_*/
