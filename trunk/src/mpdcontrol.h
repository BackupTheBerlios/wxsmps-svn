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

#ifndef MPDCONTROL_H_
#define MPDCONTROL_H_
#include "contrib/libmpd/libmpd.h"
#include "lcdwidgets.h"
#include <wx/thread.h>
class MpdControl : public wxThreadHelper {
public:
	MpdControl(LcdWidgetsBase * _lcdWidgets);
	void            *Entry();
	void 		    Run();
	bool		    IsRunning();
	bool		    IsConnected();
	
	static void	    HandleStatusChangedCB(MpdObj* mi,ChangedStatusType statusType, void* userData);
	static void     HandleConnectionStateChangedCB(MpdObj* mi,int connect, void* userData);
	static void     HandleErrorCB(MpdObj * mi, int id, char *msg, void* userData);
	
	/*mpd controlling functions*/
	void		    Stop();
	void		    ToggleState();
	void		    Play();
	void		    Pause();
	void		    NextSong();
	void		    PrevSong();
	void		    SeekInCurrentSong(int seek);
	void		    ToggleRandom();
	void		    ToggleRepeat();
	void		    SetVolume(int diff);
	void		    SetCrossfade(int crossfade);
	void		    ClearPlayList();
	void		    RemoveSongFromPlayList(int id);
	void		    PlaySongFromPlayList(int id);
	void		    CropPlayListSong(int id);
	
	
	
private:
	/*member functions*/
	void		    OnStatusChanged(ChangedStatusType status);
	void		    OnConnectionChanged(int connected);
	void		    OnError(int id, char* msg);
	bool		    Connect();
	bool		    ReconnectWithTimeout();
	/*data members*/
	MpdObj 		    * dm_mpdConnection;
	LcdWidgetsBase	* p_lcdWidgets;
	bool		    dm_isConnected;
};

#endif /*MPDCONTROL_H_*/
