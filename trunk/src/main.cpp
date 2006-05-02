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
#include <wx/wx.h>
#include <wx/app.h>
#include <wx/regex.h>
#include <time.h>
#include "lcdcom.h"
#include "main.h"

IMPLEMENT_APP(MyApp) // macro that enables ::wxGetApp()



bool MyApp::OnInit(){
	/* set ip connection*/
	this->dm_lcdComStatus = new LcdComStatus();
	wxString host = wxT("127.0.0.1");
	wxString port = wxT("13666");
	this->dm_lcdComStatus->SetLcdHostName(host);
	this->dm_lcdComStatus->SetLcdPort(port);
	/* connect*/
	this->dm_lcdComStatus->ConnectToLcd();
		
	if (dm_lcdComStatus->IsConnected()){
		std::cout << "Connection to LCDd successfully established." << std::endl;
		//try to retrieve and evaluate LCD dimensions
		this->IntroduceClient(wxT("wxsmps_player"));
		wxString tmp;
		this->dm_lcdComStatus->GetLcdResponce(tmp,2000);
		
		if (this->GetDimensionsFromLcdproc(tmp)){
			if (this->AreLcdDimensionsValide()){
				return true;
			}
			else{//LCD dimensions are not valid.
				std::cout <<"Your LCD doesn't meet the requirements, (height == 2 || height == 4) && width >=8."<<std::endl;
				return false;
			}
		}
		else{//could not retrieve LCD dimensions from lcdproc
			return false;
		}
		
		
	}
	else{
		std::cout <<"Connection to LCDd could not be established."<<std::endl;
		delete this->dm_lcdComStatus;
		return false; //application exits
	}
}

int MyApp::OnRun(){
	
	/*create another lcdCom that deals with menues*/
	this->dm_lcdComMenu = new LcdComMenu();
	wxString host = wxT("127.0.0.1");
	wxString port = wxT("13666");
	this->dm_lcdComMenu->SetLcdHostName(host);
	this->dm_lcdComMenu->SetLcdPort(port);
	this->dm_lcdComMenu->ConnectToLcd();
	this->dm_lcdComMenu->SendToLcd(wxT("hello"));
	this->dm_lcdComMenu->SendToLcd(wxT("client_set -name wxsmps_menu"));
	
	
	
	/*must be called absolutely to create an lcdwidgets object*/
	this->SetFormats();
	
	
	/*fire first lcdcom thread*/
	if(!this->dm_lcdComStatus->IsRunning()){
		this->dm_lcdComStatus->Run();
	}
	
	/*fire second lcdcom thread*/
	if(!this->dm_lcdComMenu->IsRunning()){
		this->dm_lcdComMenu->Run();
	}
			
	this->dm_lcdWidgets->InitPlayerScreen();
	this->dm_lcdWidgets->InitMainMenu();
	this->dm_lcdWidgets->InitClientKeys();
	
	
	this->dm_mpdControl = new MpdControl(this->dm_lcdWidgets);
	if(!this->dm_mpdControl->IsRunning()){
		this->dm_mpdControl->Run();
	}
    
	
	this->dm_runningMainLoop=true;
	return MainLoop();
	
}

int MyApp::MainLoop(){
		
	struct timespec test;
	test.tv_sec  = 1;
	test.tv_nsec = 0;//50000000;//100000000;//500000000;
	
	
	while(this->dm_lcdComStatus->IsRunning()&&this->dm_lcdComMenu->IsRunning()&&this->dm_runningMainLoop){
		nanosleep(&test,NULL);
	}
	
	this->dm_runningMainLoop=false;
	return 0;
}

