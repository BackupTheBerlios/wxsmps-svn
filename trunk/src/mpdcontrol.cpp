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

#include <wx/datetime.h>
#include <iostream>
#include "main.h"
#include "mpdcontrol.h"

MpdControl::MpdControl(LcdWidgetsBase * _lcdWidgets)
:wxThreadHelper()
{
	this->p_lcdWidgets=_lcdWidgets;
	this->dm_mpdConnection = NULL;
	this->dm_mpdConnection = mpd_new_default();
	mpd_set_connection_timeout(this->dm_mpdConnection, 10);
}

void MpdControl::Run(){
	if( IsRunning() ) return;                   // we are already doing something!
    wxThreadHelper::Create();                   // create thread
    this->Connect();
    GetThread()->Run();                         // run.....
}

bool MpdControl::IsConnected(){
	return mpd_check_connected(this->dm_mpdConnection);
}

bool MpdControl::IsRunning()
{
    wxThread *t = GetThread();
    if( t ){
        if( t->IsRunning() ){
        	 return true;
        }
        else {
        	return false;
        }
    }

    return false;
}

void *MpdControl::Entry(){
	
	if(!this->IsConnected()){
		do{}while(!this->ReconnectWithTimeout());
	}
	
	mpd_signal_connect_status_changed(this->dm_mpdConnection,MpdControl::HandleStatusChangedCB,(void*)this);
	mpd_signal_connect_connection_changed(this->dm_mpdConnection,MpdControl::HandleConnectionStateChangedCB,(void*)this);
    mpd_signal_connect_error(this->dm_mpdConnection,MpdControl::HandleErrorCB,(void*)this);
	
	while(!GetThread()->TestDestroy()){
		mpd_status_update(this->dm_mpdConnection);//check update to receive signals
		GetThread()->Sleep(100);
	}
	return 0;
}

bool MpdControl::Connect(){
	/*attempt to connect*/
	mpd_connect(this->dm_mpdConnection);
	/*check if successfull*/
	if (mpd_check_connected(this->dm_mpdConnection)) {
		std::cout << "Connected to MPD." << std::endl;
		this->dm_isConnected=true;
		return true;
	}
	else{
		std::cout << "Connection to MPD failed." << std::endl;
		this->dm_isConnected=false;
		return false;
	}
}

bool MpdControl::ReconnectWithTimeout(){
	wxThread *t = GetThread();
    if(t){
        if(t->TestDestroy()){
            std::cout <<"Not trying to reconnect to mpd because user wants to exit app."<<std::endl;
            return true;
        }
        std::cout <<"Retrying with 5 seconds interval..."<<std::endl;
    	t->Sleep(5000);
    	return this->Connect();
    }
    else{
    	std::cout <<"Outch, pointer to thread object is NULL!!"<<std::endl;
    	return false;
    }
}

void MpdControl::OnStatusChanged(ChangedStatusType status){
	if(status&MPD_CST_ELAPSED_TIME){
		int elapsedTime=mpd_status_get_elapsed_song_time(this->dm_mpdConnection);
		this->p_lcdWidgets->UpdateElapsedTime(elapsedTime);
	}
	
	if(status&MPD_CST_TOTAL_TIME){
		int totalTime=mpd_status_get_total_song_time(this->dm_mpdConnection);
		this->p_lcdWidgets->UpdateTotalTime(totalTime);
	}
	
	if (status&MPD_CST_STATE){
		MpdState state = (MpdState) mpd_player_get_state(this->dm_mpdConnection);
		this->p_lcdWidgets->UpdateState(state);
	}
	if (status&MPD_CST_SONGID){
		mpd_Song* song = mpd_playlist_get_current_song(this->dm_mpdConnection);
		this->p_lcdWidgets->UpdatePlayedSong(song);
	}
	
	if (status&MPD_CST_VOLUME){
		int volume =mpd_status_get_volume(this->dm_mpdConnection);
		this->p_lcdWidgets->UpdateVolume(volume);
	}	
	
	if(status&MPD_CST_RANDOM){
		bool random = mpd_player_get_random(this->dm_mpdConnection);
		this->p_lcdWidgets->UpdateRandom(random);
	}
	
	if (status&MPD_CST_REPEAT){
		bool repeat = mpd_player_get_repeat(this->dm_mpdConnection);
		this->p_lcdWidgets->UpdateRepeat(repeat);
	}
	
	if(status&MPD_CST_CROSSFADE){
		int crossfade = mpd_status_get_crossfade(this->dm_mpdConnection);
		this->p_lcdWidgets->UpdateCrossfade(crossfade);
	}
	
	if(status&MPD_CST_PLAYLIST){
		
		MpdData * mpdData =  mpd_playlist_get_changes(this->dm_mpdConnection,-1);
		int plLen = mpd_playlist_get_playlist_length(this->dm_mpdConnection);
		this->p_lcdWidgets->UpdatePlaylist(mpdData,plLen);
		
	}
}

