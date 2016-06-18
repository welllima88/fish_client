#ifndef _FISH_DEF_H
#define _FISH_DEF_H

#define PROG_VERSION					_T("1.0.0.7")

const static GUID NULL_GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};

//#define FISH_TITLE						_T("FISH1.0")

//////////////////////////////////////////////////////////////////////
// UNICODE HEADER
//////////////////////////////////////////////////////////////////////
const static byte utf8header[3] = {'\xEF', '\xBB', '\xBF'};
const static byte utf16header[2] = {'\xff', '\xfe'};

//////////////////////////////////////////////////////////////////////
// IRCD PRIV PACKET HEADER and COMMAND
//////////////////////////////////////////////////////////////////////
#define IPPH_FISH						"FISH"
#define IPPH_NFISH						"NFSH"

#define IPCMD_NR						"NR"
#define IPCMD_NU						"NU"

//////////////////////////////////////////////////////////////////////
// ERROR CODE
//////////////////////////////////////////////////////////////////////

#define	ERR_NONE						0
#define	ERR_LOGINIGNORED				100
#define	ERR_LOGINFAILED					101
#define	ERR_LOGINRETRY					102
#define	ERR_WB_UNKNOWN					200
#define	ERR_WB_HOSTNOTFOUND				201
#define	ERR_WB_WRONGRESULT				202
#define	ERR_WB_WRONGPASS				203
#define	ERR_WB_IDNOTEXIST				204
#define	ERR_WB_DBCONN					205
#define	ERR_WB_LOGINPARAMETER			206
#define	ERR_I_SOCKCREATE				300
#define	ERR_I_SOCKCONNECT				301
#define	ERR_I_LOGINTRYOVER				302			

// HTML Making Thread 사용
//#define USE_THREADHTMLMAKER
#define GNU_DEFAULT_URL					_T("#")

#define SECOND							1000
#define MINUTE							60 * SECOND
#define HOUR							60 * MINUTE

#define MIN_SEARCHLENGTH_ENG			3			// 최소 검색어 길이, 영문자.
#define MIN_SEARCHLENGTH_ETC			(MIN_SEARCHLENGTH_ENG+1) / 2			// 최소 검색어 길이, 영문자.

//////////////////////////////////////////////////////////////////////
// For Web DB Use
//////////////////////////////////////////////////////////////////////

#define REGISTER_PAGE					_T("http://3fishes.co.kr/fisher/")

#define	HOST_NAME						_T("3fishes.co.kr")
#define URL_WEBDB						_T("cfish/")
#define DELIM_NODE						_T("^;")

//////////////////////////////////////////////////////////////////////
// FOR IRC
//////////////////////////////////////////////////////////////////////
#define IRCD_HOSTNAME					_T("3fishes.co.kr")
#define IRCD_PORT						6500
#define IRCID_PREFIX					IRCID_PREFIX_KOR
#define IRCID_PREFIX_KOR				"k"
//////////////////////////////////////////////////////////////////////
// LOCAL PROPERTIES
//////////////////////////////////////////////////////////////////////
#define DEFAULT_USERID					_T("@FISH")
#define DEFAULT_PPP						2
#define DEFAULT_DOWNLOADPATH			_T("download")
#define DEFAULT_USERPATH				_T("Data\\")
#define DEFAULT_SAVEPATH				_T("savedata\\")
#define DEFAULT_TEMPPATH				_T("tempdata\\")
#define DEFAULT_POSTPATH				_T("postdata\\")
#define DEFAULT_APPLICATION_FONT        _T("Tahoma")
#define DEFAULT_STYLEPATH				_T("Style\\")
#define DEFAULT_STYLEIMGPATH			DEFAULT_STYLEPATH _T("Images\\")
#define DEFAULT_STYLEFORMAT				_T("*.xslt")

#define STR_USER_PATH_SET				_T("사용자 폴더 설정")

//////////////////////////////////////////////////////////////////////
// DEFAULT ATTENTION WEIGHT
//////////////////////////////////////////////////////////////////////

#define WEIGHT_READ_Q					1
#define WEIGHT_LINK_CLICK_Q				2
#define WEIGHT_SCRAP_Q					4
#define WEIGHT_FILTER_Q					0.5
#define WEIGHT_HIT_Q					0.5
#define ATTENTION_BASE_VALUE			100

