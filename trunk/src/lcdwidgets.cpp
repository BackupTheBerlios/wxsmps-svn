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

#include <iostream>
#include <wx/app.h>
#include <wx/datetime.h>
#include <wx/filename.h>
#include "main.h"
#include "lcdwidgets.h"

/***************************************************
 * **********LcdWidgetsBase Class*******************
 * *************************************************/

void LcdWidgetsBase::UpdateState(MpdState _state){
	this->dm_state=_state;
	this->UpdateStateChar();
}

void LcdWidgetsBase::UpdateElapsedTime(int _elapsedTime){
	this->dm_elapsedTime=_elapsedTime;
	this->UpdateElapsedTimeStr();
}

void LcdWidgetsBase::UpdateTotalTime(int _totalTime){
	this->dm_totalTime=_totalTime;
	this->UpdateTotalTimeStr();
}

void LcdWidgetsBase::UpdateRandom(bool _random){
	this->dm_random=_random;
	this->UpdateRandomChar();
}

void LcdWidgetsBase::UpdateRepeat(bool _repeat){
	this->dm_repeat=_repeat;
	this->UpdateRepeatChar();
}

void LcdWidgetsBase::UpdateCrossfade(int _crossfade){
	this->dm_crossfade = _crossfade;
	
	p_lcdComMenu->AddPendingMsg(wxString::Format(wxT("menu_set_item %s %s -value %d"),wxT(M_M_SETTINGS),wxT(M_SET_CROSSFADE),_crossfade));
}

void LcdWidgetsBase::UpdateCurSongID(int id){
	this->dm_curSongID=id;
}

void LcdWidgetsBase::UpdatePlaylist(MpdData * _mpdData,int plLen){
	
	this->p_lcdComMenu->AddPendingMsg(wxString::Format(wxT("menu_del_item \"%s\" %s"),wxT(M_M_PLAYLIST),wxT(M_PL_BROWSE)));
	this->p_lcdComMenu->AddPendingMsg(wxString::Format(wxT("menu_del_item \"%s\" %s"),wxT(M_M_PLAYLIST),wxT(M_PL_CLEAR)));
	this->p_lcdComMenu->AddPendingMsg(wxString::Format(wxT("menu_add_item \"%s\" %s menu \"%s\""),wxT(M_M_PLAYLIST),wxT(M_PL_BROWSE),wxT(M_PL_BROWSE_LABEL)));
	this->p_lcdComMenu->AddPendingMsg(wxString::Format(wxT("menu_add_item \"%s\" %s menu \"%s\""),wxT(M_M_PLAYLIST),wxT(M_PL_CLEAR),wxT(M_PL_CLEAR_LABEL)));
	this->p_lcdComMenu->AddPendingMsg(wxString::Format(wxT("menu_add_item \"%s\" %s action \"%s\" -next \"_quit_\" "),wxT(M_PL_CLEAR),wxT(M_PL_CLEAR_YES),wxT(M_PL_CLEAR_YES_LABEL)));
	this->p_lcdComMenu->AddPendingMsg(wxString::Format(wxT("menu_add_item \"%s\" %s action \"%s\" -next \"_close_\""),wxT(M_PL_CLEAR),wxT(M_PL_CLEAR_NO),wxT(M_PL_CLEAR_NO_LABEL)));	
	
	if(plLen > 0 ){
		while(_mpdData){
			if(_mpdData->type == MPD_DATA_TYPE_SONG){
				int id = _mpdData->song->id;
				int pos = _mpdData->song->pos;
				wxString * strTitle;
				if(_mpdData->song->title){
					strTitle = new wxString(_mpdData->song->title,wxConvUTF8); 
				}
				else{
					strTitle = new wxString(wxT(""));
					this->ExtractFileNameFromSongPath(_mpdData->song,*strTitle);
				}
				this->EscapeChars(*strTitle);
				wxString itemLabel = wxString::Format(wxT("%.2d"),pos+1);
				itemLabel = itemLabel + wxT(" ") + *strTitle;
				
				this->p_lcdComMenu->AddPendingMsg(wxString::Format(wxT("menu_add_item \"%s\" pl_song_%d menu \"%s\""),wxT(M_PL_BROWSE),id,itemLabel.c_str()));
				this->p_lcdComMenu->AddPendingMsg(wxString::Format(wxT("menu_add_item \"pl_song_%d\" \"play_pl_song_%d\" action \"Play\" -next \"_quit_\""),id,id));
				this->p_lcdComMenu->AddPendingMsg(wxString::Format(wxT("menu_add_item \"pl_song_%d\" \"rem_pl_song_%d\" action \"Remove\" -next \"_quit_\""),id,id));
				this->p_lcdComMenu->AddPendingMsg(wxString::Format(wxT("menu_add_item \"pl_song_%d\" \"crop_pl_song_%d\" action \"Crop\" -next \"_quit_\""),id,id));				
				
				if(strTitle){
					delete strTitle;
				}
			}
			_mpdData=mpd_data_get_next(_mpdData);
			/*if(plLen>12){
				wxString tmp;
				this->p_lcdCom->GetLcdResponce(tmp,1);
			}*/
		}
		/*
		else{//pointed to nothing
		
		}*/
	}
	else {//playlist is empty
		this->p_lcdComMenu->AddPendingMsg(wxString::Format(wxT("menu_add_item \"%s\" pl_empty action \"empty\" -next \"_quit_\""),wxT(M_PL_BROWSE)));
		
	}
	
	if(_mpdData){
		mpd_data_free(_mpdData);
	}
}