void MpdControl::OnConnectionChanged(int connected){
	if(!connected){
		std::cout << "\tConnection to mpd was lost."<< std::endl;
		do{}while(!this->ReconnectWithTimeout());
	}
}

void MpdControl::OnError(int id, char* msg){
	std::cout <<"\t mpd error occured: ID "<<id<<std::endl;
}

void MpdControl::HandleStatusChangedCB(MpdObj* mi,ChangedStatusType what, void* userData){
	((MpdControl*)userData)->OnStatusChanged(what);
}

void MpdControl::HandleConnectionStateChangedCB(MpdObj* mi,int connect, void* userData){
	((MpdControl*)userData)->OnConnectionChanged(connect);
}

void MpdControl::HandleErrorCB(MpdObj * mi, int id, char *msg, void* userData){
	((MpdControl*)userData)->OnError(id,msg);
}

void MpdControl::ToggleState(){
	MpdState state = (MpdState) mpd_player_get_state(this->dm_mpdConnection);
	switch(state){
		case MPD_PLAYER_PAUSE:
			this->Play();
			break;
		case MPD_PLAYER_PLAY:
			this->Pause();
			break;
		case MPD_PLAYER_STOP:
			this->Play();
			break;
		default:
			break;
	}
}

void MpdControl::Stop(){
	mpd_player_stop(this->dm_mpdConnection);
}

void MpdControl::Play(){
	mpd_player_play(this->dm_mpdConnection);
}

void MpdControl::Pause(){
	mpd_player_pause(this->dm_mpdConnection);
}

void MpdControl::NextSong(){
	mpd_player_next(this->dm_mpdConnection);
}

void MpdControl::PrevSong(){
	mpd_player_prev(this->dm_mpdConnection);
}

void MpdControl::SeekInCurrentSong(int seek){
	int elapsedTime=mpd_status_get_elapsed_song_time(this->dm_mpdConnection);
	int totalTime=mpd_status_get_total_song_time(this->dm_mpdConnection);
	int newPos=elapsedTime+seek;
	if ( elapsedTime>=0 && (newPos >=0 && newPos <=totalTime) ){
		mpd_player_seek(this->dm_mpdConnection,newPos);
	}
}

void MpdControl::ToggleRandom(){
	bool r = mpd_player_get_random(this->dm_mpdConnection);
	mpd_player_set_random(this->dm_mpdConnection,!r);
}

void MpdControl::ToggleRepeat(){
	bool r = mpd_player_get_repeat(this->dm_mpdConnection);
	mpd_player_set_repeat(this->dm_mpdConnection,!r);
}

void MpdControl::SetVolume(int diff){
	int newVol=diff+mpd_status_get_volume(this->dm_mpdConnection);
	// diff = [-100;100]   vol = [0;100]
	if( (diff>=-100 && diff<=100) && (newVol>=0 && newVol<=100) ){
		mpd_status_set_volume(this->dm_mpdConnection,newVol);
	}
}

void MpdControl::SetCrossfade(int crossfade){
	mpd_status_set_crossfade(this->dm_mpdConnection,crossfade);
}

void MpdControl::ClearPlayList(){
	mpd_playlist_clear(this->dm_mpdConnection);
}

void MpdControl::RemoveSongFromPlayList(int id){
	mpd_playlist_delete_id(this->dm_mpdConnection,id);
}

void MpdControl::PlaySongFromPlayList(int id){
	mpd_player_play_id(this->dm_mpdConnection,id);
}

void MpdControl::CropPlayListSong(int id){
	if (mpd_playlist_get_playlist_length(this->dm_mpdConnection)>1) {
		MpdData * mpdData =  mpd_playlist_get_changes(this->dm_mpdConnection,-1);
		while(mpdData!=NULL) {
			if(mpdData->song){
				if(mpdData->song->id != id){
					mpd_playlist_queue_delete_id(this->dm_mpdConnection,mpdData->song->id);
				}
			}
			mpdData = mpd_data_get_next(mpdData);
		}
		mpd_playlist_queue_commit(this->dm_mpdConnection);
	}
}
