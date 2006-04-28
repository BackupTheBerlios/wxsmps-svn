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

#ifndef LCDCOM_H_
#define LCDCOM_H_

#include <wx/wx.h>
#include <wx/socket.h>
#include <wx/thread.h>
#include <wx/tokenzr.h>
#include "messagelist.h"


class LcdComBase : public wxThreadHelper {
	friend class LcdComStatus;
	friend class LcdComMenu;
	public:
		/*public member functions*/
		LcdComBase();
		~LcdComBase();
		virtual void 		* Entry();
		void		 		Run();
		bool				IsRunning();
		/* set/get properties */
		void                SetDefaultConnectionTimeout(unsigned int s);
		void		        SetLcdHostName(const wxString& host);
		void		        GetLcdHostName(wxString& host);
		void	        	SetLcdPort(const wxString& port);
		void		        GetLcdPort(wxString& port);

		/* actions */
		bool		        ConnectToLcd();
		bool		        DisconnectFromLcd();
		void		        AddPendingMsg(const wxString& msg);
		void		        SendToLcd(const wxString& str);
		void 		        GetLcdResponce(bool hasToBeprocessed);
		void		        GetLcdResponce(wxString& str,int ms=50);
		
		/* status checks */
		bool        		IsConnected();
		bool	        	IsDisconnected();
		bool        		HasErrorOccured();
		void	        	LogLastError();
		bool		        HasLostConnection();
		/*
		class Init;
		friend struct 		LcdCom::Init;
		static LcdMsgList*	s_messageList;
		static wxMutex*			s_mutexProtectingGlobalMessageList;
		*/
	
	private:
		LcdMsgList*			dm_messageList;
		wxSocketClient		* dm_sock;
		wxIPV4address		dm_addr;
};

class LcdComStatus : public LcdComBase {
	public:
		virtual void		* Entry();
};

class LcdComMenu : public LcdComBase {
	public:
		virtual void		* Entry();
};

/*
STATIC INITIALIZER/DEINITIALIZER
class LcdCom::Init {
	public:
		Init() 	{ if (count++ == 0) {s_messageList = new LcdMsgList(); s_messageList->DeleteContents(true); s_mutexProtectingGlobalMessageList = new wxMutex();} }
		~Init()	{ if (--count == 0) {delete s_messageList; delete s_mutexProtectingGlobalMessageList;} }
	private:
		static unsigned count;
};

static LcdCom::Init lcdComInit;*/


#endif /*LCDCOM_H_*/