//////////////////////////////////////////////////////////////////////
// MAX DEFINITION
//////////////////////////////////////////////////////////////////////
#define	MAX_BUFF						1024
#define	MAX_SMALLBUFF					512
#define	MAX_HUGEBUFF					4096
#define MAX_VERYHUGEBUFF                8192

//////////////////////////////////////////////////////////////////////
// REGISTRY STRING
//////////////////////////////////////////////////////////////////////

#define	REG_STR_KEYPATH					_T("Software\\OnNet\\FISH")

#define REG_STR_MFCX					_T("cx")
#define REG_STR_MFCY					_T("cy")

// for OPML Sync
#define REG_STR_SAVEPASSWORD			_T("SavePassword")
#define REG_STR_AUTOLOGIN				_T("AutoLogin")
#define REG_STR_LASTUSER				_T("LastUser")
#define REG_STR_ENCODEDPASS				_T("EncodedPass")
#define REG_STR_LASTOPMLTIME			_T("LastOPMLTime")
#define REG_STR_USEOPMLSYNC				_T("UseOPMLSync")
#define REG_STR_NOTUSEP2P				_T("NotUseP2P")
#define REG_STR_SRL						_T("SRL")
#define REG_STR_POSTAUTODELETE			_T("PostAutoDelete")

#define REG_STR_FIRSTSTART				_T("First")
#define REG_STR_AUTOSTART				_T("AutoStart")
#define REG_STR_CLOSEBTNEXIT			_T("CloseButtonExit")
#define REG_STR_SEARCHSAVE				_T("SearchSave")
#define REG_STR_IRCID					_T("IRCID")

#define REG_STR_DOWNLOADPATH			_T("DownloadPath")
#define REG_STR_USERPATH				_T("UserPath")
#define REG_STR_PPP						_T("ppp")
#define REG_STR_USEINFORMWND			_T("UseInformWnd")

#define REG_STR_MAXDOWNLOADS			_T("MaxDownloads")
#define REG_STR_MAXUPLOADS				_T("MaxUploads")
#define REG_STR_BANDWIDTHUP				_T("BandwidthUp")
#define REG_STR_BANDWIDTHDOWN			_T("BandwidthDown")
#define REG_STR_GNUHOSTPORT				_T("Gnuhostport")

#define REG_STR_SWREAD					_T("SWRead")
#define REG_STR_SWLINK					_T("SWLink")
#define REG_STR_SWHIT					_T("SWHit")
#define REG_STR_SWFILTER				_T("SWFilter")
#define REG_STR_SWSCRAP					_T("SWScrap")
#define REG_STR_SWBASE					_T("SWBase")
#define REG_STR_SWREADLIMIT				_T("SWReadLimit")
#define REG_STR_SWLINKLIMIT				_T("SWLinkLimit")
#define REG_STR_SWHITLIMIT				_T("SWHitLimit")
#define REG_STR_SWSCRAPLIMIT			_T("SWScrapLimit")
#define REG_STR_SWFILTERLIMIT			_T("SWFilterLimit")

#define REG_STR_STYLE					_T("Style")

#define REG_STR_LISTDESCSUMMARY			_T("ListDescSummary")
#define REG_STR_PAPERSTYLE				_T("PaperStyle")

#define REG_STR_LASTSEQ					_T("LastPostSeq")

#define REG_STR_WINAUTORUN				_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run")

//////////////////////////////////////////////////////////////////////
// FISH COLOR DEFINITION
//////////////////////////////////////////////////////////////////////

#define CLR_WHITE						RGB(255, 255, 255)
#define CLR_RED							RGB(255, 0, 0)
#define CLR_GREEN						RGB(0, 255, 0)
#define CLR_BLUE						RGB(0, 0, 255)
#define CLR_YELLOW						RGB(255, 255, 0)
#define CLR_BLACK						RGB(0, 0, 0)

#define CLR_LH_TEXT						CLR_BLACK

#define CLR_LH_BG						RGB(249, 250, 253)
#define CLR_LH_VLINE					RGB(181, 182, 200)
#define CLR_LH_HLINE1					RGB(230, 231, 239)
#define CLR_LH_HLINE2					RGB(209, 210, 222)
#define CLR_LH_HLINE3					RGB(189, 190, 206)