void LcdWidgetsBase::InitMainMenu(){
	p_lcdComMenu->AddPendingMsg(wxString::Format(wxT("menu_add_item \"\" %s menu \"%s\""),wxT(M_M_PLAYLIST),wxT(M_M_PLAYLIST_LABEL)));
	p_lcdComMenu->AddPendingMsg(wxString::Format(wxT("menu_add_item \"%s\" %s menu \"%s\""),wxT(M_M_PLAYLIST),wxT(M_PL_BROWSE),wxT(M_PL_BROWSE_LABEL)));
	p_lcdComMenu->AddPendingMsg(wxString::Format(wxT("menu_add_item \"%s\" %s menu \"%s\""),wxT(M_M_PLAYLIST),wxT(M_PL_CLEAR),wxT(M_PL_CLEAR_LABEL)));
	p_lcdComMenu->AddPendingMsg(wxString::Format(wxT("menu_add_item \"%s\" %s action \"%s\" -next \"_quit_\""),wxT(M_PL_CLEAR),wxT(M_PL_CLEAR_YES),wxT(M_PL_CLEAR_YES_LABEL)));
	p_lcdComMenu->AddPendingMsg(wxString::Format(wxT("menu_add_item \"%s\" %s action \"%s\" -next \"_close_\""),wxT(M_PL_CLEAR),wxT(M_PL_CLEAR_NO),wxT(M_PL_CLEAR_NO_LABEL)));
	p_lcdComMenu->AddPendingMsg(wxString::Format(wxT("menu_add_item \"\" %s menu \"%s\""),wxT(M_M_MEDIA),wxT(M_M_MEDIA_LABEL)));
	p_lcdComMenu->AddPendingMsg(wxString::Format(wxT("menu_add_item \"\" %s menu \"%s\""),wxT(M_M_SETTINGS),wxT(M_M_SETTINGS_LABEL)));
	p_lcdComMenu->AddPendingMsg(wxString::Format(wxT("menu_add_item \"\" %s menu \"%s\""),wxT(M_M_EXIT),wxT(M_M_EXIT_LABEL)));
	p_lcdComMenu->AddPendingMsg(wxString::Format(wxT("menu_add_item \"%s\" %s action \"%s\" -next \"_quit_\""),wxT(M_M_EXIT),wxT(M_EXIT_YES),wxT(M_EXIT_YES_LABEL)));
	p_lcdComMenu->AddPendingMsg(wxString::Format(wxT("menu_add_item \"%s\" %s action \"%s\" -next \"_close_\""),wxT(M_M_EXIT),wxT(M_EXIT_NO),wxT(M_EXIT_NO_LABEL)));
	p_lcdComMenu->AddPendingMsg(wxString::Format(wxT("menu_add_item \"%s\" %s numeric \"%s\" -value %d"),wxT(M_M_SETTINGS),wxT(M_SET_CROSSFADE),wxT(M_SET_CROSSFADE_LABEL),DEFAUT_CROSSFADE_INT));
	p_lcdComMenu->AddPendingMsg(wxString::Format(wxT("menu_set_main \"\"")));
}

