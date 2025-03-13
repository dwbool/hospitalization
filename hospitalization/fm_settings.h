//---------------------------------------------------------------------------

#ifndef fm_settingsH
#define fm_settingsH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ComCtrls.hpp>
//---------------------------------------------------------------------------
class TfmSettings : public TForm
{
__published:	// IDE-managed Components
	TBitBtn *BitBtn1;
	TBitBtn *BitBtn2;
	TGroupBox *GroupBox1;
	TLabel *Label1;
	TSpeedButton *SpeedButton1;
	TEdit *edIncomingDir;
	TCheckBox *chSurveyRun;
	TGroupBox *GroupBox2;
	TLabel *Label4;
	TSpeedButton *SpeedButton4;
	TEdit *edIncomingDirHosp;
	TCheckBox *chHospRun;
	TGroupBox *GroupBox3;
	TLabel *Label2;
	TSpeedButton *SpeedButton2;
	TLabel *Label3;
	TSpeedButton *SpeedButton3;
	TEdit *edProcessedDir;
	TEdit *edOutcomingDir;
	TCheckBox *chReport;
	TDateTimePicker *dpReport;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall BitBtn1Click(TObject *Sender);
	void __fastcall SpeedButton1Click(TObject *Sender);
	void __fastcall SpeedButton2Click(TObject *Sender);
	void __fastcall BitBtn2Click(TObject *Sender);
	void __fastcall SpeedButton3Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfmSettings(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfmSettings *fmSettings;
//---------------------------------------------------------------------------
#endif