//////////////////////////////////////////////////////////////////////
// FISH APP DEFINITION
//////////////////////////////////////////////////////////////////////
#define FISH_GUID						_T("FISH10-{7444E45F-F960-4c7c-A8A6-A9D77B71ABE7}")
#define OP_FISHDATA						63113
#define FISHDATA_DIV					_T("\x1b")

//////////////////////////////////////////////////////////////////////
// POST UPDATE FLAG DEFINITION
//////////////////////////////////////////////////////////////////////
#define FISH_ITEM_ALL					0x0000ffff

#define FISH_ITEM_READ					0x00000001
#define FISH_ITEM_READON				0x00000002
#define FISH_ITEM_XMLUPDATE				0x00000004
#define FISH_ITEM_URL					0x00000008
#define FISH_ITEM_GUID					0x00000010
#define FISH_ITEM_AUTHOR				0x00000020
#define FISH_ITEM_CATEGORY				0x00000040
#define FISH_ITEM_SUBJECT				0x00000080
#define FISH_ITEM_DESCRIPTION			0x00000100
#define FISH_ITEM_ENCLOSURE				0x00000200
#define FISH_ITEM_PUBDATE				0x00000400
#define FISH_ITEM_SEARCHVALUE			0x00000800
#define FISH_ITEM_ENCODED				0x00001000
#define FISH_ITEM_SEARCHTYPE			0x00002000


//#define FISH_ITEM_TAG					0x00010000
//#define FISH_TAG_ADD					0X00020000
//#define FISH_ITEM_TAG_ADD				FISH_ITEM_TAG | FISH_TAG_ADD
//#define FISH_ITEM_NODE					0x00040000
//#define FISH_NODE_ADD					0x00080000
//#define FISH_ITEM_NODE_ADD				FISH_ITEM_NODE | FISH_NODE_ADD
//#define FISH_ADD_LIST					FISH_TAG_ADD | FISH_NODE_ADD

#define FISH_ITEM_URL_NAVIGATE			0x00100000
#define FISH_ITEM_INC_COUNT				0x00200000

//////////////////////////////////////////////////////////////////////
// POST GET FLAG DEFINITION
//////////////////////////////////////////////////////////////////////
#define FISH_GET_DEFAULT				0x0000000f

#define FISH_GET_FLAG					0x00000001
#define	FISH_GET_STRING					0x00000002
//#define FISH_GET_NODE					0x01000000
//#define FISH_GET_TAG					0x02000000
#define FISH_GET_READONLY				0x04000000
#define FISH_GET_UNREADONLY				0x08000000


#define FISH_USEOLDDATA					0x10000000


// 기본적으로 데이터를 요청할 때 Tag와 Node 정보는 요청하지 않는다.
#define FISH_GET_ALL					0x00ffffff

//////////////////////////////////////////////////////////////////////
// CHANNEL UPDATE FLAG DEFINITION
//////////////////////////////////////////////////////////////////////
#define	FISH_CHANNEL_ALL				0xffffffff
#define FISH_CHANNEL_TERMFROM			0x00000001
#define	FISH_CHANNEL_TERMTO				0x00000002
#define FISH_CHANNEL_TYPE				0x00000004
#define FISH_CHANNEL_LOAD				0x00000008

//////////////////////////////////////////////////////////////////////
// CHANNEL CREATE FLAG DEFINITION
//////////////////////////////////////////////////////////////////////
#define CHANNEL_CREATE_BNS				0x00000001
#define CHANNEL_CREATE_SEARCHLOG		0x00000002

//////////////////////////////////////////////////////////////////////
// MAIN FRAME MESSAGE ID
//////////////////////////////////////////////////////////////////////

#define WM_MAINFRAME					WM_APP + 100
#define WM_INITVIEW						WM_MAINFRAME + 1

//////////////////////////////////////////////////////////////////////
// LOGIN MESSAGE ID
//////////////////////////////////////////////////////////////////////

#define WM_LOGINCOMPLETE				WM_APP + 110
#define WM_REQUESTLOGIN					WM_APP + 111
#define WM_LOGINRESULT					WM_APP + 112
#define WM_IRCDAUTHCOMPLETE				WM_APP + 113
#define WM_IRCDCLOSED					WM_APP + 114