void LcdWidgetsBase::InitClientKeys(){
	p_lcdComStatus->SendToLcd(wxString::Format(wxT("client_add_key %s"),wxT(DEFAULT_KEY_STOP_STRING)));
	p_lcdComStatus->SendToLcd(wxString::Format(wxT("client_add_key %s"),wxT(DEFAULT_KEY_REPEAT_STRING)));
	p_lcdComStatus->SendToLcd(wxString::Format(wxT("client_add_key %s"),wxT(DEFAULT_KEY_PREV_SONG_STRING)));
	p_lcdComStatus->SendToLcd(wxString::Format(wxT("client_add_key %s"),wxT(DEFAULT_KEY_RANDOM_STRING)));
	p_lcdComStatus->SendToLcd(wxString::Format(wxT("client_add_key %s"),wxT(DEFAULT_KEY_SEEK_BACK_STRING)));
	p_lcdComStatus->SendToLcd(wxString::Format(wxT("client_add_key %s"),wxT(DEFAULT_KEY_PLAY_PAUSE_STRING)));
	p_lcdComStatus->SendToLcd(wxString::Format(wxT("client_add_key %s"),wxT(DEFAULT_KEY_SEEK_FORWARD_STRING)));
	p_lcdComStatus->SendToLcd(wxString::Format(wxT("client_add_key %s"),wxT(DEFAULT_KEY_NEXT_SONG_STRING)));
	p_lcdComStatus->SendToLcd(wxString::Format(wxT("client_add_key %s"),wxT(DEFAULT_KEY_VOL_UP_STRING)));
	p_lcdComStatus->SendToLcd(wxString::Format(wxT("client_add_key %s"),wxT(DEFAULT_KEY_VOL_DOWN_STRING)));
	p_lcdComStatus->SendToLcd(wxString::Format(wxT("client_add_key %s"),wxT(DEFAULT_KEY_CLEAR_PLAYLIST_STRING)));
}

void LcdWidgetsBase::UpdateStateChar(){
	this->dm_charState=DEFAULT_UNKOWN_CHAR;
	switch(this->dm_state){
		case MPD_PLAYER_PAUSE:
			this->dm_charState=DEFAULT_STATE_PAUSED_CHAR;
			break;
		case MPD_PLAYER_PLAY:
			this->dm_charState=DEFAULT_STATE_PLAYING_CHAR;
			break;
		case MPD_PLAYER_STOP:
			this->dm_charState=DEFAULT_STATE_STOPPED_CHAR;
			break;
		case MPD_PLAYER_UNKNOWN:
			this->dm_charState=DEFAULT_UNKOWN_CHAR;
			break;
		default:
			this->dm_charState=DEFAULT_UNKOWN_CHAR;
			break;
	}
	this->UpdatePlayerStateLine();
}

void LcdWidgetsBase::UpdateElapsedTimeStr(){
	
	if(this->dm_elapsedTime>=0){
		wxTimeSpan tp(0,0,this->dm_elapsedTime,0);
		this->dm_elapsedTimeStr=tp.Format(this->dm_elapsedTimeStrFormat);
	}
	this->UpdatePlayerStateLine();
	
}

void LcdWidgetsBase::UpdateTotalTimeStr(){
	
	if(this->dm_totalTime>=0){
		wxTimeSpan tp(0,0,this->dm_totalTime,0);
		this->dm_totalTimeStr=tp.Format(wxT("%M:%S"));
	}
	
}

void LcdWidgetsBase::UpdateRandomChar(){		
	this->dm_charRandom = DEFAULT_UNKOWN_CHAR;
	if(this->dm_random){
		this->dm_charRandom = DEFAULT_RANDOM_ON_CHAR;
	}
	else{
		this->dm_charRandom = DEFAULT_RANDOM_OFF_CHAR;
	}
	this->UpdatePlayerStateLine();
}

void LcdWidgetsBase::UpdateRepeatChar(){
	this->dm_charRepeat = DEFAULT_UNKOWN_CHAR;
	if(this->dm_repeat){
		this->dm_charRepeat = DEFAULT_REPEAT_ON_CHAR;
	}
	else{
		this->dm_charRepeat = DEFAULT_REPEAT_OFF_CHAR;
	}
	this->UpdatePlayerStateLine();
}

void LcdWidgetsBase::ExtractFileNameFromSongPath(mpd_Song * song,wxString & fileName){
	if(song!=NULL){
		if(song->file){
			wxString path(song->file,wxConvUTF8);
			wxFileName fnObj(path);
			fileName=fnObj.GetName();
		}
		else{
			std::cout << "ExtractFileNameFromSongPath(): song->file is not set!!!" << std::endl;
			fileName=wxT("n/a");
		}

	}
	else{
		std::cout << "ExtractFileNameFromSongPath(): Pointer to song is NULL!!!" << std::endl;
		fileName=wxT("n/a");
	}
}