void MyApp::ProcessMenuEvents(const wxString &str){
	//std::cout<<"In ProcessLcdResponce: " << str.mb_str(wxConvUTF8)<<"."<<std::endl;

	if(str.Contains(wxT(M_EXIT_YES))){//user wants to exit
		this->ExitMainLoop();
		return;
	}
	
    /*playlist stuff*/
	if(str.Contains(wxString::Format(wxT("menuevent select %s"),wxT(M_PL_CLEAR_YES)))){//clear the playlist
		this->dm_mpdControl->ClearPlayList();
		return;
	}
	
	if(str.Contains(wxT("menuevent select rem_pl_song_"))){//remove song from playlist request
		wxString strID= str.AfterLast('_');
		strID=strID.Trim(true);
		unsigned long ID;
		if(strID.ToULong(&ID,10)){
			this->dm_mpdControl->RemoveSongFromPlayList((int)ID);
		}
		else{
			std::cout <<"Remove Song: Could not get song ID from playlist item."<<std::endl;
		}
		return;
	}
	
	if(str.Contains(wxT("menuevent select crop_pl_song_"))){//crop song in playlist
		wxString strID= str.AfterLast('_');
		strID=strID.Trim(true);
		unsigned long ID;
		if(strID.ToULong(&ID,10)){
			this->dm_mpdControl->CropPlayListSong((int)ID);
		}
		else{
			std::cout <<"Crop Song: Could not get song ID from playlist item."<<std::endl;
		}
		return;
	}
	
	if(str.Contains(wxT("menuevent select play_pl_song_"))){//play song from playlist
		wxString strID= str.AfterLast('_');
		strID=strID.Trim(true);
		unsigned long ID;
		if(strID.ToULong(&ID,10)){
			this->dm_mpdControl->PlaySongFromPlayList((int)ID);
		}
		else{
			std::cout <<"Play Song: Could not get song ID from playlist item."<<std::endl;
		}
		return;
	}
	
	if(str.Contains(wxT("menuevent update m_set_crossfade"))){//user changed crossfade
		wxString strCf = str.AfterLast(' ');
		long crossfade;
		if(strCf.ToLong(&crossfade)){
			this->dm_mpdControl->SetCrossfade((int)crossfade);
		}
		else{
			std::cout << "Could not retrieve a crossfade integer value!!" << std::endl;
		}
		return;
	}
}

void MyApp::IntroduceClient(const wxString &strName){
	this->dm_lcdComStatus->SendToLcd(wxT("hello"));
	wxString introMsg=wxT("client_set -name ") + strName;
	this->dm_lcdComStatus->SendToLcd(introMsg);
}

bool MyApp::GetDimensionsFromLcdproc(const wxString& str){
	wxRegEx reHeight;//[\d]+(?=\shgt\s) [\d]+(?=\scellwid\s)
	wxRegEx reWidth;
	reHeight.Compile(wxT("[\\d]+(?=\\scellwid\\s)"),wxRE_ADVANCED);
	reWidth.Compile(wxT("[\\d]+(?=\\shgt\\s)"),wxRE_ADVANCED);

	if ( reHeight.Matches(str) && reWidth.Matches(str) ){
		wxString strHeight=reHeight.GetMatch(str);
		wxString strWidth=reWidth.GetMatch(str);

		long lHeight;
		long lWidth;
		if( strHeight.ToLong(&lHeight) && strWidth.ToLong(&lWidth) ){
			this->dm_h=(int)lHeight;
			this->dm_w=(int)lWidth;
			return true;
		}
		else {
			std::cout <<"ToLong() failed: Could not get LCD width and height from lcdproc."<<std::endl;
			return false;
		}
	}
	else{
		std::cout <<"Regex did not match: Could not get LCD width and height from lcdproc."<<std::endl;
		return false;
	}
}

bool MyApp::AreLcdDimensionsValide(){
	return ( (this->dm_h == 2 || this->dm_h == 4) && (this->dm_w >=8) );
}

