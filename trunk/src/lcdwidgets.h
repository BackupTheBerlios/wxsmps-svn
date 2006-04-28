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

#ifndef LCDWIDGETS_H_
#define LCDWIDGETS_H_
#include "lcdcom.h"
#include "contrib/libmpd/libmpd.h"
// chars lcdproc doesn't like in it's widgets syntax are: ´ "

#define DEFAULT_SCROLLER_SPEED				4
#define DEFAULT_VOL_DIFF					1
#define DEFAULT_SEEK						2

/*Player Screen*/
#define SCREEN_PLAYER						"scr_player"
#define SCREEN_PLAYER_SCROLLER_ARTIST		"scr_player_scroller_artist"
#define SCREEN_PLAYER_SCROLLER_TITLE		"scr_player_scroller_title"
#define SCREEN_PLAYER_SCROLLER_ARTIST_TITLE	"scr_player_scroller_artist_title"
#define SCREEN_PLAYER_SCROLLER_ALBUM		"scr_player_scroller_album"
#define SCREEN_PLAYER_STRING_STATELINE		"scr_player_string_stateline"


#define DEFAULT_ARTIST_STRING				" Artist "
#define DEFAULT_TITLE_STRING				" Title "
#define DEFAULT_ALBUM_STRING				" Album "
#define DEFAULT_ARTIST_TITLE_STRING			" Artist - Title "

#define DEFAULT_UNKOWN_CHAR					'?'
#define DEFAULT_RANDOM_ON_CHAR				'~'
#define DEFAULT_RANDOM_OFF_CHAR				'-'
#define DEFAULT_REPEAT_ON_CHAR				']'
#define DEFAULT_REPEAT_OFF_CHAR				'|'
#define DEFAULT_STATE_PLAYING_CHAR			'>'
#define DEFAULT_STATE_PAUSED_CHAR			'p'
#define DEFAULT_STATE_STOPPED_CHAR			's'

/*Main Menu*/
#define M_M_PLAYLIST						"m_m_playlist"
#define M_M_MEDIA							"m_m_media"
#define M_M_EXIT							"m_m_exit"
#define M_M_SETTINGS						"m_m_settings"

#define M_M_PLAYLIST_LABEL					"Playlist..."
#define M_M_MEDIA_LABEL						"Media..."
#define M_M_EXIT_LABEL						"Exit Program?"
#define M_M_SETTINGS_LABEL					"Settings..."

/*Exit menu*/
#define M_EXIT_YES							"m_m_exit_yes"
#define M_EXIT_NO							"m_m_exit_no"

#define M_EXIT_YES_LABEL					"yes"
#define M_EXIT_NO_LABEL						"no"


/*Settings menu*/
/*numeric crossfade*/
#define M_SET_CROSSFADE						"m_set_crossfade"
#define M_SET_CROSSFADE_LABEL				"Set crossfade..."
#define DEFAUT_CROSSFADE_INT				0

/*Playlist menu*/
#define M_PL_BROWSE							"m_pl_browse"
#define M_PL_CLEAR							"m_pl_clear"
#define M_PL_GOTO_CURRENT_SONG				"m_pl_goto_current"

#define M_PL_BROWSE_LABEL					"Browse..."
#define M_PL_CLEAR_LABEL					"Clear?"
#define M_PL_GOTO_CURRENT_SONG_LABEL		"Jump to cur. song"

/*Playlist clear menu*/
#define M_PL_CLEAR_YES						"m_pl_clear_yes"
#define M_PL_CLEAR_NO						"m_pl_clear_no"

#define M_PL_CLEAR_YES_LABEL				"yes"
#define M_PL_CLEAR_NO_LABEL					"no"



/*client keys*/
#define DEFAULT_KEY_STOP_STRING				"0"
#define DEFAULT_KEY_REPEAT_STRING			"1"
#define DEFAULT_KEY_PREV_SONG_STRING		"2"
#define DEFAULT_KEY_RANDOM_STRING			"3"
#define DEFAULT_KEY_SEEK_BACK_STRING		"4"
#define DEFAULT_KEY_PLAY_PAUSE_STRING		"5"
#define DEFAULT_KEY_SEEK_FORWARD_STRING		"6"
#define DEFAULT_KEY_NEXT_SONG_STRING		"8"
#define DEFAULT_KEY_VOL_UP_STRING			"+"
#define DEFAULT_KEY_VOL_DOWN_STRING			"*"   //lcdproc bug: does not accept "-" as client key.
#define DEFAULT_KEY_CLEAR_PLAYLIST_STRING	"."