void LcdWidgetsBase::EscapeChars(wxString &str){
	if(!str.IsEmpty()){
		/* \ -> \\*/
		if(str.Replace(wxT("\\"),wxT("\\\\"),true)){
			std::cout <<"Replaced [\\] with [\\\\]" <<std::endl;
		}
		/* " -> \"*/
		if(str.Replace(wxT("\""),wxT("\\\""),true)){
			std::cout <<"Replaced [\"] with [\\\"]" <<std::endl;
		}
		/* ´ -> '*/
		if(str.Replace(wxT("´"),wxT("'"),true)){
			std::cout <<"Replaced [´] with [']" <<std::endl;
		}
	}
}

//just declared the virtual functions. they will be overwritten in derived classes
void LcdWidgetsBase::UpdateVolume(int){}
void LcdWidgetsBase::UpdatePlayedSong(mpd_Song *){}
void LcdWidgetsBase::UpdatePlayerStateLine(){}
void LcdWidgetsBase::InitPlayerScreen(){}


/***************************************************
 * **********LcdWidgetsFourLines Class**************
 * *************************************************/
void LcdWidgetsFourLines::InitPlayerScreen(){
/*
       ┌────────────────────┐
       │ Artist             │
       │ Title              │
       │ Album              │
       │0:00:00 00% ???V000%│
       └────────────────────┘
*/
	
	if(this->p_lcdComStatus!=NULL){
		p_lcdComStatus->SendToLcd(wxString::Format(wxT("screen_add %s"),wxT(SCREEN_PLAYER)));
		p_lcdComStatus->SendToLcd(wxString::Format(wxT("screen_set %s -priority foreground"),wxT(SCREEN_PLAYER)));
		p_lcdComStatus->SendToLcd(wxString::Format(wxT("screen_set %s -heartbeat off"),wxT(SCREEN_PLAYER)));
		p_lcdComStatus->SendToLcd(wxString::Format(wxT("widget_add %s %s scroller"),wxT(SCREEN_PLAYER),wxT(SCREEN_PLAYER_SCROLLER_ARTIST)));
		p_lcdComStatus->SendToLcd(wxString::Format(wxT("widget_set %s %s 1 1 %d 1 h %d \"%s\""),	wxT(SCREEN_PLAYER),wxT(SCREEN_PLAYER_SCROLLER_ARTIST),this->dm_lcdWidth,
																							DEFAULT_SCROLLER_SPEED,wxT(DEFAULT_ARTIST_STRING)));
		p_lcdComStatus->SendToLcd(wxString::Format(wxT("widget_add %s %s scroller"),wxT(SCREEN_PLAYER),wxT(SCREEN_PLAYER_SCROLLER_TITLE)));
		p_lcdComStatus->SendToLcd(wxString::Format(wxT("widget_set %s %s 1 2 %d 1 h %d \"%s\""),	wxT(SCREEN_PLAYER),wxT(SCREEN_PLAYER_SCROLLER_TITLE),this->dm_lcdWidth,
																							DEFAULT_SCROLLER_SPEED,wxT(DEFAULT_TITLE_STRING)));
		p_lcdComStatus->SendToLcd(wxString::Format(wxT("widget_add %s %s scroller"),wxT(SCREEN_PLAYER),wxT(SCREEN_PLAYER_SCROLLER_ALBUM)));
		p_lcdComStatus->SendToLcd(wxString::Format(wxT("widget_set %s %s 1 3 %d 1 h %d \"%s\""),	wxT(SCREEN_PLAYER),wxT(SCREEN_PLAYER_SCROLLER_ALBUM),this->dm_lcdWidth,
																							DEFAULT_SCROLLER_SPEED,wxT(DEFAULT_ALBUM_STRING)));
		p_lcdComStatus->SendToLcd(wxString::Format(wxT("widget_add %s %s string"),wxT(SCREEN_PLAYER),wxT(SCREEN_PLAYER_STRING_STATELINE)));
		this->UpdateStateChar(); this->UpdateRandomChar(); this->UpdateRepeatChar();this->UpdateElapsedTimeStr();
	}
}