//////////////////////////////////////////////////////////////////////
// RSS DB MESSAGE ID
//////////////////////////////////////////////////////////////////////

#define WM_RSSDBRESULT					WM_APP + 200
#define WM_POSTINITALL					WM_APP + 201

#define WM_NAVIGATE_URL					WM_APP + 202

//////////////////////////////////////////////////////////////////////
// SUBSCRIBE VIEW MESSAGE ID
//////////////////////////////////////////////////////////////////////

#define WM_SUBSCRIBE_VIEW				WM_APP + 210
#define WM_CHANNELUCNT_UPDATE			WM_SUBSCRIBE_VIEW + 1
#define WM_INITSUBSCRIBE				WM_SUBSCRIBE_VIEW + 2
#define WM_AFTERCHECKURL				WM_SUBSCRIBE_VIEW + 3
#define WM_CHANNELSTATECHANGE			WM_SUBSCRIBE_VIEW + 4
#define WM_CHANNELPCNT_UPDATE			WM_SUBSCRIBE_VIEW + 5
#define WM_UPDATEOPMLCOMPLETE			WM_SUBSCRIBE_VIEW + 6
#define WM_SUBSCRIBE_URL                WM_SUBSCRIBE_VIEW + 7
#define WM_SETSEARCHSAVE				WM_SUBSCRIBE_VIEW + 8
#define WM_HTMLLOADCOMPLETE				WM_SUBSCRIBE_VIEW + 9

//////////////////////////////////////////////////////////////////////
// FEEDER MESSAGE ID
//////////////////////////////////////////////////////////////////////

#define WM_FEEDCOMPLETE					WM_APP + 230

//////////////////////////////////////////////////////////////////////
// IE Control VIEW MESSAGE ID
//////////////////////////////////////////////////////////////////////
#define WM_VIEW_CURRENTLIST				WM_APP + 244
#define WM_VIEW_FIRSTPOST				WM_APP + 245
#define WM_VIEW_LASTPOST				WM_APP + 246
#define WM_VIEW_NEXTPOSTS				WM_APP + 247
#define WM_VIEW_PREVPOSTS				WM_APP + 248
#define WM_VIEW_HTML					WM_APP + 249
#define WM_VIEW_REFRESH					WM_APP + 250

//////////////////////////////////////////////////////////////////////
// RSS URL PARSING
//////////////////////////////////////////////////////////////////////
#define WM_INFORM_TRAVELCNT				WM_APP + 300
#define WM_INFORM_TRAVELEND				WM_APP + 301
#define WM_INFROM_TRAVELGET				WM_APP + 302

//////////////////////////////////////////////////////////////////////
// etc
//////////////////////////////////////////////////////////////////////
#define WM_INFORMCURRENTPOSTS			WM_APP + 320
#define WM_NETCNTCHANGED				WM_APP + 321
#define WM_RECEIVEDRWORDS				WM_APP + 322

#define WM_WHEELENABLE					WM_APP + 323

#define WM_XMLURL_FOUND					WM_APP + 324

//////////////////////////////////////////////////////////////////////
// IE MESSAGE ID
//////////////////////////////////////////////////////////////////////
// WM_APP + 350 ~ 400

#ifndef IE_MESSAGE
#define IE_MESSAGE

#define WM_IESIZE						WM_APP + 356
#define WM_NAVIGATENEW					WM_APP + 357
#define WM_GETNEWBROWSERPTR				WM_APP + 358
#define WM_SUBSCRIBE					WM_APP + 359
#define WM_SCRAP						WM_APP + 360
#define WM_READON						WM_APP + 361
#define WM_VIEW_IDPOST					WM_APP + 362

#define WM_INITIECONTROL				WM_APP + 381
#define WM_COMMANDSTATECHANGE			WM_APP + 382
#define WM_STOPBUTTONCHANGE				WM_APP + 383
#define WM_URLCHANGED					WM_APP + 384
#define WM_TITLECHANGED					WM_APP + 385
#define WM_USERREQ_GOTO_ADDRBAR         WM_APP + 386

#endif


