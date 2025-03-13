//---------------------------------------------------------------------------

#ifndef fm_hosp_rep_mainH
#define fm_hosp_rep_mainH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Menus.hpp>
#include <Data.DB.hpp>
#include <Data.Win.ADODB.hpp>
#include <Vcl.DBGrids.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmHospRepMain : public TForm
{
__published:	// IDE-managed Components
	TMainMenu *MainMenu1;
	TMenuItem *File1;
	TMenuItem *Process1;
	TMenuItem *N1;
	TMenuItem *Exit1;
	TMenuItem *Database1;
	TADOQuery *qOmni;
	TDataSource *DataSource1;
	TDBGrid *DBGrid1;
	TListBox *lbxRep;
	TMemo *m;
	TSpeedButton *btRun;
	TComboBox *cbDay;
	TLabel *Label1;
	TLabel *Label2;
	TSaveDialog *SaveDialog1;
	TOpenDialog *OpenDialog1;
	TTimer *Timer1;
	TSpeedButton *btRunT2;
	TSpeedButton *btRunAll;
	TMenuItem *Open1;
	TMenuItem *Openoutcomingfile1;
	TMenuItem *Explore1;
	TSpeedButton *btRunT3;
	TMenuItem *Npolistoauxtable1;
	TADOQuery *qIns;
	TSpeedButton *btRunT4;
	TSpeedButton *btRunT5;
	TSpeedButton *btRunT6;
	void __fastcall Database1Click(TObject *Sender);
	void __fastcall Process1Click(TObject *Sender);
	void __fastcall btRunClick(TObject *Sender);
	void __fastcall lbxRepClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall cbDayChange(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall btRunT2Click(TObject *Sender);
	void __fastcall btRunAllClick(TObject *Sender);
	void __fastcall Open1Click(TObject *Sender);
	void __fastcall DBGrid1DblClick(TObject *Sender);
	void __fastcall Explore1Click(TObject *Sender);
	void __fastcall btRunT3Click(TObject *Sender);
	void __fastcall Npolistoauxtable1Click(TObject *Sender);
	void __fastcall btRunT4Click(TObject *Sender);
	void __fastcall btRunT5Click(TObject *Sender);
	void __fastcall btRunT6Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmHospRepMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmHospRepMain *frmHospRepMain;
//---------------------------------------------------------------------------
#endif