void LcdWidgetsFourLines::UpdatePlayedSong(mpd_Song * song){
	wxString * strArtist,* strTitle,* strAlbum;
	if(song!=NULL){
		/*update current song id*/
		this->UpdateCurSongID(song->id);
		
		/*artist*/
		if(song->artist){
			strArtist = new wxString(song->artist,wxConvUTF8);
			this->EscapeChars(*strArtist);
		}
		else {
			std::cout <<"Artist field not set" << std::endl;
			strArtist=new wxString(wxT("n/a"));
		}
		
		/*title*/
		if(song->title){
			strTitle = new wxString(song->title,wxConvUTF8);
		}
		else {
			std::cout <<"Artist field not set" << std::endl;
			strTitle = new wxString(wxT("n/a"));
			this->ExtractFileNameFromSongPath(song,*strTitle);
			
			
		}
		this->EscapeChars(*strTitle);
		*strTitle=*strTitle + wxT(" (") + this->dm_totalTimeStr + wxT(")");
		
		/*album*/
		if(song->album){
			strAlbum= new wxString(song->album,wxConvUTF8);
			this->EscapeChars(*strAlbum);
		}
		else {
			std::cout <<"Album field not set"<<std::endl;
			strAlbum=new wxString(wxT("n/a"));
		}
											
	}
	else {//pointer was NULL
		strArtist = new wxString(wxT(DEFAULT_ARTIST_STRING));
		strTitle = new wxString(wxT(DEFAULT_TITLE_STRING));
		strAlbum = new wxString(wxT(DEFAULT_ALBUM_STRING));
	}
	
	p_lcdComStatus->SendToLcd(wxString::Format(wxT("widget_set %s %s 1 1 %d 1 h %d \"%s\""),	wxT(SCREEN_PLAYER),wxT(SCREEN_PLAYER_SCROLLER_ARTIST),this->dm_lcdWidth,
																							DEFAULT_SCROLLER_SPEED,strArtist->c_str()));
	p_lcdComStatus->SendToLcd(wxString::Format(wxT("widget_set %s %s 1 2 %d 1 h %d \"%s\""),	wxT(SCREEN_PLAYER),wxT(SCREEN_PLAYER_SCROLLER_TITLE),this->dm_lcdWidth,
																							DEFAULT_SCROLLER_SPEED,strTitle->c_str()));
	
	p_lcdComStatus->SendToLcd(wxString::Format(wxT("widget_set %s %s 1 3 %d 1 h %d \"%s\""),	wxT(SCREEN_PLAYER),wxT(SCREEN_PLAYER_SCROLLER_ALBUM),this->dm_lcdWidth,
																						DEFAULT_SCROLLER_SPEED,strAlbum->c_str()));
																							
	if(strArtist){
		delete strArtist;
	}
	
	if(strTitle){
		delete strTitle;
	}
	
	if(strAlbum){
		delete strAlbum;
	}
	
			
}

/***************************************************
 * **********LcdWidgetsTwoLines Class***************
 * *************************************************/

void LcdWidgetsTwoLines::InitPlayerScreen(){
if(this->p_lcdComStatus!=NULL){
		p_lcdComStatus->SendToLcd(wxString::Format(wxT("screen_add %s"),wxT(SCREEN_PLAYER)));
		p_lcdComStatus->SendToLcd(wxString::Format(wxT("screen_set %s -priority foreground"),wxT(SCREEN_PLAYER)));
		p_lcdComStatus->SendToLcd(wxString::Format(wxT("screen_set %s -heartbeat off"),wxT(SCREEN_PLAYER)));
		p_lcdComStatus->SendToLcd(wxString::Format(wxT("widget_add %s %s scroller"),wxT(SCREEN_PLAYER),wxT(SCREEN_PLAYER_SCROLLER_ARTIST_TITLE)));
		p_lcdComStatus->SendToLcd(wxString::Format(wxT("widget_set %s %s 1 1 %d 1 h %d \"%s\""),	wxT(SCREEN_PLAYER),wxT(SCREEN_PLAYER_SCROLLER_ARTIST_TITLE),this->dm_lcdWidth,
																							DEFAULT_SCROLLER_SPEED,wxT(DEFAULT_ARTIST_TITLE_STRING)));
		
		p_lcdComStatus->SendToLcd(wxString::Format(wxT("widget_add %s %s string"),wxT(SCREEN_PLAYER),wxT(SCREEN_PLAYER_STRING_STATELINE)));
		this->UpdateStateChar(); this->UpdateRandomChar(); this->UpdateRepeatChar();this->UpdateElapsedTimeStr();
	}
}