//////////////////////////////////////////////////////////////////////////
// UI MSG
//////////////////////////////////////////////////////////////////////////
#define WM_UIMSG                        WM_APP      +   500
#define WM_REPOS_TREE_LABELUPDATE       WM_UIMSG    +   1
#define WM_LISTVIEW_SORT                WM_UIMSG    +   2
#define WM_INFOPANE_UPDATE_POST_QUANTITY    WM_UIMSG+   3
#define WM_REQUEST_POST_DBLCLK          WM_UIMSG    +   3
#define WM_REDRAW_RCMMDWNDS             WM_UIMSG    +   4

#define MAX_SCRAPMENU_ITEM              300


//////////////////////////////////////////////////////////////////////
// work list count & priority of work
//////////////////////////////////////////////////////////////////////
#define MAX_WORKLISTCNT					2

#define PTW_POST_ADD					0
#define PTW_POST_UPDATE					0
#define PTW_POST_GET					0
#define PTW_POST_DELETE					0
#define PTW_POST_SCRAP					0
#define PTW_POST_UPLOAD					0
#define PTW_CHANNEL_ADD					0
#define PTW_CHANNEL_MODIFY				0
#define PTW_CHANNEL_DELETE				0
#define PTW_CHANNEL_GET					0
#define PTW_CHANNEL_LOAD				0
#define	PTW_CHANNEL_READ				0
#define PTW_CHANNEL_FLUSH				1
#define PTW_SEARCH						1
#define PTW_CHANNEL_SETPATH				1

//////////////////////////////////////////////////////////////////////
// search list count & priority of search
//////////////////////////////////////////////////////////////////////
#define MAX_SEARCHLISTCNT				2

#define PTS_NET_SEARCH					0
#define PTS_LOCAL_SEARCH				0
#define PTS_LONGTERM_SEARCH				1
#define PTS_GRAPH_SEARCH				1

//////////////////////////////////////////////////////////////////////
// SUBSCRIBE VIEW DEFINITION
//////////////////////////////////////////////////////////////////////

enum CHANNEL_STATE
{
	CS_IDLE = 0,
	CS_NEW,
	CS_FEEDING,
	CS_FAILED,
	CS_UNKNOWN
};

//////////////////////////////////////////////////////////////////////
// FISH VIEW DEFINITION
//////////////////////////////////////////////////////////////////////

enum VIEW_ID
{
	VID_SUBSCRIBE = 0,
	VID_INFORMATION,
	MAX_MAINVIEWID
};


//////////////////////////////////////////////////////////////////////
// RSS DB DEFINITION
//////////////////////////////////////////////////////////////////////
#define STORAGE_DAYS					14
#define MAX_UNITITEMCNT					50
#define MAX_MEMORYLISTCNT				5
#define MAX_CHANNELITEMCNT				MAX_UNITITEMCNT * MAX_MEMORYLISTCNT
#define MAX_WORKSEQ						LONG_MAX
#define MAX_OLDLISTCNT					3
#define	MAX_QUEUE						50000
#define MAX_SEARCHQUEUE					30
#define MAX_SEARCHCOUNT					60			// 네트워크 검색의 최대 검색 결과 개수
#define MAX_NETSEARCHCHANNEL			20			// 네트워크 검색시 최대 검색 대상이 되는 채널의 수
#define MIN_KEEPCNT						10

#define INTERVAL_FLUSH					600			// seconds
#define INTERVAL_OLDUSE					300			// seconds

#define FISH_VERSION					PROG_VERSION
#define FISHPOST_VERSION				_T("1.0")
//#define FISHOPML_VERSION				_T("0.5")

#define XML_FISH						_T("fish")
#define XML_CHANNEL						_T("channel")
#define XML_VERSION						_T("version")
#define XML_FROM						_T("from")
#define XML_TO							_T("to")
#define XML_LOAD						_T("load")
#define XML_CHANNELID					_T("channelid")
#define XML_CHANNELTYPE					_T("channeltype")
#define XML_CHANNELURL					_T("channelurl")

