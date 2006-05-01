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
#include "lcdcom.h"
#include "lcdwidgets.h"
#include "main.h"

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(LcdMsgList)


/***************************************************
 * **********LcdComBase Class***********************
 * *************************************************/

LcdComBase::LcdComBase() 
: wxThreadHelper()
{
	/* create instance of sock object*/
	this->dm_sock = new wxSocketClient(wxSOCKET_NONE);
	this->dm_messageList = new LcdMsgList();
	//this->dm_sock->SetTimeout(0);
}

LcdComBase::~LcdComBase() {
	this->dm_sock->Destroy(); //Destroy() calls Close() automatically
	this->dm_sock=NULL;
	this->dm_messageList->clear();
	delete this->dm_messageList;
}

void LcdComBase::Run(){
	if( IsRunning() ) return;                   // we are already doing something!

    wxThreadHelper::Create();                   // create thread
    GetThread()->Run();                         // run.....
}

void *LcdComBase::Entry(){}

bool LcdComBase::IsRunning(){
	
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

void LcdComBase::final(){
    this->dm_sock->Close();
}

void LcdComBase::SetLcdHostName(const wxString& host){
	this->dm_addr.Hostname(host);
}

void LcdComBase::GetLcdHostName(wxString& host){
	host=this->dm_addr.Hostname();
}

void LcdComBase::SetLcdPort(const wxString & port){
	this->dm_addr.Service(port);
}

void LcdComBase::GetLcdPort(wxString & port){
	int p = this->dm_addr.Service();
	port= wxString::Format(wxT("%d"),p);
}

void LcdComBase::SetDefaultConnectionTimeout(unsigned int s){
	this->dm_sock->SetTimeout(s);
}

bool LcdComBase::ConnectToLcd(){
	if(!this->IsConnected()){
		return this->dm_sock->Connect(this->dm_addr,true);
	}
	else{//is already connected
		std::cout << "The connection is already established." << std::endl;
		return false;
	}
}

bool LcdComBase::DisconnectFromLcd(){
	if(this->IsConnected()){
		this->dm_sock->Close();
		return true;
	}
	else{//there was no connection
		std::cout <<"The connection was not established." << std::endl;
		return false;
	}
}

void LcdComBase::AddPendingMsg(const wxString &msg){
	LcdMessage* m = new LcdMessage();
	m->msg=msg;
	this->dm_messageList->Append(m);
	//LcdCom::s_messageList->Append(m);
}


void LcdComBase::SendToLcd(const wxString& str){
	if(!this->HasErrorOccured()){ //checks for error and logs it
		wxString sendStr = str + wxT("\n");
		//this->dm_sock->WaitForWrite(0,300);
		//std::cout << "About to send this to lcd: " << sendStr.mb_str(wxConvUTF8) <<std::endl;
		this->dm_sock->Write(sendStr.mb_str(wxConvISO8859_1),sendStr.Length());
	}
}

void LcdComBase::GetLcdResponce(bool hasToBeProcessed){
	if(!this->HasErrorOccured()){ //checks for error and logs it
		int actualRecv = 0;
		char buffer[8192];
		wxString temp;
		//this->dm_sock->WaitForRead(0,100);
		this->dm_sock->Read(buffer,8192);
		actualRecv = this->dm_sock->LastCount();
		buffer[actualRecv] = '\0';
		temp = wxString::FromAscii(buffer);
		wxStringTokenizer strTok(temp,wxT("\n"));
		while (strTok.HasMoreTokens()){
			wxString token=strTok.GetNextToken();
			//std::cout << "Proccessing LCDProc said: " <<token.mb_str(wxConvUTF8) << std::endl;
			if (hasToBeProcessed && token.Contains(wxT("key"))){
				::wxGetApp().ProcessKeyPress(token);
			}
		}
		
	}
}


void LcdComBase::GetLcdResponce(wxString & str,int ms){
	
	if(!this->HasErrorOccured()){ //checks for error and logs it
		int actualRecv = 0;
		char buffer[8192];
		wxString temp; 
		this->dm_sock->WaitForRead(0,ms);
		this->dm_sock->Read(buffer,8192);
		actualRecv = this->dm_sock->LastCount();
		buffer[actualRecv] = '\0';
		temp = wxString::FromAscii(buffer);
		//std::cout << "Not Processing LCDProc said: " <<temp.mb_str(wxConvUTF8) << std::endl;	
		str = temp;
	}
	
}

bool LcdComBase::IsConnected(){
	return this->dm_sock->IsConnected();
}

bool LcdComBase::IsDisconnected(){
	return this->dm_sock->IsDisconnected();
}
bool LcdComBase::HasErrorOccured(){
	if (this->dm_sock->Error()&&this->dm_sock->LastError()!=wxSOCKET_WOULDBLOCK&&this->dm_sock->LastError()!=wxSOCKET_NOERROR){
		this->LogLastError();
		return true;
	}
	else{
		return false;
	}
}

void LcdComBase::LogLastError(){
	wxString error;
	int sockError = (int) this->dm_sock->LastError();
	switch(sockError){
		case wxSOCKET_INVOP:
			error=wxT("Invalid operation.");
			break;
		case wxSOCKET_IOERR:
			error=wxT("Input/Output error.");
			break;
		case wxSOCKET_INVADDR:
			error=wxT("Invalid address.");
			break;
		case wxSOCKET_INVSOCK:
			error=wxT("Invalid socket.");
			break;
		case wxSOCKET_NOHOST:
			error=wxT("No corresponding host.");
			break;
		case wxSOCKET_INVPORT:
			error=wxT("Invalid port.");
			break;
		case wxSOCKET_WOULDBLOCK:
			error=wxT("Operation would block.");
			break;
		case wxSOCKET_TIMEDOUT:
			error=wxT("The timeout expired.");
			break;
		case wxSOCKET_MEMERR:
			error=wxT("Memory exhausted");
			break;
		case wxSOCKET_NOERROR:
			error=wxT("No error happened.");
			break;
		default:
			error=wxT("Unknown error.");
			break;
	}
	std::cout << "LCD Socket Error: " << error.mb_str() << std::endl;
}

bool LcdComBase::HasLostConnection(){
	return this->dm_sock->WaitForLost(5,0);
}

/*Initialization of static stuff
unsigned 				LcdCom::Init::count=0;
LcdMsgList* 			LcdCom::s_messageList = NULL;
wxMutex*				LcdCom::s_mutexProtectingGlobalMessageList = NULL;
*/




/***************************************************
 * **********LcdComStatus Class*********************
 * *************************************************/
void *LcdComStatus::Entry(){
	while(!GetThread()->TestDestroy()&&!this->HasErrorOccured()){
		
		wxString responce;
		this->GetLcdResponce(responce,0);
		wxStringTokenizer tokenizer(responce,wxT("\n"));
		if(tokenizer.CountTokens()>0){
			while(tokenizer.HasMoreTokens()){
				wxString toProcess = tokenizer.GetNextToken();
				if(toProcess.Contains(wxT("key"))){
						::wxGetApp().ProcessKeyPress(toProcess);
				}
			}
		}
		else{
			if(responce.Contains(wxT("key"))){
				::wxGetApp().ProcessKeyPress(responce);
			}
				
		}
		this->SendToLcd(wxT("hello"));
		//this->GetLcdResponce(true);
        GetThread()->Sleep(1);
	}
    this->final();
	return 0;
}

/***************************************************
 * **********LcdComMenu Class***********************
 * *************************************************/
void *LcdComMenu::Entry(){
while(!GetThread()->TestDestroy()&&!this->HasErrorOccured()){
		
		LcdMsgList::Node* node = this->dm_messageList->GetFirst();
		while(node){
			LcdMessage* msg = node->GetData();
			this->SendToLcd(msg->msg);
			
			wxString responce;
			this->GetLcdResponce(responce,0);
			wxStringTokenizer tokenizer(responce,wxT("\n"));
			if(tokenizer.CountTokens()>0){
				while(tokenizer.HasMoreTokens()){
					wxString toProcess = tokenizer.GetNextToken();
					if(toProcess.Contains(wxT("menuevent"))){
						::wxGetApp().ProcessMenuEvents(toProcess);
					}
				}
			}
			else{
				if(responce.Contains(wxT("menuevent"))){
					::wxGetApp().ProcessMenuEvents(responce);
				}
				
			}
			
			LcdMsgList::Node* oldNode = node;
			this->dm_messageList->DeleteNode(oldNode);
			node = node->GetNext();
		}
		
		this->SendToLcd(wxT("hello"));
		wxString responce;
		this->GetLcdResponce(responce,0);
		wxStringTokenizer tokenizer(responce,wxT("\n"));
		if(tokenizer.CountTokens()>0){
			while(tokenizer.HasMoreTokens()){
				wxString toProcess = tokenizer.GetNextToken();
				if(toProcess.Contains(wxT("menuevent"))){
					::wxGetApp().ProcessMenuEvents(toProcess);
				}
			}
		}
		else{
			::wxGetApp().ProcessMenuEvents(responce);
		}
		
		GetThread()->Sleep(1);
	}
    this->final();
	return 0;
	/*
	while(true){
		GetThread()->Sleep(100);
	}
	*/
}