void LcdWidgetsTwoLines::UpdatePlayedSong(mpd_Song * song){
	if(song!=NULL){
		/*update current song id*/
		this->UpdateCurSongID(song->id);
		
		wxString artistTitleStr;
		if(!song->artist || !song->title){
			this->ExtractFileNameFromSongPath(song,artistTitleStr);
		}
		else if(song->artist && song->title){
			wxString strArtist(song->artist,wxConvUTF8);
			wxString strTitle(song->title,wxConvUTF8);
			artistTitleStr=artistTitleStr + wxT(" - ") + strTitle;
		}
		this->EscapeChars(artistTitleStr);
		//append total track time (00:00)
		artistTitleStr=artistTitleStr + wxT(" (") + this->dm_totalTimeStr + wxT(")");
		
		p_lcdComStatus->SendToLcd(wxString::Format(wxT("widget_set %s %s 1 1 %d 1 h %d \"%s\""),	wxT(SCREEN_PLAYER),wxT(SCREEN_PLAYER_SCROLLER_ARTIST_TITLE),this->dm_lcdWidth,
																							DEFAULT_SCROLLER_SPEED,artistTitleStr.c_str()));
	}
	else{
		std::cout <<"Pointer to mpd_Song is NULL!!" << std::endl;
		
		p_lcdComStatus->SendToLcd(wxString::Format(wxT("widget_set %s %s 1 1 %d 1 h %d \"%s\""),	wxT(SCREEN_PLAYER),wxT(SCREEN_PLAYER_SCROLLER_ARTIST_TITLE),this->dm_lcdWidth,
																							DEFAULT_SCROLLER_SPEED,wxT(DEFAULT_ARTIST_TITLE_STRING)));	
	}
}





/***************************************************
 * **********LcdWidgetsWide Class*******************
 * *************************************************/
void LcdWidgetsWide::UpdateVolume(int _volume){
	this->dm_volume=_volume;
	this->UpdateVolumeStr();
}

void LcdWidgetsWide::UpdatePlayerStateLine(){
	this->UpdatePercTotalTimeStr();
	this->UpdateSpinnerChar();
	//Format %s%c%s%c%c%c%s
	wxString playerStateStr=wxString::Format(this->dm_playerStateStrFormat,this->dm_elapsedTimeStr.c_str(),spinnerCharArray[this->dm_spinner],this->dm_percTotalTimeStr.c_str(),this->dm_charState,this->dm_charRandom,
											this->dm_charRepeat,this->dm_volumeStr.c_str());
	
	p_lcdComStatus->SendToLcd(wxString::Format(wxT("widget_set %s %s 1 %d \"%s\""),wxT(SCREEN_PLAYER),wxT(SCREEN_PLAYER_STRING_STATELINE),this->dm_lcdHeight,playerStateStr.c_str()));	
}

void LcdWidgetsWide::UpdatePercTotalTimeStr(){
	int perc=0;
	if (this->dm_elapsedTime > 0 && this->dm_totalTime >0) {   // if player state is STOPPED _elapsedTime will always be 0
		perc = (this->dm_elapsedTime*100)/this->dm_totalTime;
	}
		
	if(perc < 100){//append '%'
		this->dm_percTotalTimeStr=this->dm_percTotalTimeStr=wxString::Format(wxT("%.2d"),perc)+wxT("%");
	}
	else if (perc == 100){//write out 100
		this->dm_percTotalTimeStr=this->dm_percTotalTimeStr=wxString::Format(wxT("%.2d"),perc);
	}//prevent write-out of >100
}

void LcdWidgetsWide::UpdateVolumeStr(){
	this->dm_volumeStr=wxString::Format(this->dm_volumeStrFormat,this->dm_volume);
	this->UpdatePlayerStateLine();
}

void LcdWidgetsWide::UpdateSpinnerChar(){
	if(this->dm_spinner<3){
		this->dm_spinner++;
	}
	else{
		this->dm_spinner=0;
	}
}

/***************************************************
 * **********LcdWidgetsSmall Class******************
 * *************************************************/
 void LcdWidgetsSmall::UpdatePlayerStateLine(){
	//Format %s%c%c%c
	wxString playerStateStr=wxString::Format(this->dm_playerStateStrFormat,this->dm_elapsedTimeStr.c_str(),this->dm_charState,this->dm_charRandom,this->dm_charRepeat);
	
	p_lcdComStatus->SendToLcd(wxString::Format(wxT("widget_set %s %s 1 %d \"%s\""),wxT(SCREEN_PLAYER),wxT(SCREEN_PLAYER_STRING_STATELINE),this->dm_lcdHeight,playerStateStr.c_str()));		
 }