void MyApp::SetFormats(){
	if(this->dm_h==4){
		if(this->dm_w>=20){
			this->dm_lcdWidgets =	new LcdWidgetsFourLinesWide(this->dm_lcdComStatus,this->dm_lcdComMenu,this->dm_w,this->dm_h,
									wxT("%s%c%s%c%c%c%s"),wxT("%H:%M:%S"),wxT("V%.3d%%"));
		}
		else if(this->dm_w>=16){
			this->dm_lcdWidgets = 	new LcdWidgetsFourLinesWide(this->dm_lcdComStatus,this->dm_lcdComMenu,this->dm_w,this->dm_h,
									wxT("%s%c%s%c%c%c%s"),wxT("%M:%S"),wxT("V%.3d"));
		}
		else if (this->dm_w>=8){
			this->dm_lcdWidgets =	new LcdWidgetsFourLinesSmall(this->dm_lcdComStatus,this->dm_lcdComMenu,this->dm_w,this->dm_h,
									wxT("%s%c%c%c"),wxT("%M:%S"));
		}
	}
	else if (this->dm_h==2){
		if(this->dm_w>=20){
			this->dm_lcdWidgets = new LcdWidgetsTwoLinesWide::LcdWidgetsTwoLinesWide(this->dm_lcdComStatus,this->dm_lcdComMenu,this->dm_w,this->dm_h,
									wxT("%s%c%s%c%c%c%s"),wxT("%H:%M:%S"),wxT("V%.3d%%"));
		}
		else if(this->dm_w>=16){
			this->dm_lcdWidgets = new LcdWidgetsTwoLinesWide::LcdWidgetsTwoLinesWide(this->dm_lcdComStatus,this->dm_lcdComMenu,this->dm_w,this->dm_h,
									wxT("%s%c%s%c%c%c%s"),wxT("%M:%S"),wxT("V%.3d"));
		}
		else if (this->dm_w>=8){
			this->dm_lcdWidgets =	new LcdWidgetsTwoLinesSmall(this->dm_lcdComStatus,this->dm_lcdComMenu,this->dm_w,this->dm_h,
									wxT("%s%c%c%c"),wxT("%M:%S"));
		}
	}
}


void MyApp::ProcessKeyPress(const wxString & str){
	wxString kStr=wxT("key ");   //expects "key k" k=acutal key
	if(str == (kStr + wxT(DEFAULT_KEY_STOP_STRING))){//stop request
		this->dm_mpdControl->Stop();
		return;
	}
	
	if(str == (kStr + wxT(DEFAULT_KEY_PLAY_PAUSE_STRING))){// play/pause request
		this->dm_mpdControl->ToggleState();
		return;
	}
	
	if(str == (kStr + wxT(DEFAULT_KEY_NEXT_SONG_STRING))){// skip to next song request
		this->dm_mpdControl->NextSong();
		return;
	}
	
	if(str == (kStr + wxT(DEFAULT_KEY_PREV_SONG_STRING))){ // skip to prev song request
		this->dm_mpdControl->PrevSong();
		return;
	}
	
	if(str == (kStr + wxT(DEFAULT_KEY_SEEK_FORWARD_STRING))){ // seek forward in current song request
		this->dm_mpdControl->SeekInCurrentSong(DEFAULT_SEEK);
		return;
	}
	
	if(str == (kStr + wxT(DEFAULT_KEY_SEEK_BACK_STRING))){ // seek backward in current song request
		this->dm_mpdControl->SeekInCurrentSong(DEFAULT_SEEK*-1);
		return;
	}
	
	
	if(str == (kStr + wxT(DEFAULT_KEY_VOL_UP_STRING))){ // vol up request
		this->dm_mpdControl->SetVolume(DEFAULT_VOL_DIFF);
		return;
	}
	
	if(str == (kStr + wxT(DEFAULT_KEY_VOL_DOWN_STRING))){ //vol down request
		this->dm_mpdControl->SetVolume(DEFAULT_VOL_DIFF*-1);
		return;
	}
	
	if(str == (kStr + wxT(DEFAULT_KEY_REPEAT_STRING))){ //toggle repeat request
		this->dm_mpdControl->ToggleRepeat();
		return;
	}
	
	if(str == (kStr + wxT(DEFAULT_KEY_RANDOM_STRING))){ //toggle random request
		this->dm_mpdControl->ToggleRandom();
		return;
	}
}

void MyApp::ExitMainLoop(){
	this->dm_runningMainLoop=false;
}

bool MyApp::IsMainLoopRunning(){
	return this->dm_runningMainLoop;
}

int MyApp::OnExit(){

    std::cout <<"Shutting down mpdControl thread..."<<std::endl;
	this->dm_mpdControl->GetThread()->Delete();
	std::cout <<"Done."<<std::endl;
	delete this->dm_mpdControl;
    std::cout <<"Shutting down lcdCom Menu thread..."<<std::endl;
    this->dm_lcdComMenu->GetThread()->Delete();
	std::cout <<"Done."<<std::endl;	
	delete this->dm_lcdComMenu;
	std::cout <<"Shutting down lcdCom Status thread..."<<std::endl;
    this->dm_lcdComStatus->GetThread()->Delete();
	std::cout <<"Done."<<std::endl;
	delete this->dm_lcdComStatus;	
    std::cout <<"Goodbye."<<std::endl;
	return this->wxAppConsole::OnExit();
    
}