#define XML_ITEM						_T("item")
#define XML_ID							_T("id")
#define XML_READ						_T("read")
#define XML_READON						_T("readon")
//#define XML_XMLUPDATE					_T("xmlupdate")
#define XML_PUBDATE						_T("pubdate")
#define XML_SUBJECT						_T("subject")
#define XML_GUID						_T("guid")
#define XML_URL							_T("url")
#define XML_AUTHOR						_T("author")
#define XML_CATEGORY					_T("category")
#define XML_DESC						_T("desc")
#define XML_ENCLOSURE					_T("enclosure")
#define XML_STYPE						_T("stype")				// 검색된 결과인 경우 검색의 종류
#define XML_BNS							_T("bns")				// boolean of not-search
#define XML_ENCODED						_T("encoded")			// content:encoded 정보.... description으로 쓰이는 경우가 있음...

#define XML_KEEPCNT						_T("kc")
#define XML_KEEPDAYS					_T("kd")

// written by moonknit 2005-12-06
// 검색 결과의 가치판단 자료 
#define XML_SV_READ						_T("sv_read")
#define XML_SV_READON					_T("sv_readon")
#define XML_SV_SCRAP					_T("sv_scrap")
#define XML_SV_FILTER					_T("sv_filter")
#define XML_SV_HIT						_T("sv_hit")
// --

#define XML_TAG							_T("tag")

#define XML_NODE						_T("node")
#define XML_IP							_T("ip")
#define XML_PORT						_T("port")
#define XML_POSTID						_T("postid")

#define XML_DECLARATION					_T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>")
#define XML_ROOT_DECLARATION			_T("<fish/>")

#define FISH_DT_TEMPLETE				_T("%m/%d/%Y %H:%M:%S")

#define XML_XPATH_ITEMSEARCH_ID			_T("//item[@id=\"%d\"]")
#define XML_XPATH_ITEMSEARCH_UNREAD		_T("//item[@read=\"0\"]")
#define XML_XPATH_ITEMSEARCH_READ		_T("//item[@read=\"1\"]")
#define XML_XPATH_ITEMSEARCH_GUID		_T("//item[@guid=\"%s\"]")
#define XML_XPATH_ITEM_ALL				_T("//item")
#define XML_XPATH_CHANNEL				_T("//channel")

#define STR_XML_CON_TRAN_XPATH_TEMPLATE		_T("contains(translate(%s, \'ABCDEFGHIJKLMNOPQRSTUVWXYZ \', \'abcdefghijklmnopqrstuvwxyz\'), \"%s\")")

// SEARCH DESTINATION CLAUSE
#define STR_XML_WHERE_SUBJECT			_T("./@subject")
#define STR_XML_WHERE_AUTHOR			_T("./@author")
#define STR_XML_WHERE_DESC				_T("./@desc")

// rss version
#define RV_RSS20						_T("rss2.0")
#define RV_RSS091						_T("rss0.91")
#define RV_RSS092						_T("rss0.92")
#define RV_RSS10						_T("rss1.0")
#define RV_ATOM03						_T("atom0.3")
#define RV_ATOM10						_T("atom1.0")

//////////////////////////////////////////////////////////////////////
// SUBSCRIBE VIEW STRING DEFINITION
//////////////////////////////////////////////////////////////////////
#define S_DEFAULT_LOCALOPML				_T("default.opml")



//////////////////////////////////////////////////////////////////////
// RSS & OPML TAG DEFINITION
//////////////////////////////////////////////////////////////////////
#define XML_TITLE						_T("title")
#define XML_TEXT						_T("text")
#define XML_DESCRIPTION					_T("description")
#define XML_DC_DESCRIPTION				_T("dc:description")
#define XML_BODY						_T("body")
#define XML_DC_SUBJECT					_T("dc:subject")
#define XML_PUBDATE_2					_T("pubDate")
#define XML_DC_DATE						_T("dc:date")
#define XML_DC_CREATOR					_T("dc:creator")
#define XML_DC_RIGHTS					_T("dc:rights")
#define XML_DC_LANGUAGE					_T("dc:language")
#define XML_LINK						_T("link")
#define XML_CONTENT_ENCODED				_T("content:encoded")
#define XML_COMMENTS					_T("comments")
#define XML_WFW_COMMENTRSS				_T("wfw:commentRss")