const wxChar spinnerCharArray[4] = {16,20,17,20};


class LcdWidgetsBase {
    friend class LcdWidgetsFourLines;
    friend class LcdWidgetsTwoLines;
    friend class LcdWidgetsWide;
    friend class LcdWidgetsSmall;
    public:
        LcdWidgetsBase(LcdComStatus* _lcdComS,LcdComMenu* _lcdComM,int _w,int _h,const wxString & _plyStateStrF,const wxString & _eTimeStrF)
        {p_lcdComStatus=_lcdComS;p_lcdComMenu=_lcdComM;dm_lcdWidth=_w;dm_lcdHeight=_h;dm_playerStateStrFormat=_plyStateStrF;
        dm_elapsedTimeStrFormat=_eTimeStrF;dm_elapsedTime=0;dm_totalTime=0;dm_charState=DEFAULT_UNKOWN_CHAR;dm_charRepeat=DEFAULT_UNKOWN_CHAR;dm_charRandom=DEFAULT_UNKOWN_CHAR;}
        virtual			~LcdWidgetsBase(){}
        void            UpdateState(MpdState _state);
        void	 		UpdateElapsedTime(int _elapsedTime);
    	void			UpdateTotalTime(int _totalTime);
    	void 			UpdateRandom(bool _random);
    	void 			UpdateRepeat(bool _repeat);
    	void			UpdateCrossfade(int _crossfade);
        void			UpdatePlaylist(MpdData * _mpdData,int plLen);
        
        void 			InitMainMenu();
	    void			InitClientKeys();
		virtual void	UpdateVolume(int);
        virtual void	InitPlayerScreen();
		virtual void	UpdatePlayerStateLine();
		virtual void	UpdatePlayedSong(mpd_Song *);
    private:
        void            UpdateStateChar();
        void			UpdateCurSongID(int id);
        void            UpdateElapsedTimeStr();
        void			UpdateTotalTimeStr();
        void            UpdateRandomChar();
        void            UpdateRepeatChar();
        void			ExtractFileNameFromSongPath(mpd_Song *,wxString & fileName);
        void			EscapeChars(wxString &str);
        
        LcdComStatus	* p_lcdComStatus;
        LcdComMenu		* p_lcdComMenu;
        MpdState        dm_state;
	    int				dm_lcdWidth;
	    int				dm_lcdHeight;
	    bool			dm_repeat;
	    bool			dm_random;
	    int				dm_elapsedTime;
	    int				dm_totalTime;
	    int				dm_curSongID;
	    int				dm_crossfade;
	
	    wxChar			dm_charState;
		wxChar			dm_charRepeat;
		wxChar			dm_charRandom;        
        wxString		dm_elapsedTimeStr;
        wxString		dm_totalTimeStr;
        wxString		dm_playerStateStrFormat;
        wxString        dm_elapsedTimeStrFormat;

};
//inline LcdWidgetsBase::~LcdWidgetsBase(){}

class LcdWidgetsFourLines :  public virtual LcdWidgetsBase{
	public:
		LcdWidgetsFourLines(LcdComStatus* _lcdComS,LcdComMenu* _lcdComM,int _w,int _h,const wxString & _plyStateStrF,const wxString & _eTimeStrF)
		: LcdWidgetsBase(_lcdComS,_lcdComM,_w,_h,_plyStateStrF,_eTimeStrF)
		{}
		
		virtual void    InitPlayerScreen();
        virtual void    UpdatePlayedSong(mpd_Song * song);
};

class LcdWidgetsTwoLines :  public virtual LcdWidgetsBase{
	public:	
		LcdWidgetsTwoLines(LcdComStatus* _lcdComS,LcdComMenu* _lcdComM,int _w,int _h,const wxString & _plyStateStrF,const wxString & _eTimeStrF)
		: LcdWidgetsBase(_lcdComS,_lcdComM,_w,_h,_plyStateStrF,_eTimeStrF)
		{}
		
		virtual void    InitPlayerScreen();
        virtual void    UpdatePlayedSong(mpd_Song * song);
};

