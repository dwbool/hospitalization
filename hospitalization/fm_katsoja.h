//---------------------------------------------------------------------------

#ifndef fm_katsojaH
#define fm_katsojaH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.Outline.hpp>
#include <Vcl.Samples.DirOutln.hpp>
#include "file_dir_funcs.hpp"
#include <Xml.xmldom.hpp>
#include <Xml.XmlTransform.hpp>
#include "survey_utils.h"
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Dialogs.hpp>
#include <Data.DB.hpp>
#include <Data.Win.ADODB.hpp>
#include <Vcl.Menus.hpp>
#include <inifiles.hpp>
#include <Vcl.Buttons.hpp>
#include "fm_database.hpp"
#include "dm_base.hpp"
#include "hosp_utils.h"
//---------------------------------------------------------------------------
#define WM_TRAY_ICON_MESSAGE (WM_USER+1997)

class TfmSurveyMain : public TForm
{
__published:	// IDE-managed Components
	TButton *Button1;
	TMemo *m;
	TButton *Button2;
	TButton *Button3;
	TTimer *Timer1;
	TButton *btNewFileAdded;
	TADOQuery *qOmni;
	TButton *Button4;
	TButton *Button5;
	TButton *Button6;
	TPopupMenu *PopupMenu1;
	TMenuItem *Settings1;
	TMenuItem *Exit1;
	TMainMenu *MainMenu1;
	TMenuItem *File1;
	TMenuItem *Exit2;
	TMenuItem *IDphysmax1;
	TMenuItem *Settings2;
	TSpeedButton *btRun;
	TSpeedButton *btStop;
	TLabel *lbRun;
	TListBox *lbxFiles;
	TMenuItem *N1;
	TMenuItem *Database1;
	TTimer *Timer2;
	TMenuItem *findIDphysical1;
	TMenuItem *findOutcomedirectory1;
	TMenuItem *hospital1;
	TMenuItem *t21;
	TMenuItem *t41;
	TMenuItem *t51;
	TMenuItem *t61;
	TMenuItem *strin1;
	TMenuItem *required1;
	TMenuItem *scope1;
	TMenuItem *ident1;
	TMenuItem *XMLt11;
	TMenuItem *int1;
	TStatusBar *StatusBar1;
	TMenuItem *About1;
	TMenuItem *About2;
	TMenuItem *exec1;
	TMenuItem *Reports1;
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall btNewFileAddedClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall Button4Click(TObject *Sender);
	void __fastcall Button5Click(TObject *Sender);
	void __fastcall Button6Click(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall Exit1Click(TObject *Sender);
	void __fastcall IDphysmax1Click(TObject *Sender);
	void __fastcall Settings1Click(TObject *Sender);
	void __fastcall btRunClick(TObject *Sender);
	void __fastcall Database1Click(TObject *Sender);
	void __fastcall Timer2Timer(TObject *Sender);
	void __fastcall findIDphysical1Click(TObject *Sender);
	void __fastcall findOutcomedirectory1Click(TObject *Sender);
	void __fastcall hospital1Click(TObject *Sender);
	void __fastcall t21Click(TObject *Sender);
	void __fastcall t41Click(TObject *Sender);
	void __fastcall t51Click(TObject *Sender);
	void __fastcall t61Click(TObject *Sender);
	void __fastcall strin1Click(TObject *Sender);
	void __fastcall required1Click(TObject *Sender);
	void __fastcall scope1Click(TObject *Sender);
	void __fastcall ident1Click(TObject *Sender);
	void __fastcall int1Click(TObject *Sender);
	void __fastcall About2Click(TObject *Sender);
	void __fastcall exec1Click(TObject *Sender);
	void __fastcall Reports1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfmSurveyMain(TComponent* Owner);
	void TrayIcon_HandleMessage(TMessage &Message);
   BEGIN_MESSAGE_MAP
    MESSAGE_HANDLER(WM_TRAY_ICON_MESSAGE,TMessage,TrayIcon_HandleMessage)
  END_MESSAGE_MAP(TForm)
};


int add_person(
	   TADOQuery* qOmni,
	   String FAM,
	   String IM,
	   String OT,
	   int W,
	   String NPOLIS,
	   int ID_Physical,
	   int ID_File , TDate DR);
AnsiString sqlDate(TDateTime dt );
AnsiString sqlDateTime(TDateTime dt );

int find_idcard_person(TADOQuery* qOmni,String NPOLIS,int & id_physical, TPersonInfo & inf);
int scopeIdentity(TADOQuery * q);
//---------------------------------------------------------------------------
extern PACKAGE TfmSurveyMain *fmSurveyMain;
extern TIniFile * iniSet;

void add_slash(String & s);

//---------------------------------------------------------------------------
#endif