// For rss 1.0
#define XML_TITLE_F						_T("*[name()='title']")
#define XML_DESCRIPTION_F				_T("*[name()='description']")
#define XML_DC_DESCRIPTION_F			_T("*[name()='dc:description']")
#define XML_DC_SUBJECT_F				_T("*[name()='dc:subject']")
#define XML_PUBDATE_F					_T("*[name()='pubDate']")
#define XML_LINK_F						_T("*[name()='link']")
#define XML_GUID_F						_T("*[name()='guid']")
#define XML_COMMENTS_F					_T("*[name()='comments']")
#define XML_ENCLOSURE_F					_T("*[name()='enclosure']")

// For Atom
#define XML_XPATH_ENTRY_F				_T("//*[name()='entry']")
#define XML_XPATH_AUTHOR_NAME_F			_T("*[name()='author']/*[name()='name']")
#define XML_MODIFIED_F					_T("*[name()='modified']")
#define XML_UPDATED_F					_T("*[name()='updated']")
#define XML_ISSUED_F					_T("*[name()='issued']")
#define XML_CREATED_F					_T("*[name()='created']")
#define XML_CATEGORY_F					_T("*[name()='category']")
#define XML_CONTENT_F					_T("*[name()='content']")
#define XML_SUMMARY_F					_T("*[name()='summary']")
#define XML_ID_F						_T("*[name()='id']")
#define XML_REL							_T("rel")
#define XML_HREF						_T("href")
#define XML_TYPE_ALTERNATE				_T("alternate")
#define XML_TYPE_ENCLOSURE				_T("enclosure")

// For OPML (FISH or Original)
#define OPML_ROOT_DECLARATION			_T("<opml/>")
#define XML_XPATH_OPML					_T("//opml")
#define XML_XPATH_DATEMODIFIED			_T("//dateModified")
#define XML_XPATH_OUTLINE				_T("//outline")
#define XML_XPATH_BODY					_T("//body")
#define XML_XPATH_HEAD					_T("//head")
#define XML_ROOT_TITLE					_T("Fish List")
#define XML_DATECREATED					_T("dateCreated")
#define	XML_DATEMODIFIED				_T("dateModified")
#define XML_OWNERNAME					_T("ownerName")
#define XML_OWNEREMAIL					_T("ownerEmail")
#define XML_HEAD						_T("head")
#define XML_OUTLINE						_T("outline")
#define XML_TYPE						_T("type")
#define XML_XMLURL						_T("xmlUrl")
#define XML_HTMLURL						_T("htmlUrl")
#define XML_COMMENT						_T("comment")
#define XML_LASTBUILDDATE				_T("lastBuildDate")
#define XML_FEEDINTERVAL				_T("feedInterval")
#define XML_RSS							_T("rss")
#define XML_EXPANSIONSTATE				_T("expasionState")
#define XML_GROUPTYPE					_T("grouptype")

//////////////////////////////////////////////////////////////////////
// MESSAGE STRING
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// LOGIN ERROR STRING
//////////////////////////////////////////////////////////////////////
#define STR_LERR_UNKNOWN				_T("로그인에 실패했습니다.")
#define STR_LERR_ID						_T("존재하지 않는 ID 입니다.")
#define STR_LERR_PASSWORD				_T("틀린 패스워드 입니다.")
#define STR_LERR_DB						_T("DB 접속에 실패했습니다.")
#define STR_LERR_PARAMETER				_T("로그인 정보 값이 잘못 되었습니다.")
#define STR_LERR_HOSTNOTFOUND			_T("로그인 페이지에 접속 할 수 없습니다.")

//////////////////////////////////////////////////////////////////////
// CHANGE PORT DLG STRING
//////////////////////////////////////////////////////////////////////
#define STR_MSG_CHGPORTDLG_001			_T("0 이외의 값을 입력하셔야 합니다.")
#define STR_MSG_CHGPORTDLG_002			_T("포트번호 오류")
#define STR_MSG_CHGPORTDLG_003			_T("포트 번호를 입력해 주세요")
#define STR_MSG_CHGPORTDLG_004			_T("포트번호")
#define STR_CHG_PORT_TITLE				_T("변경할 포트를 입력하십시오")