class LcdWidgetsWide :  public virtual LcdWidgetsBase{
    public:
        LcdWidgetsWide(LcdComStatus* _lcdComS,LcdComMenu* _lcdComM,int _w,int _h,const wxString & _plyStateStrF,const wxString & _eTimeStrF,const wxString & _volStrF)
        : LcdWidgetsBase(_lcdComS,_lcdComM,_w,_h,_plyStateStrF,_eTimeStrF)
        {dm_volumeStrFormat=_volStrF;}
        
        void			UpdatePercTotalTimeStr();
        virtual void	UpdatePlayerStateLine();
        virtual void 	UpdateVolume(int _volume);
    private:
        void            UpdateVolumeStr();
        void			UpdateSpinnerChar();
        int				dm_volume;
        int				dm_spinner;
        wxString		dm_percTotalTimeStr;
        wxString		dm_volumeStr;
        wxString        dm_volumeStrFormat;
        
};

class LcdWidgetsSmall : public virtual LcdWidgetsBase {
	public:
		LcdWidgetsSmall(LcdComStatus* _lcdComS,LcdComMenu* _lcdComM,int _w,int _h,const wxString & _plyStateStrF,const wxString & _eTimeStrF)
		:LcdWidgetsBase(_lcdComS,_lcdComM,_w,_h,_plyStateStrF,_eTimeStrF)
		{}
		
		virtual void	UpdatePlayerStateLine();
};



class LcdWidgetsFourLinesWide: public LcdWidgetsFourLines , public LcdWidgetsWide {
	public:
		LcdWidgetsFourLinesWide(LcdComStatus* _lcdComS,LcdComMenu* _lcdComM,int _w,int _h,const wxString & _plyStateStrF,const wxString & _eTimeStrF,const wxString & _volStrF)
		:LcdWidgetsBase(_lcdComS,_lcdComM,_w,_h,_plyStateStrF,_eTimeStrF),LcdWidgetsFourLines(_lcdComS,_lcdComM,_w,_h,_plyStateStrF,_eTimeStrF),
		LcdWidgetsWide(_lcdComS,_lcdComM,_w,_h,_plyStateStrF,_eTimeStrF,_volStrF)
		{}
};

class LcdWidgetsTwoLinesWide: public LcdWidgetsTwoLines, public LcdWidgetsWide {
	public:
		LcdWidgetsTwoLinesWide(LcdComStatus* _lcdComS,LcdComMenu* _lcdComM,int _w,int _h,const wxString & _plyStateStrF,const wxString & _eTimeStrF,const wxString & _volStrF)
		:LcdWidgetsBase(_lcdComS,_lcdComM,_w,_h,_plyStateStrF,_eTimeStrF),LcdWidgetsTwoLines(_lcdComS,_lcdComM,_w,_h,_plyStateStrF,_eTimeStrF), 
		LcdWidgetsWide(_lcdComS,_lcdComM,_w,_h,_plyStateStrF,_eTimeStrF,_volStrF)
		{}
};


class LcdWidgetsFourLinesSmall: public LcdWidgetsFourLines , public LcdWidgetsSmall {
	public:
		LcdWidgetsFourLinesSmall(LcdComStatus* _lcdComS,LcdComMenu* _lcdComM,int _w,int _h,const wxString & _plyStateStrF,const wxString & _eTimeStrF)
		:LcdWidgetsBase(_lcdComS,_lcdComM,_w,_h,_plyStateStrF,_eTimeStrF),LcdWidgetsFourLines(_lcdComS,_lcdComM,_w,_h,_plyStateStrF,_eTimeStrF),
		LcdWidgetsSmall(_lcdComS,_lcdComM,_w,_h,_plyStateStrF,_eTimeStrF)
		{}
};

class LcdWidgetsTwoLinesSmall: public LcdWidgetsTwoLines , public LcdWidgetsSmall {
	public:
		LcdWidgetsTwoLinesSmall(LcdComStatus* _lcdComS,LcdComMenu* _lcdComM,int _w,int _h,const wxString & _plyStateStrF,const wxString & _eTimeStrF)
		:LcdWidgetsBase(_lcdComS,_lcdComM,_w,_h,_plyStateStrF,_eTimeStrF),LcdWidgetsTwoLines(_lcdComS,_lcdComM,_w,_h,_plyStateStrF,_eTimeStrF),
		LcdWidgetsSmall(_lcdComS,_lcdComM,_w,_h,_plyStateStrF,_eTimeStrF)
		{}
};

#endif /*LCDWIDGETS_H_*/