//////////////////////////////////////////////////////////////////////
// CONFIG VIEW STRING
//////////////////////////////////////////////////////////////////////
#define STR_MSG_CONFIGVIEW_029			_T("ENPPY 포트 변경")
#define STR_MSG_CONFIGVIEW_030			_T("포트의 값이 너무 큽니다. 49000이하의 값을 입력하여 주십시오.")
#define STR_MSG_CONFIGVIEW_031			_T("포트의 값이 너무 작습니다. 1 이상의 값을 입력하여 주십시오.")

//////////////////////////////////////////////////////////////////////
// SUBSCRIBE STRING
//////////////////////////////////////////////////////////////////////
#define STR_NEWGROUP					_T("New Group")
#define STR_ERR_OPML_EXPORT				_T("OPML 내보내기를 실패했습니다.")
#define STR_ERR_OPML_IMPORT				_T("OPML 가져오기를 실패했습니다.")
#define STR_OPML_FILE_FORMAT			_T("OPML file(.xml;.opml)|*.xml;*.opml|All file|*.*|")
#define STR_XML_FORMAT					_T(".xml")
#define STR_INFRO_WAITPOSTINIT			_T("현재 기존의 포스트를 읽어오고 있습니다.\r\n잠시만 기다려 주십시오.")

#define STR_HTML_FILE_FORMAT		_T("HTML file(.html, .htm)|*.html;*.htm|All file|*.*|")
#define STR_STYLE_FILE_FORMAT		_T("XSLT file(.xslt)|*.xslt|All file|*.*|")

//////////////////////////////////////////////////////////////////////
// SEARCH DIALOG STRING
//////////////////////////////////////////////////////////////////////
#define STR_SEARCH_LENGTHLIMIT			_T("검색어는 영문자 %d자 한글 %d자 이상입니다.")


//////////////////////////////////////////////////////////////////////
// MACRO
//////////////////////////////////////////////////////////////////////
#define GO_ON(A)						(A == TRUE)

//////////////////////////////////////////////////////////////////////////
// 마우스 사이즈 변경 관련 Define
#define	SC_DRAGMOVE						(0xF012)      // move by drag 
#define SC_SZLEFT						(0xF001)      
#define SC_SZRIGHT						(0xF002)      
#define SC_SZTOP						(0xF003)      
#define SC_SZTOPLEFT					(0xF004)      
#define SC_SZTOPRIGHT					(0xF005)      
#define SC_SZBOTTOM						(0xF006)      
#define SC_SZBOTTOMLEFT					(0xF007)      
#define SC_SZBOTTOMRIGHT				(0xF008)  

//////////////////////////////////////////////////////////////////////
// TIMER ID
//////////////////////////////////////////////////////////////////////
#define TID_MAINTIMER					1
#define TID_REPOSITORY_ANIMATION        2
#define TID_REPOSITORY_LABELEDIT        3
#define	TID_INITUSER					4
#define TID_SUBSCRIPTION                5
#define TID_SUBSCRIBE_OPMLSAVE			6
#define TID_EXIT						7
#define TID_VIEWLIST					8
#define TID_GNUOPEN						9
#endif // _FISH_DEF_H


//////////////////////////////////////////////////////////////////////
// FISH MESSAGE BOX DEFINITION
//////////////////////////////////////////////////////////////////////
#define FMB_OK					0x00000000L
#define FMB_OKCANCEL			0x00000001L
#define FMB_ABORTRETRYIGNORE	0x00000002L
#define FMB_YESNOCANCEL			0x00000003L
#define FMB_YESNO				0x00000004L
#define FMB_RETRYCANCEL			0x00000005L
#define FMB_YESYESALLNONOALL	0x00000006L
#define FMB_YESYESALLNO			0x00000007L
#define FMB_YESNONOALL			0x00000008L

#define FMB_ICONQUESTION		MB_ICONQUESTION
#define FMB_ICONEXCLAMATION		MB_ICONEXCLAMATION
#define FMB_ICONASTERISK		MB_ICONASTERISK
#define FMB_ICONHAND			MB_ICONHAND
#define FMB_ICONWARNING			MB_ICONEXCLAMATION
#define FMB_ICONSTOP			MB_ICONHAND
#define FMB_ICONERROR			MB_ICONHAND
#define FMB_ICONINFORMATION		MB_ICONINFORMATION

#define IDYESALL				20
#define IDNOALL					21

//#define ID_FISH_SCRAPMENU_BASE_VALUE    (32000 + 2000)