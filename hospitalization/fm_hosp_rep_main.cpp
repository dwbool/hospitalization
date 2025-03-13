//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "fm_hosp_rep_main.h"
#include "dm_base.hpp"
#include "fm_database.hpp"
#include "survey_utils.h"
#include "hosp_utils.h"
#include "file_dir_funcs.hpp"
#include "dateutils.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmHospRepMain *frmHospRepMain;
TIniFile * iniSet=0;


//---------------------------------------------------------------------------
__fastcall TfrmHospRepMain::TfrmHospRepMain(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmHospRepMain::Database1Click(TObject *Sender)
{
	 fmDatabase->Show();
}
//---------------------------------------------------------------------------

void __fastcall TfrmHospRepMain::Process1Click(TObject *Sender)
{
	  fmDatabase->btConnectClick(0);
	  qOmni->Active=1;
	  //

	  String sql="";

}
//---------------------------------------------------------------------------


void getConditionEndTime(TDateTime dtNow ,int NdaysBackInitial, int & NdaysToApply, String & sTimeEnd )
{
	 int dw=  DayOfTheWeek(dtNow);
	 NdaysToApply=   NdaysBackInitial;
	 if (dw==1 ) {
		 NdaysToApply+=2; // to set to Friday
	 }
	 if (NdaysBackInitial==1) // default setting, yesterday
	 {
		 //
		 sTimeEnd="10:00";
	 }else
	 {
        sTimeEnd="23:59"  ;
	 }
}

int  NdaysToApplyBack=1;
String sTimeEnd="10:00";

void __fastcall TfrmHospRepMain::btRunClick(TObject *Sender)
{
  // all
  getConditionEndTime(Now(),StrToIntDef(cbDay->Text,1) , NdaysToApplyBack, sTimeEnd);
  //

  int TNUM=1;
  int persons_added=0;




  bool btRunAll_en=   btRunAll->Enabled;
  if (Sender) {


  m->Lines->Add("started "+DateTimeToStr(Now()));
  }
  String sDirOutcome= iniSet->ReadString("settings","outcoming_dir",ExtractFilePath(Application->ExeName));

  fmDatabase->btConnectClick(0);


  AnsiString MCOD_STC_="";
  if (lbxRep->ItemIndex==0) {
	//T1
  m->Lines->Add("T1 report ");

 
  AnsiString sql=
  "SELECT [MCOD_STC] \
	  ,[FAM] \
	  ,[IM]  \
	  ,[OT]  \
	  ,f.PROC_DT \
	  ,f.[FILENAME],[faulty]  , [IDN_SCT],[IDN_PAC],[NOM_NAP],[DTA_NAP] \
	  ,[FRM_MP],[MCOD_NAP],[VPOLIS],[SPOLIS],[NPOLIS] \
	  ,[W]   \
	  ,[DR]  \
	  ,[TLF] \
	  ,[DS]  \
	  ,[USL_OK]  \
	  ,[KOD_PFK] \
	  ,[KOD_PFO] \
	  ,[KOD_DCT] \
	  ,[DTA_PLN] \
	  ,[SMOCOD]  \
	  ,[err_codes] \
  FROM [DB_SURVEYS].[dbo].[HOS_T1_PAC] p , [DB_SURVEYS].[dbo].files_log f \
  where p.IDN_SCT=f.ID and f.PROC_DT>= \
  cast(CONVERT(DATE,  GETDATE()-"+/*5*/IntToStr(/*cbDay->ItemIndex+1*/NdaysToApplyBack) +") as datetime)  +cast(CONVERT(TIME,  '10:00') as datetime) \
  and f.PROC_DT< \
  cast(CONVERT(DATE,  GETDATE()-"+/*4*/IntToStr(0)+") as datetime)  +cast(CONVERT(TIME,  '"/*10:00*/+sTimeEnd+ "') as datetime) \
  order by MCOD_STC,p.FAM,p.IM,p.OT";
  qOmni->Close();
  qOmni->SQL->Text=sql;
  qOmni->Open();

  for (int i = 0; i < qOmni->Fields->Count; i++) {
	  qOmni->Fields->Fields[i]->DisplayWidth=10;
  }

  if (Sender==0) {
      goto l_exit;
  }


  btRun->Enabled=false;
  btRunAll->Enabled=false;

  qOmni->First();
  TStringList *ssXml= new  TStringList;
  TStringList *ssXmlAll = new  TStringList;

  ssXmlAll->Add("<?xml version=\"1.0\" encoding=\"windows-1251\" standalone=\"yes\"?>");
		ssXmlAll->Add("<ZL_LIST>");
		ssXmlAll->Add("<ZGLV>");
		String sDirDropFile = find_or_create_dir_str(sDirOutcome,  "SOGAZ");
		AnsiString sDate= sqlDateNoQuotes(Now() );
		AnsiString fname_new_insurance = sDirDropFile+"T1-S-0003"+"-"+sDate+".XML";
		//fname=fname_new;
		ssXmlAll->Add("<FILENAME>"+ExtractFileName(fname_new_insurance)+"</FILENAME>");
		ssXmlAll->Add("<DATA>"+sDate+"</DATA>");
		ssXmlAll->Add("</ZGLV>");


  AnsiString sDir=ExtractFilePath(Application->ExeName)+"\\";

  AnsiString fname="1.xml";
  AnsiString sPerson_= "";

  //int N=0;
  while (1/*!qOmni->Eof*/ )
  try
  {
	  //Caption=IntToStr(N++);
	  //iniSet->WriteInteger("log","n",N);
	  //
	  Application->ProcessMessages();
	  /*
	  m->Lines->Add(
	   qOmni->Fields->Fields[0]->AsAnsiString+" "+
	   qOmni->Fields->Fields[1]->AsAnsiString+" "+
	   qOmni->Fields->Fields[2]->AsAnsiString	  );
	   */
	  //
	  AnsiString MCOD_STC=qOmni->FieldByName("MCOD_STC")   ->AsAnsiString;


	  if ((MCOD_STC!=MCOD_STC_) || qOmni->Eof) {

        sPerson_="";
		AnsiString sDate= sqlDateNoQuotes(Now() );
		//AnsiString fname=sDir+"pT1-F-0000-"+sDate+"_"+MCOD_STC_+".XML";
		//AnsiString fname=sDir+"T1-F-"+MCOD_STC_+"-"+sDate+".XML";
		//////ssXml->Add("</ZL_LIST>");
		if (MCOD_STC_!="") {
		   ssXml->Add("</ZL_LIST>");
			// file write
			int CODE_MO=StrToFloat( MCOD_STC_ );
			String sDirDropFile = find_or_create_dir(sDirOutcome,  CODE_MO);
			String sFileFullFinal = sDirDropFile+ExtractFileName(fname);
			//ssXml->SaveToFile(sFileFullFinal);
			//m->Lines->Add(sFileFullFinal) ;
			if ( persons_added>0) {
			  ssXml->SaveToFile(sFileFullFinal);
			  m->Lines->Add(sFileFullFinal) ;
			}
			else
			{
			  m->Lines->Add(sFileFullFinal+" has not been saved due to absence of records") ;
			}
		}
		ssXml->Clear();

		persons_added=0;
		ssXml->Add("<?xml version=\"1.0\" encoding=\"windows-1251\" standalone=\"yes\"?>");
		ssXml->Add("<ZL_LIST>");
		ssXml->Add("<ZGLV>");
		AnsiString fname_new = sDir+"T"+IntToStr(TNUM)+"-F-"+MCOD_STC+"-"+sDate+".XML";
		fname=fname_new;
		ssXml->Add("<FILENAME>"+ExtractFileName(fname_new)+"</FILENAME>");
		ssXml->Add("<DATA>"+sDate+"</DATA>");
		ssXml->Add("</ZGLV>");

		MCOD_STC_=MCOD_STC;

		if (qOmni->Eof) {
		          m->Lines->Add("done "+DateTimeToStr(Now())) ;
                  break;
		}
	  }
/*		ssXml->Add("<?xml version=\"1.0\" encoding=\"WINDOWS-1251\" standalone=\"true\"?>");*/

		//ssXml->Add("<REPORT>");
		//ssXml->Add("</REPORT>");
//		ssXml->SaveToFile(sDir+"aaa.xml");


		//////////////////  from Word document
		AnsiString NOM_NAP =qOmni->FieldByName("NOM_NAP")->AsAnsiString;
		TDate DTA_NAP =(qOmni->FieldByName("DTA_NAP")->AsDateTime);
		AnsiString FRM_MP=qOmni->FieldByName("FRM_MP")->AsAnsiString;
		AnsiString MCOD_NAP=qOmni->FieldByName("MCOD_NAP")->AsAnsiString;
		//AnsiString MCOD_STC=qOmni->FieldByName("MCOD_STC")->AsAnsiString;
		AnsiString VPOLIS=qOmni->FieldByName("VPOLIS")->AsAnsiString;
		AnsiString SPOLIS=qOmni->FieldByName("SPOLIS")->AsAnsiString;
		AnsiString NPOLIS=qOmni->FieldByName("NPOLIS")->AsAnsiString;
		AnsiString SMOCOD=qOmni->FieldByName("SMOCOD")->AsAnsiString;
		AnsiString FAM=qOmni->FieldByName("FAM")->AsAnsiString;
		AnsiString IM=qOmni->FieldByName("IM")->AsAnsiString;
		AnsiString OT=qOmni->FieldByName("OT")->AsAnsiString;
		AnsiString W=qOmni->FieldByName("W")->AsAnsiString;
		TDate DR=(qOmni->FieldByName("DR")->AsDateTime);
		AnsiString TLF=qOmni->FieldByName("TLF")->AsAnsiString;
		AnsiString DS=qOmni->FieldByName("DS")->AsAnsiString;
		AnsiString KOD_PFK=qOmni->FieldByName("KOD_PFK")->AsAnsiString;
		AnsiString KOD_PFO=qOmni->FieldByName("KOD_PFO")->AsAnsiString;
		AnsiString KOD_DÑT=qOmni->FieldByName("KOD_DCT")->AsAnsiString; //KOD_DCT probaly has cyrillic symb
		TDate DTA_PLN=(qOmni->FieldByName("DTA_PLN")->AsDateTime);
		AnsiString USL_OK=qOmni->FieldByName("USL_OK")->AsAnsiString;

		int faulty   =  qOmni->FieldByName("faulty")->AsInteger;
		int	IDN_PAC  =  qOmni->FieldByName("IDN_PAC")->AsInteger;
		AnsiString FILENAME    =  qOmni->FieldByName("FILENAME")->AsString;

		AnsiString sPerson= "\r\n<ZAP>\r\n\
		\r\n<NOM_NAP>"+NOM_NAP  +"   </NOM_NAP>        \
		\r\n<DTA_NAP> "+sqlDateNoQuotes(DTA_NAP)  +"  </DTA_NAP>        \
		\r\n<FRM_MP>  "+FRM_MP  +"  </FRM_MP>          \
		\r\n<MCOD_NAP>"+MCOD_NAP  +"  </MCOD_NAP>      \
		\r\n<MCOD_STC>"+MCOD_STC  +"  </MCOD_STC>      \
		\r\n<VPOLIS> "+VPOLIS  +"   </VPOLIS>          \
		\r\n<SPOLIS> "+SPOLIS  +"   </SPOLIS>          \
		\r\n<NPOLIS>"+NPOLIS  +"    </NPOLIS>          \
		\r\n<SMOCOD>"+ SMOCOD +"    </SMOCOD>          \
		\r\n<FAM>"+ FAM +"       </FAM>                \
		\r\n<IM>"+  IM+"        </IM>                  \
		\r\n<OT>"+OT  +"        </OT>                  \
		\r\n<W>"+W  +"         </W>                    \
		\r\n<DR>"+sqlDateNoQuotes(DR)  +"        </DR>                  \
		\r\n<TLF>"+ TLF +"       </TLF>                \
		\r\n<DS>"+DS  +"        </DS>                  \
		\r\n<KOD_PFK>"+ KOD_PFK +"   </KOD_PFK>        \
		\r\n<KOD_PFO>"+KOD_PFO  +"   </KOD_PFO>        \
		\r\n<KOD_DÑT>"+KOD_DÑT  +"   </KOD_DÑT>        \
		\r\n<DTA_PLN>"+sqlDateNoQuotes(DTA_PLN)  +"   </DTA_PLN>        \
		\r\n<USL_OK>"+  USL_OK+"    </USL_OK>          \
		\r\n</ZAP>\r\n";

	   //<!-- Write your comment-->


	   if (sPerson_!=sPerson) {

		if (faulty==0) {

		 persons_added++;
		 sPerson_=sPerson;
		 ssXml->Add(sPerson+"\r\n"+	"<!-- "+IntToStr(IDN_PAC)+" "+FILENAME+" -->  " );

		 ssXmlAll->Add(sPerson+"\r\n"+	"<!-- "+IntToStr(IDN_PAC)+" "+FILENAME+" -->  ");
		 //m->Lines->Add(sPerson);
		}

	   }

		/////////////////

	//

	  qOmni->Next();
  }
  catch(...){};

  ssXml->Free();
  ////////////////


  ssXmlAll->Add("</ZL_LIST>");
  ssXmlAll->SaveToFile(	 fname_new_insurance);
  m->Lines->Add(fname_new_insurance);
  ssXmlAll->Free();
  //m->Lines->Add(sDate);

/*
<ZL_LIST>

<ZGLV>
<DATA>2020-05-22</DATA>
<FILENAME>T1-M-1801-2020-05-21.xml</FILENAME>
</ZGLV>

<ZAP>
<NOM_NAP>   </NOM_NAP>
<DTA_NAP>   </DTA_NAP>
<FRM_MP>    </FRM_MP>
<MCOD_NAP>  </MCOD_NAP>
<MCOD_STC>  </MCOD_STC>
<VPOLIS>    </VPOLIS>
<SPOLIS>    </SPOLIS>
<NPOLIS>    </NPOLIS>
<SMOCOD>    </SMOCOD>
<FAM>       </FAM>
<IM>        </IM>
<OT>        </OT>
<W>         </W>
<DR>        </DR>
<TLF>       </TLF>
<DS>        </DS>
<KOD_PFK>   </KOD_PFK>
<KOD_PFO>   </KOD_PFO>
<KOD_DÑT>   </KOD_DÑT>
<DTA_PLN>   </DTA_PLN>
<USL_OK>    </USL_OK>
</ZAP>

</ZL_LIST>

  */
  ////////////////
  }
  else if (lbxRep->ItemIndex==1) {
	  //
	  btRunT2Click(Sender);
  }
  else if (lbxRep->ItemIndex==2) {
	  //
	  btRunT3Click(Sender);
  }
  else if (lbxRep->ItemIndex==3) {
	  //
	  btRunT4Click(Sender);
  }

  else if (lbxRep->ItemIndex==4) {
	  btRunT5Click(Sender);
  }
  else if (lbxRep->ItemIndex==5) {
	  btRunT6Click(Sender);
  }


  btRun->Enabled=true;
  l_exit:;

  btRunAll->Enabled=btRunAll_en;
}
//---------------------------------------------------------------------------

void __fastcall TfrmHospRepMain::lbxRepClick(TObject *Sender)
{
 btRunClick(0);
}
//---------------------------------------------------------------------------

void __fastcall TfrmHospRepMain::FormCreate(TObject *Sender)
{
 //lbxRep->ItemIndex=0;
// ini = new TIniFile()
 iniSet=new TIniFile(ExtractFilePath(Application->ExeName)+"survey_info.ini");

 //lbxRep->ItemIndex=iniSet->ReadInteger("report","days",10);

}
//---------------------------------------------------------------------------

void __fastcall TfrmHospRepMain::FormShow(TObject *Sender)
{
 int x=  iniSet->ReadInteger("report","days",0);

 //lbxRep->ItemIndex=  x;
 cbDay->ItemIndex=x;
 btRunClick(0);

}
//---------------------------------------------------------------------------

void __fastcall TfrmHospRepMain::cbDayChange(TObject *Sender)
{
 btRunClick(0);

 iniSet->WriteInteger("report","days",cbDay->ItemIndex);

}
//---------------------------------------------------------------------------

void __fastcall TfrmHospRepMain::Timer1Timer(TObject *Sender)
{
 Timer1->Enabled=false;
 if (ParamStr(1).LowerCase()=="/r") {
     //cbDay->ItemIndex=8;
	 btRunClick(btRun);
 }
}
//---------------------------------------------------------------------------

void __fastcall TfrmHospRepMain::btRunT2Click(TObject *Sender)
{


  int TNUM=2;
  int persons_added=0;

  String sDirOutcome= iniSet->ReadString("settings","outcoming_dir",ExtractFilePath(Application->ExeName));
  m->Lines->Add("T2 report");
  AnsiString MCOD_NAP_="";
   	//T2
  AnsiString sql=
  "SELECT       \
	   MCOD_NAP,   \
	   [FAM], \
	   [IM],   \
	   [OT]    \
	  ,f.[PROC_DT] \
	  ,f.[FILENAME], [faulty] \
	  ,[IDN_SCT]   \
	  ,[IDN_PAC]   \
	  ,[NOM_NAP]    \
	  ,[DTA_NAP]    \
	  ,[FRM_MP]     \
	  ,[MCOD_STC]   \
	  ,[DTA_FKT]    \
	  ,[TIM_FKT]    \
	  ,[VPOLIS],[SPOLIS],[NPOLIS],SMOCOD,[W],[DR],[KOD_PFK],[KOD_PFO],[NHISTORY] \
	  ,[DS],[err_codes] \
FROM [DB_SURVEYS].[dbo].[HOS_T2_PAC] p , [DB_SURVEYS].[dbo].files_log f \
  where p.IDN_SCT=f.ID and f.PROC_DT>= \
  cast(CONVERT(DATE,  GETDATE()-" +IntToStr(/*cbDay->ItemIndex+1*/NdaysToApplyBack)+ ") as datetime)  +cast(CONVERT(TIME,  '10:00') as datetime) \
  and f.PROC_DT<  \
  cast(CONVERT(DATE,  GETDATE()-0) as datetime)  +cast(CONVERT(TIME,  '"+sTimeEnd+/*23:00*/"') as datetime) \
  order by MCOD_NAP,p.FAM,p.IM,p.OT";
  qOmni->Close();
  qOmni->SQL->Text=sql;
  qOmni->Open();

  for (int i = 0; i < qOmni->Fields->Count; i++) {
	  qOmni->Fields->Fields[i]->DisplayWidth=10;
  }

  if (Sender==0) {
	  //goto l_exit;
	  return;
  }
  bool btRun_en=  btRun->Enabled;
  btRun->Enabled=false;

  qOmni->First();
  TStringList *ssXml= new  TStringList;
  TStringList *ssXmlAll = new  TStringList;

  ssXmlAll->Add("<?xml version=\"1.0\" encoding=\"windows-1251\" standalone=\"yes\"?>");
		ssXmlAll->Add("<ZL_LIST>");
		ssXmlAll->Add("<ZGLV>");
		String sDirDropFile = find_or_create_dir_str(sDirOutcome,  "SOGAZ");
		AnsiString sDate= sqlDateNoQuotes(Now() );
		AnsiString fname_new_insurance = sDirDropFile+"T"+IntToStr(TNUM)+"-S-0003"+"-"+sDate+".XML";
		//fname=fname_new;
		ssXmlAll->Add("<FILENAME>"+ExtractFileName(fname_new_insurance)+"</FILENAME>");
		ssXmlAll->Add("<DATA>"+sDate+"</DATA>");
		ssXmlAll->Add("</ZGLV>");


  AnsiString sDir=ExtractFilePath(Application->ExeName)+"\\";

  AnsiString fname="1.xml";
  AnsiString sPerson_= "";

  //int N=0;
  while (1/*!qOmni->Eof*/ )
  try
  {
	  //Caption=IntToStr(N++);
	  //iniSet->WriteInteger("log","n",N);
	  //
	  Application->ProcessMessages();
	  /*
	  m->Lines->Add(
	   qOmni->Fields->Fields[0]->AsAnsiString+" "+
	   qOmni->Fields->Fields[1]->AsAnsiString+" "+
	   qOmni->Fields->Fields[2]->AsAnsiString	  );
	   */
	  //
	  AnsiString MCOD_NAP=qOmni->FieldByName("MCOD_NAP")   ->AsAnsiString;


	  if ((MCOD_NAP!=MCOD_NAP_) || qOmni->Eof) {

		sPerson_="";
		AnsiString sDate= sqlDateNoQuotes(Now() );
		//AnsiString fname=sDir+"pT1-F-0000-"+sDate+"_"+MCOD_NAP_+".XML";
		//AnsiString fname=sDir+"T1-F-"+MCOD_NAP_+"-"+sDate+".XML";
		//////ssXml->Add("</ZL_LIST>");
		if (MCOD_NAP_!="") {
		   ssXml->Add("</ZL_LIST>");
			// file write
			int CODE_MO=StrToFloat( MCOD_NAP_ );
			String sDirDropFile = find_or_create_dir(sDirOutcome,  CODE_MO);
			String sFileFullFinal = sDirDropFile+ExtractFileName(fname);
			if ( persons_added>0) {
			  ssXml->SaveToFile(sFileFullFinal);
			  m->Lines->Add(sFileFullFinal) ;
			}
			else
			{
			  m->Lines->Add(sFileFullFinal+" has not been saved due to absence of records") ;
			}
		}
		ssXml->Clear();

		persons_added=0; //counter to decide whether or not we are going to drop this file into a directory
						 // if 0 - we will not, coz they said no need empty files
		ssXml->Add("<?xml version=\"1.0\" encoding=\"windows-1251\" standalone=\"yes\"?>");
		ssXml->Add("<ZL_LIST>");
		ssXml->Add("<ZGLV>");
		AnsiString fname_new = sDir+"T"+IntToStr(TNUM)+"-F-"+MCOD_NAP+"-"+sDate+".XML";
		fname=fname_new;
		ssXml->Add("<FILENAME>"+ExtractFileName(fname_new)+"</FILENAME>");
		ssXml->Add("<DATA>"+sDate+"</DATA>");
		ssXml->Add("</ZGLV>");

		MCOD_NAP_=MCOD_NAP;

		if (qOmni->Eof) {
				  m->Lines->Add("done "+DateTimeToStr(Now())) ;
                  break;
		}
	  }
/*		ssXml->Add("<?xml version=\"1.0\" encoding=\"WINDOWS-1251\" standalone=\"true\"?>");*/

		//ssXml->Add("<REPORT>");
		//ssXml->Add("</REPORT>");
//		ssXml->SaveToFile(sDir+"aaa.xml");


		//////////////////  from Word document

		AnsiString NOM_NAP=qOmni->FieldByName("NOM_NAP")->AsAnsiString;
		TDate DTA_NAP=qOmni->FieldByName("DTA_NAP")->AsDateTime;
		AnsiString FRM_MP=qOmni->FieldByName("FRM_MP")->AsAnsiString;
		AnsiString MCOD_STC=qOmni->FieldByName("MCOD_STC")->AsAnsiString;
		//AnsiString MCOD_NAP=qOmni->FieldByName("MCOD_NAP")->AsAnsiString;
		TDate DTA_FKT=qOmni->FieldByName("DTA_FKT")->AsDateTime;
		/*TTime*/String sTIM_FKT=qOmni->FieldByName("TIM_FKT")->AsString;
		int h=0,min=0;
		readSqlTime_colon(  sTIM_FKT, h,min);
		TTime TIM_FKT= EncodeTime(h,min,0,0);

		AnsiString VPOLIS=qOmni->FieldByName("VPOLIS")->AsAnsiString;
		AnsiString SPOLIS=qOmni->FieldByName("SPOLIS")->AsAnsiString;
		AnsiString NPOLIS=qOmni->FieldByName("NPOLIS")->AsAnsiString;
//		AnsiString SMOCOD=qOmni->FieldByName("SMOCOD")->AsAnsiString;
		AnsiString FAM=qOmni->FieldByName("FAM")->AsAnsiString;
		AnsiString IM=qOmni->FieldByName("IM")->AsAnsiString;
		AnsiString OT=qOmni->FieldByName("OT")->AsAnsiString;
		AnsiString W=qOmni->FieldByName("W")->AsAnsiString;
		TDate DR=qOmni->FieldByName("DR")->AsDateTime;
		AnsiString KOD_PFK=qOmni->FieldByName("KOD_PFK")->AsAnsiString;
		AnsiString KOD_PFO=qOmni->FieldByName("KOD_PFO")->AsAnsiString;
		AnsiString NHISTORY=qOmni->FieldByName("NHISTORY")->AsAnsiString;
		AnsiString DS=qOmni->FieldByName("DS")->AsAnsiString;
		AnsiString SMOCOD=qOmni->FieldByName("SMOCOD")->AsAnsiString;

		////////////////
		/*
		AnsiString NOM_NAP =qOmni->FieldByName("NOM_NAP")->AsAnsiString;
		AnsiString DTA_NAP =sqlDateNoQuotes(qOmni->FieldByName("DTA_NAP")->AsDateTime);
		AnsiString FRM_MP=qOmni->FieldByName("FRM_MP")->AsAnsiString;
		//AnsiString MCOD_NAP=qOmni->FieldByName("MCOD_NAP")->AsAnsiString;
		//AnsiString MCOD_NAP=qOmni->FieldByName("MCOD_NAP")->AsAnsiString;
		AnsiString VPOLIS=qOmni->FieldByName("VPOLIS")->AsAnsiString;
		AnsiString SPOLIS=qOmni->FieldByName("SPOLIS")->AsAnsiString;
		AnsiString NPOLIS=qOmni->FieldByName("NPOLIS")->AsAnsiString;
		AnsiString SMOCOD=qOmni->FieldByName("SMOCOD")->AsAnsiString;
		AnsiString FAM=qOmni->FieldByName("FAM")->AsAnsiString;
		AnsiString IM=qOmni->FieldByName("IM")->AsAnsiString;
		AnsiString OT=qOmni->FieldByName("OT")->AsAnsiString;
		AnsiString W=qOmni->FieldByName("W")->AsAnsiString;
		AnsiString DR=sqlDateNoQuotes(qOmni->FieldByName("DR")->AsDateTime);
		AnsiString TLF=qOmni->FieldByName("TLF")->AsAnsiString;
		AnsiString DS=qOmni->FieldByName("DS")->AsAnsiString;
		AnsiString KOD_PFK=qOmni->FieldByName("KOD_PFK")->AsAnsiString;
		AnsiString KOD_PFO=qOmni->FieldByName("KOD_PFO")->AsAnsiString;
		AnsiString KOD_DÑT=qOmni->FieldByName("KOD_DCT")->AsAnsiString; //KOD_DCT probaly has cyrillic symb
		AnsiString DTA_PLN=sqlDateNoQuotes(qOmni->FieldByName("DTA_PLN")->AsDateTime);
		AnsiString USL_OK=qOmni->FieldByName("USL_OK")->AsAnsiString;
		  */
		int faulty   =  qOmni->FieldByName("faulty")->AsInteger;
		int	IDN_PAC  =  qOmni->FieldByName("IDN_PAC")->AsInteger;
		AnsiString FILENAME    =  qOmni->FieldByName("FILENAME")->AsString;

		AnsiString sPerson= "\r\n<ZAP>\r\n\
		\r\n<DTA_NAP>"+sqlDateNoQuotes(DTA_NAP)  +"   </DTA_NAP>        \
		\r\n<FRM_MP> "+FRM_MP  +"  </FRM_MP>        \
		\r\n<MCOD_STC>  "+MCOD_STC  +"  </MCOD_STC>          \
		\r\n<MCOD_NAP>"+MCOD_NAP  +"  </MCOD_NAP>      \
		\r\n<DTA_FKT>"+sqlDateNoQuotes(DTA_FKT)  +"  </DTA_FKT>      \
		\r\n<TIM_FKT> "+surveyTime_NoQuotes(TIM_FKT)  +"   </TIM_FKT>          \
		\r\n<VPOLIS> "+VPOLIS  +"   </VPOLIS>          \
		\r\n<SPOLIS>"+SPOLIS  +"    </SPOLIS>          \
		\r\n<NPOLIS>"+ NPOLIS +"    </NPOLIS>          \
		\r\n<SMOCOD>"+ SMOCOD +"       </SMOCOD>                \
		\r\n<FAM>"+  FAM+"        </FAM>                  \
		\r\n<IM>"+  IM+"        </IM>                  \
		\r\n<OT>"+OT  +"        </OT>                  \
		\r\n<W>"+W  +"         </W>                    \
		\r\n<DR>"+sqlDateNoQuotes(DR)  +"        </DR>                  \
		\r\n<KOD_PFK>"+ KOD_PFK +"   </KOD_PFK>        \
		\r\n<KOD_PFO>"+KOD_PFO  +"   </KOD_PFO>        \
		\r\n<NHISTORY>"+NHISTORY+"   </NHISTORY>        \
		\r\n<DS>"+DS  +"        </DS>                 \
		\r\n</ZAP>\r\n";
		/*        NOM_NAP
DTA_NAP
FRM_MP
MCOD_STC
MCOD_NAP
DTA_FKT
TIM_FKT
VPOLIS
SPOLIS
NPOLIS
SMOCOD
FAM
IM
OT
W
DR
KOD_PFK
KOD_PFO
NHISTORY
DS

		*/

	   //<!-- Write your comment-->


	   if (sPerson_!=sPerson) {

		if (faulty==0) {


		 sPerson_=sPerson;
		 ssXml->Add(sPerson+"\r\n"+	"<!-- "+IntToStr(IDN_PAC)+" "+FILENAME+" -->  " );

		 ssXmlAll->Add(sPerson+"\r\n"+	"<!-- "+IntToStr(IDN_PAC)+" "+FILENAME+" -->  ");

		 persons_added++;
		 //m->Lines->Add(sPerson);
		}

	   }

		/////////////////

	//

	  qOmni->Next();
  }
  catch(...){};

  ssXml->Free();
  ////////////////


  ssXmlAll->Add("</ZL_LIST>");
  ssXmlAll->SaveToFile(	 fname_new_insurance);
  m->Lines->Add(fname_new_insurance);
  ssXmlAll->Free();

   l_exit:;
  btRun->Enabled=true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmHospRepMain::btRunAllClick(TObject *Sender)
{
 //
 for (int i = 0; i < lbxRep->Items->Count; i++) {
 btRunAll->Enabled=0;
 btRun->Enabled=0;
	 lbxRep->ItemIndex=i;
	 lbxRepClick(lbxRep);
	 btRunClick(btRun);
	 delay(100);
 }
 btRunAll->Enabled=1;
 btRun->Enabled=1;
}
//---------------------------------------------------------------------------

void __fastcall TfrmHospRepMain::Open1Click(TObject *Sender)
{
	 AnsiString sCmd=
	 //"c:\\develop\\outcome\\2833_auto\\T2-F-2833-2020-10-20.XML"
	 m->SelText
	 ,
	 sPar=m->SelText;

	if (m->Focused()) {


	 if (sCmd.Trim()=="") {
	   //if (m->get) {	   }
	  int Row= SendMessage(m->Handle, EM_LINEFROMCHAR, m->SelStart, 0);
	  int Col= m->SelStart - SendMessage(m->Handle, EM_LINEINDEX, Row, 0);
	  if (Row>-1) {
		 sCmd=m->Lines->Strings[Row];
	  }
	 }
	 }
	 else if (DBGrid1->Focused()) {
	   //
	   AnsiString s= DBGrid1->SelectedField->AsAnsiString;
       sCmd=s;
	 }

	 if(!FileExists(sCmd))
	 {
	 String sIncome= iniSet->ReadString("settings","incoming_dir_hosp","c:\\develop\\income\\");
	 String sDirProcessed = iniSet->ReadString("settings","processed_dir","c:\\develop\\income\\processed\\");
	 String sDirOutcome = iniSet->ReadString("settings","outcoming_dir","c:\\develop\\outcome\\");

	 TStringList *ssFiles=new TStringList;
	 SearchInDir_Subdirs_file(sIncome,sCmd,ssFiles  );
	 if (ssFiles->Count==0)  SearchInDir_files_only(sDirProcessed,sCmd,ssFiles  );
     if (ssFiles->Count==0)  SearchInDir_Subdirs_file(sDirOutcome,sCmd,ssFiles  );

	 if (ssFiles->Count) {
					   sCmd=ssFiles->Strings[0];
	 }
	 ssFiles->Free();     m->Lines->Add(sCmd);
	 }

	 ShellExecuteA(Handle,0,sCmd.c_str(),"",0,3 );
	 //ShellExecuteA(Handle,0,"open",sPar.c_str(),0,3 );
//	 m->Lines->Add(sCmd);
}
//---------------------------------------------------------------------------


String get_rep_file_name()
{
  //
  int TNUM=frmHospRepMain->lbxRep->ItemIndex+1;
  if (TNUM==1) {
  String sDirOutcome= iniSet->ReadString("settings","outcoming_dir",ExtractFilePath(Application->ExeName));
		AnsiString sDate= sqlDateNoQuotes(Now() );
   AnsiString MCOD_STC=frmHospRepMain->qOmni->FieldByName("MCOD_STC")   ->AsAnsiString;
   //AsAnsiString;
   AnsiString MCOD_NAP=frmHospRepMain->qOmni->FieldByName("MCOD_NAP")   ->AsAnsiString;
   //AsAnsiString;
   int iMCOD_STC=StrToFloat( MCOD_STC );
   String sDirDropFile = find_or_create_dir(sDirOutcome,  iMCOD_STC);
	AnsiString fname_new = sDirDropFile+"T"+IntToStr(TNUM)+"-F-"+MCOD_STC+"-"+sDate+".XML";
	return    fname_new;
  }else
  if (TNUM==2) {
   String sDirOutcome= iniSet->ReadString("settings","outcoming_dir",ExtractFilePath(Application->ExeName));
		AnsiString sDate= sqlDateNoQuotes(Now() );
   AnsiString MCOD_STC=frmHospRepMain->qOmni->FieldByName("MCOD_STC")   ->AsAnsiString;
   //AsAnsiString;
   AnsiString MCOD_NAP=frmHospRepMain->qOmni->FieldByName("MCOD_NAP")   ->AsAnsiString;
   //AsAnsiString;
//   int CODE_MO=StrToFloat( MCOD_NAP );
   int iMCOD_NAP=  StrToFloat( MCOD_NAP );
   String sDirDropFile = find_or_create_dir(sDirOutcome,  iMCOD_NAP);
   AnsiString fname_new = sDirDropFile+"T"+IntToStr(TNUM)+"-F-"+MCOD_NAP+"-"+sDate+".XML";
   return    fname_new;

  }
}
void __fastcall TfrmHospRepMain::DBGrid1DblClick(TObject *Sender)
{
 AnsiString fname=get_rep_file_name();
 m->Lines->Add(fname);
 if (FileExists(fname)) {
	  ShellExecuteA(Handle,0,fname.c_str(),"",0,3 );
 }
}
//---------------------------------------------------------------------------

void __fastcall TfrmHospRepMain::Explore1Click(TObject *Sender)
{
	//
    AnsiString sCmd=
	 //"c:\\develop\\outcome\\2833_auto\\T2-F-2833-2020-10-20.XML"
	 m->SelText
	 ,
	 sPar=m->SelText;

	 ShellExecuteA(Handle,0,"explorer.exe",("/select, "+sCmd).c_str(),0,3 );

}
//---------------------------------------------------------------------------

void __fastcall TfrmHospRepMain::btRunT3Click(TObject *Sender)
{
int TNUM=3;
  int persons_added=0;

  String sDirOutcome= iniSet->ReadString("settings","outcoming_dir",ExtractFilePath(Application->ExeName));
  m->Lines->Add("T3 report");
  AnsiString MCOD_="";

  String sql_simple=
 "SELECT [IDN_SCT]    \
	  ,[IDN_PAC]       \
	  ,[MCOD_STC]      \
	  ,[DTA_FKT]        \
	  ,[TIM_FKT]         \
	  ,[VPOLIS]           \
	  ,[SPOLIS]            \
	  ,[NPOLIS]             \
	  ,[FAM]                 \
	  ,[IM]                   \
	  ,[OT]                    \
	  ,[W]                      \
	  ,[DR]                      \
	  ,[KOD_PFK]                  \
	  ,[KOD_PFO]                   \
	  ,[NHISTORY]                   \
	  ,[DS]                          \
	  ,[faulty]                       \
	  ,[err_codes]             \
  FROM [DB_SURVEYS].[dbo].[HOS_T3_PAC] p,[DB_SURVEYS].[dbo].files_log f \
  where  \
  p.IDN_SCT=f.ID and f.PROC_DT>= \
  cast(CONVERT(DATE,  GETDATE()-" +IntToStr(/*cbDay->ItemIndex+1*/NdaysToApplyBack)+ ") as datetime)  +cast(CONVERT(TIME,  '10:00') as datetime) \
  and f.PROC_DT<  \
  cast(CONVERT(DATE,  GETDATE()-0) as datetime)  +cast(CONVERT(TIME,  '"+sTimeEnd+/*23:00*/"') as datetime) \
  order by p.FAM,p.IM,p.OT";

 String sql=
 "SELECT [IDN_SCT]    \
	  ,[IDN_PAC]       \
	  ,[MCOD_STC]      \
	  ,[DTA_FKT]        \
	  ,[TIM_FKT]         \
	  ,[VPOLIS]           \
	  ,[SPOLIS]            \
	  ,[NPOLIS]             \
	  ,[FAM]                 \
	  ,[IM]                   \
	  ,[OT]                    \
	  ,[W]                      \
	  ,[DR]                      \
	  ,[KOD_PFK]                  \
	  ,[KOD_PFO]                   \
	  ,[NHISTORY]                   \
	  ,[DS]                          \
	  ,[faulty]                       \
	  ,[err_codes], SMOCOD,f.filename, \
	  SUBSTRING(cast(pr.mcod as varchar(9)), Len(cast(pr.mcod as varchar(9)))-3, 4) as mcod           \
  FROM [DB_SURVEYS].[dbo].[HOS_T3_PAC] p,[DB_REG].[dbo].PRK_REG pr,[DB_SURVEYS].[dbo].files_log f \
  where p.NPOLIS=pr.ENP and pr.actual=1  and \
  p.IDN_SCT=f.ID and f.PROC_DT>= \
  cast(CONVERT(DATE,  GETDATE()-" +IntToStr(cbDay->ItemIndex+1)+ ") as datetime)  +cast(CONVERT(TIME,  '10:00') as datetime) \
  and f.PROC_DT<  \
  cast(CONVERT(DATE,  GETDATE()-0) as datetime)  +cast(CONVERT(TIME,  '23:00') as datetime) \
  order by pr.MCOD,p.FAM,p.IM,p.OT";
  qOmni->Close();
  qOmni->SQL->Text=sql;//sql_simple;//sql;
  qOmni->Open();

  if (Sender==0) {
	  //goto l_exit;
	  return;
  }
  //////////////
  //////////////
    bool btRun_en=  btRun->Enabled;
  btRun->Enabled=false;

  qOmni->First();
  TStringList *ssXml= new  TStringList;
  TStringList *ssXmlAll = new  TStringList;

  ssXmlAll->Add("<?xml version=\"1.0\" encoding=\"windows-1251\" standalone=\"yes\"?>");
		ssXmlAll->Add("<ZL_LIST>");
		ssXmlAll->Add("<ZGLV>");
		String sDirDropFile = find_or_create_dir_str(sDirOutcome,  "SOGAZ");
		AnsiString sDate= sqlDateNoQuotes(Now() );
		AnsiString fname_new_insurance = sDirDropFile+"T"+IntToStr(TNUM)+"-S-0003"+"-"+sDate+".XML";
		//fname=fname_new;
		ssXmlAll->Add("<FILENAME>"+ExtractFileName(fname_new_insurance)+"</FILENAME>");
		ssXmlAll->Add("<DATA>"+sDate+"</DATA>");
		ssXmlAll->Add("</ZGLV>");


  AnsiString sDir=ExtractFilePath(Application->ExeName)+"\\";

  AnsiString fname="1.xml";
  AnsiString sPerson_= "";

  //int N=0;
  while (1/*!qOmni->Eof*/ )
  try
  {
	  //Caption=IntToStr(N++);
	  //iniSet->WriteInteger("log","n",N);
	  //
	  Application->ProcessMessages();
	  /*
	  m->Lines->Add(
	   qOmni->Fields->Fields[0]->AsAnsiString+" "+
	   qOmni->Fields->Fields[1]->AsAnsiString+" "+
	   qOmni->Fields->Fields[2]->AsAnsiString	  );
	   */
	  //
	  //AnsiString MCOD_NAP=qOmni->FieldByName("MCOD_NAP")   ->AsAnsiString;
	  AnsiString MCOD =qOmni->FieldByName("MCOD")   ->AsAnsiString;


	  if ((MCOD!=MCOD_) || qOmni->Eof) {

		sPerson_="";
		AnsiString sDate= sqlDateNoQuotes(Now() );
		//AnsiString fname=sDir+"pT1-F-0000-"+sDate+"_"+MCOD_NAP_+".XML";
		//AnsiString fname=sDir+"T1-F-"+MCOD_NAP_+"-"+sDate+".XML";
		//////ssXml->Add("</ZL_LIST>");
		if (MCOD_!="") {
		   ssXml->Add("</ZL_LIST>");
			// file write
			int CODE_MO=StrToFloat( MCOD_ );
			String sDirDropFile = find_or_create_dir(sDirOutcome,  CODE_MO);
			String sFileFullFinal = sDirDropFile+ExtractFileName(fname);
			if ( persons_added>0) {
			  ssXml->SaveToFile(sFileFullFinal);
			  m->Lines->Add(sFileFullFinal) ;
			}
			else
			{
			  m->Lines->Add(sFileFullFinal+" has not been saved due to absence of records") ;
			}
		}
		ssXml->Clear();

		persons_added=0; //counter to decide whether or not we are going to drop this file into a directory
						 // if 0 - we will not, coz they said no need empty files
		ssXml->Add("<?xml version=\"1.0\" encoding=\"windows-1251\" standalone=\"yes\"?>");
		ssXml->Add("<ZL_LIST>");
		ssXml->Add("<ZGLV>");
		AnsiString fname_new = sDir+"T"+IntToStr(TNUM)+"-F-"+MCOD+"-"+sDate+".XML";
		fname=fname_new;
		ssXml->Add("<FILENAME>"+ExtractFileName(fname_new)+"</FILENAME>");
		ssXml->Add("<DATA>"+sDate+"</DATA>");
		ssXml->Add("</ZGLV>");

		MCOD_=MCOD;

		if (qOmni->Eof) {
				  m->Lines->Add("done "+DateTimeToStr(Now())) ;
                  break;
		}
	  }
/*		ssXml->Add("<?xml version=\"1.0\" encoding=\"WINDOWS-1251\" standalone=\"true\"?>");*/

		//ssXml->Add("<REPORT>");
		//ssXml->Add("</REPORT>");
//		ssXml->SaveToFile(sDir+"aaa.xml");


		//////////////////  from Word document
		AnsiString MCOD_STC=qOmni->FieldByName("MCOD_STC")->AsAnsiString;
TDate DTA_FKT=qOmni->FieldByName("DTA_FKT")->AsDateTime;
//AnsiString TIM_FKT=qOmni->FieldByName("TIM_FKT")->AsAnsiString;
String sTIM_FKT=qOmni->FieldByName("TIM_FKT")->AsString;
		int h=0,min=0;
		readSqlTime_colon(  sTIM_FKT, h,min);
		TTime TIM_FKT= EncodeTime(h,min,0,0);
AnsiString VPOLIS=qOmni->FieldByName("VPOLIS")->AsAnsiString;
AnsiString SPOLIS=qOmni->FieldByName("SPOLIS")->AsAnsiString;
AnsiString NPOLIS=qOmni->FieldByName("NPOLIS")->AsAnsiString;
AnsiString SMOCOD=qOmni->FieldByName("SMOCOD")->AsAnsiString;
AnsiString FAM=qOmni->FieldByName("FAM")->AsAnsiString;
AnsiString IM=qOmni->FieldByName("IM")->AsAnsiString;
AnsiString OT=qOmni->FieldByName("OT")->AsAnsiString;
AnsiString W=qOmni->FieldByName("W")->AsAnsiString;
TDate DR=qOmni->FieldByName("DR")->AsDateTime;
AnsiString KOD_PFK=qOmni->FieldByName("KOD_PFK")->AsAnsiString;
AnsiString KOD_PFO=qOmni->FieldByName("KOD_PFO")->AsAnsiString;
AnsiString NHISTORY=qOmni->FieldByName("NHISTORY")->AsAnsiString;
AnsiString DS=qOmni->FieldByName("DS")->AsAnsiString;
//AnsiString SMOCOD=qOmni->FieldByName("SMOCOD")->AsAnsiString;

		/*
		AnsiString NOM_NAP=qOmni->FieldByName("NOM_NAP")->AsAnsiString;
		TDate DTA_NAP=qOmni->FieldByName("DTA_NAP")->AsDateTime;
		AnsiString FRM_MP=qOmni->FieldByName("FRM_MP")->AsAnsiString;
		AnsiString MCOD_STC=qOmni->FieldByName("MCOD_STC")->AsAnsiString;
		//AnsiString MCOD_NAP=qOmni->FieldByName("MCOD_NAP")->AsAnsiString;
		TDate DTA_FKT=qOmni->FieldByName("DTA_FKT")->AsDateTime;
		String sTIM_FKT=qOmni->FieldByName("TIM_FKT")->AsString;
		int h=0,min=0;
		readSqlTime_colon(  sTIM_FKT, h,min);
		TTime TIM_FKT= EncodeTime(h,min,0,0);

		AnsiString VPOLIS=qOmni->FieldByName("VPOLIS")->AsAnsiString;
		AnsiString SPOLIS=qOmni->FieldByName("SPOLIS")->AsAnsiString;
		AnsiString NPOLIS=qOmni->FieldByName("NPOLIS")->AsAnsiString;
//		AnsiString SMOCOD=qOmni->FieldByName("SMOCOD")->AsAnsiString;
		AnsiString FAM=qOmni->FieldByName("FAM")->AsAnsiString;
		AnsiString IM=qOmni->FieldByName("IM")->AsAnsiString;
		AnsiString OT=qOmni->FieldByName("OT")->AsAnsiString;
		AnsiString W=qOmni->FieldByName("W")->AsAnsiString;
		TDate DR=qOmni->FieldByName("DR")->AsDateTime;
		AnsiString KOD_PFK=qOmni->FieldByName("KOD_PFK")->AsAnsiString;
		AnsiString KOD_PFO=qOmni->FieldByName("KOD_PFO")->AsAnsiString;
		AnsiString NHISTORY=qOmni->FieldByName("NHISTORY")->AsAnsiString;
		AnsiString DS=qOmni->FieldByName("DS")->AsAnsiString;
		AnsiString SMOCOD=qOmni->FieldByName("SMOCOD")->AsAnsiString;
		  */
		////////////////
		/*
		AnsiString NOM_NAP =qOmni->FieldByName("NOM_NAP")->AsAnsiString;
		AnsiString DTA_NAP =sqlDateNoQuotes(qOmni->FieldByName("DTA_NAP")->AsDateTime);
		AnsiString FRM_MP=qOmni->FieldByName("FRM_MP")->AsAnsiString;
		//AnsiString MCOD_NAP=qOmni->FieldByName("MCOD_NAP")->AsAnsiString;
		//AnsiString MCOD_NAP=qOmni->FieldByName("MCOD_NAP")->AsAnsiString;
		AnsiString VPOLIS=qOmni->FieldByName("VPOLIS")->AsAnsiString;
		AnsiString SPOLIS=qOmni->FieldByName("SPOLIS")->AsAnsiString;
		AnsiString NPOLIS=qOmni->FieldByName("NPOLIS")->AsAnsiString;
		AnsiString SMOCOD=qOmni->FieldByName("SMOCOD")->AsAnsiString;
		AnsiString FAM=qOmni->FieldByName("FAM")->AsAnsiString;
		AnsiString IM=qOmni->FieldByName("IM")->AsAnsiString;
		AnsiString OT=qOmni->FieldByName("OT")->AsAnsiString;
		AnsiString W=qOmni->FieldByName("W")->AsAnsiString;
		AnsiString DR=sqlDateNoQuotes(qOmni->FieldByName("DR")->AsDateTime);
		AnsiString TLF=qOmni->FieldByName("TLF")->AsAnsiString;
		AnsiString DS=qOmni->FieldByName("DS")->AsAnsiString;
		AnsiString KOD_PFK=qOmni->FieldByName("KOD_PFK")->AsAnsiString;
		AnsiString KOD_PFO=qOmni->FieldByName("KOD_PFO")->AsAnsiString;
		AnsiString KOD_DÑT=qOmni->FieldByName("KOD_DCT")->AsAnsiString; //KOD_DCT probaly has cyrillic symb
		AnsiString DTA_PLN=sqlDateNoQuotes(qOmni->FieldByName("DTA_PLN")->AsDateTime);
		AnsiString USL_OK=qOmni->FieldByName("USL_OK")->AsAnsiString;
		  */
		int faulty   =  qOmni->FieldByName("faulty")->AsInteger;
		int	IDN_PAC  =  qOmni->FieldByName("IDN_PAC")->AsInteger;
		AnsiString FILENAME    =  qOmni->FieldByName("FILENAME")->AsString;

		AnsiString sPerson= "\r\n<ZAP>\r\n\
<MCOD_STC>"+		MCOD_STC+ "   </MCOD_STC>\r\n\ \
<DTA_FKT>"+ sqlDateNoQuotes(DTA_FKT)  +         "  </DTA_FKT> \r\n\  \
<TIM_FKT>"+ surveyTime_NoQuotes(TIM_FKT)   +         " </TIM_FKT>  \r\n\  \
<VPOLIS>"+ VPOLIS      +       "  </VPOLIS>  \r\n\    \
<SPOLIS>"+ SPOLIS       +       " </SPOLIS>  \r\n\     \
<NPOLIS>"+ NPOLIS        +     "  </NPOLIS>  \r\n\      \
<SMOCOD>"+ SMOCOD  +            " </SMOCOD>  \r\n\          \
<FAM>"+ FAM         +           " </FAM>     \r\n\       \
<IM>"+ IM            +          " </IM>      \r\n\        \
<OT>"+ OT             +          "</OT>      \r\n\         \
<W>"+ W                +         "</W>       \r\n\           \
<DR>"+ sqlDateNoQuotes(DR   )            +        "</DR>       \r\n\           \
<KOD_PFK>"+ KOD_PFK      +       "</KOD_PFK>  \r\n\            \
<KOD_PFO>"+ KOD_PFO       +      "</KOD_PFO>   \r\n\            \
<NHISTORY>"+ NHISTORY      +     "</NHISTORY>  \r\n\             \
<DS>"+ DS                   +    "</DS>       \r\n\               \
		\r\n</ZAP>\r\n";
		/*        NOM_NAP
DTA_NAP
FRM_MP
MCOD_STC
MCOD_NAP
DTA_FKT
TIM_FKT
VPOLIS
SPOLIS
NPOLIS
SMOCOD
FAM
IM
OT
W
DR
KOD_PFK
KOD_PFO
NHISTORY
DS

		*/

	   //<!-- Write your comment-->


	   if (sPerson_!=sPerson) {

		if (faulty==0) {


		 sPerson_=sPerson;
		 ssXml->Add(sPerson+"\r\n"+	"<!-- "+IntToStr(IDN_PAC)+" "+FILENAME+" -->  " );

		 ssXmlAll->Add(sPerson+"\r\n"+	"<!-- "+IntToStr(IDN_PAC)+" "+FILENAME+" -->  ");

		 persons_added++;
		 //m->Lines->Add(sPerson);
		}

	   }

		/////////////////

	//

	  qOmni->Next();
  }
  catch(...){};

  ssXml->Free();
  ////////////////


  ssXmlAll->Add("</ZL_LIST>");
  ssXmlAll->SaveToFile(	 fname_new_insurance);
  m->Lines->Add(fname_new_insurance);
  ssXmlAll->Free();

   l_exit:;
  btRun->Enabled=true;


}
//---------------------------------------------------------------------------

void __fastcall TfrmHospRepMain::Npolistoauxtable1Click(TObject *Sender)
{
   //
   qOmni->First();
   while (!qOmni->Eof)
	   {
	   String s=qOmni->Fields->Fields[7]->AsString;
	   m->Lines->Add(s);

	   String mcod[]={"352511","352512"};
	   static int mcindex=1;
	   if(++mcindex>1) mcindex=0;

	   String sql=
	   "insert into [DB_REG].[dbo].PRK_REG ( \
	  [UNKOD]          \
	  ,[OGRN]            \
	  ,[GOD]              \
	  ,[START_MONTH]       \
	  ,[END_GOD]            \
	  ,[END_MONTH]           \
	  ,[ACTUAL]               \
	  ,[DOC_CODE]              \
	  ,[ENP]                    \
	  ,[mcod])  \
	   values (       \
	   1,      \
	   2,       \
	   3,        \
	   4,         \
	   5  ,         \
	   6  ,         \
	   1  ,         \
	   8  ,         \
	   '"+ s+"', '"+mcod[mcindex]
	   +"')";

	   m->Lines->Add("/////////////////////////");
	   m->Lines->Add(sql);

	   qIns->SQL->Text=sql;
	   qIns->ExecSQL();

	   qOmni->Next();
	   }

}
//---------------------------------------------------------------------------

void __fastcall TfrmHospRepMain::btRunT4Click(TObject *Sender)
{
  //
  int TNUM=4;
  int persons_added=0;

  String sDirOutcome= iniSet->ReadString("settings","outcoming_dir",ExtractFilePath(Application->ExeName));
  m->Lines->Add("T4 report");
  AnsiString MCOD_="";


 String sql=
 "SELECT               \
			\
	  t1.IDN_PAC as t1_idn_pac     \
	  ,t1.[NOM_NAP]       \
	  ,t1.[DTA_NAP]        \
	  ,[FRM_MP]             \
	  ,[MCOD_NAP]            \
	  ,[MCOD_STC]             \
	  ,[VPOLIS]                \
	  ,[SPOLIS]                 \
	  ,[NPOLIS]                  \
	  ,[FAM]                      \
	  ,[IM]                        \
	  ,[OT]                         \
	  ,[W]        \
	  ,[DR]        \
	  ,[TLF]        \
	  ,[DS]          \
	  ,[USL_OK]       \
	  ,[KOD_PFK]       \
	  ,[KOD_PFO]        \
	  ,[KOD_DCT]         \
	  ,[DTA_PLN]          \
	  ,[SMOCOD]            \
		 \
       \
 \
	  ,t4.[IDN_SCT]            \
	  ,t4.[IDN_PAC]             \
	  ,t4.[NOM_NAP]              \
	  ,t4.[DTA_NAP]               \
	  ,t4.[IST_ANL]                \
	  ,t4.[ACOD]                    \
	  ,t4.[PR_ANL]       \
	  ,t4.[faulty]        \
	  ,t4.[err_codes]      \
 \
	  ,f.FILENAME            \
	  ,f.PROC_DT              \
	  ,f.ERROR_CODE            \
	  ,f.CODE_MO                \
	  ,f.DATA                    \
								  \
  FROM                             \
	  [DB_SURVEYS].[dbo].[HOS_T1_PAC] t1, \
	  [DB_SURVEYS].[dbo].[HOS_T4_PAC] t4,  \
	  [DB_SURVEYS].[dbo].files_log f        \
											 \
  where                                       \
	  t1.NOM_NAP=t4.NOM_NAP                    \
	  and t4.DTA_NAP=t1.DTA_NAP                 \
	  and t4.IDN_SCT=f.ID                        \
	  and f.PROC_DT>=                             \
  cast(CONVERT(DATE,  GETDATE()-"+IntToStr(/*cbDay->ItemIndex+1*/NdaysToApplyBack)+") as datetime)  +cast(CONVERT(TIME,  '10:00') as datetime) \
  and f.PROC_DT<                                                                              \
  cast(CONVERT(DATE,  GETDATE()-0) as datetime)  +cast(CONVERT(TIME,  '"+sTimeEnd+/*23:00*/"') as datetime)    \
  order by t1.MCOD_NAP,t1.FAM,t1.IM,t1.OT ";
  qOmni->Close();
  qOmni->SQL->Text=sql;//sql_simple;//sql;
  qOmni->Open();

  if (Sender==0) {
	  //goto l_exit;
	  return;
  }
  //////////////
  //////////////
    bool btRun_en=  btRun->Enabled;
  btRun->Enabled=false;

  qOmni->First();
  TStringList *ssXml= new  TStringList;
  TStringList *ssXmlAll = new  TStringList;

  ssXmlAll->Add("<?xml version=\"1.0\" encoding=\"windows-1251\" standalone=\"yes\"?>");
		ssXmlAll->Add("<ZL_LIST>");
		ssXmlAll->Add("<ZGLV>");
		String sDirDropFile = find_or_create_dir_str(sDirOutcome,  "SOGAZ");
		AnsiString sDate= sqlDateNoQuotes(Now() );
		AnsiString fname_new_insurance = sDirDropFile+"T"+IntToStr(TNUM)+"-S-0003"+"-"+sDate+".XML";
		//fname=fname_new;
		ssXmlAll->Add("<FILENAME>"+ExtractFileName(fname_new_insurance)+"</FILENAME>");
		ssXmlAll->Add("<DATA>"+sDate+"</DATA>");
		ssXmlAll->Add("</ZGLV>");


  AnsiString sDir=ExtractFilePath(Application->ExeName)+"\\";

  AnsiString fname="1.xml";
  AnsiString sPerson_= "";

  //int N=0;
  while (1/*!qOmni->Eof*/ )
  try
  {
	  //Caption=IntToStr(N++);
	  //iniSet->WriteInteger("log","n",N);
	  //
	  Application->ProcessMessages();
	  /*
	  m->Lines->Add(
	   qOmni->Fields->Fields[0]->AsAnsiString+" "+
	   qOmni->Fields->Fields[1]->AsAnsiString+" "+
	   qOmni->Fields->Fields[2]->AsAnsiString	  );
	   */
	  //
	  //AnsiString MCOD_NAP=qOmni->FieldByName("MCOD_NAP")   ->AsAnsiString;
	  AnsiString MCOD =qOmni->FieldByName("MCOD_NAP")   ->AsAnsiString;


	  if ((MCOD!=MCOD_) || qOmni->Eof) {

		sPerson_="";
		AnsiString sDate= sqlDateNoQuotes(Now() );
		//AnsiString fname=sDir+"pT1-F-0000-"+sDate+"_"+MCOD_NAP_+".XML";
		//AnsiString fname=sDir+"T1-F-"+MCOD_NAP_+"-"+sDate+".XML";
		//////ssXml->Add("</ZL_LIST>");
		if (MCOD_!="") {
		   ssXml->Add("</ZL_LIST>");
			// file write
			int CODE_MO=StrToFloat( MCOD_ );
			String sDirDropFile = find_or_create_dir(sDirOutcome,  CODE_MO);
			String sFileFullFinal = sDirDropFile+ExtractFileName(fname);
			if ( persons_added>0) {
			  ssXml->SaveToFile(sFileFullFinal);
			  m->Lines->Add(sFileFullFinal) ;
			}
			else
			{
			  m->Lines->Add(sFileFullFinal+" has not been saved due to absence of records") ;
			}
		}
		ssXml->Clear();

		persons_added=0; //counter to decide whether or not we are going to drop this file into a directory
						 // if 0 - we will not, coz they said no need empty files
		ssXml->Add("<?xml version=\"1.0\" encoding=\"windows-1251\" standalone=\"yes\"?>");
		ssXml->Add("<ZL_LIST>");
		ssXml->Add("<ZGLV>");
		AnsiString fname_new = sDir+"T"+IntToStr(TNUM)+"-F-"+MCOD+"-"+sDate+".XML";
		fname=fname_new;
		ssXml->Add("<FILENAME>"+ExtractFileName(fname_new)+"</FILENAME>");
		ssXml->Add("<DATA>"+sDate+"</DATA>");
		ssXml->Add("</ZGLV>");

		MCOD_=MCOD;

		if (qOmni->Eof) {
				  m->Lines->Add("done "+DateTimeToStr(Now())) ;
                  break;
		}
	  }
/*		ssXml->Add("<?xml version=\"1.0\" encoding=\"WINDOWS-1251\" standalone=\"true\"?>");*/

		//ssXml->Add("<REPORT>");
		//ssXml->Add("</REPORT>");
//		ssXml->SaveToFile(sDir+"aaa.xml");


		//////////////////  from Word document
		/*
		AnsiString MCOD_STC=qOmni->FieldByName("MCOD_STC")->AsAnsiString;
TDate DTA_FKT=qOmni->FieldByName("DTA_FKT")->AsDateTime;
//AnsiString TIM_FKT=qOmni->FieldByName("TIM_FKT")->AsAnsiString;
String sTIM_FKT=qOmni->FieldByName("TIM_FKT")->AsString;
		int h=0,min=0;
		readSqlTime_colon(  sTIM_FKT, h,min);
		TTime TIM_FKT= EncodeTime(h,min,0,0);
AnsiString VPOLIS=qOmni->FieldByName("VPOLIS")->AsAnsiString;
AnsiString SPOLIS=qOmni->FieldByName("SPOLIS")->AsAnsiString;
AnsiString NPOLIS=qOmni->FieldByName("NPOLIS")->AsAnsiString;
AnsiString SMOCOD=qOmni->FieldByName("SMOCOD")->AsAnsiString;
AnsiString FAM=qOmni->FieldByName("FAM")->AsAnsiString;
AnsiString IM=qOmni->FieldByName("IM")->AsAnsiString;
AnsiString OT=qOmni->FieldByName("OT")->AsAnsiString;
AnsiString W=qOmni->FieldByName("W")->AsAnsiString;
TDate DR=qOmni->FieldByName("DR")->AsDateTime;
AnsiString KOD_PFK=qOmni->FieldByName("KOD_PFK")->AsAnsiString;
AnsiString KOD_PFO=qOmni->FieldByName("KOD_PFO")->AsAnsiString;
AnsiString NHISTORY=qOmni->FieldByName("NHISTORY")->AsAnsiString;
AnsiString DS=qOmni->FieldByName("DS")->AsAnsiString;
//AnsiString SMOCOD=qOmni->FieldByName("SMOCOD")->AsAnsiString;
		  */



AnsiString NOM_NAP=qOmni->FieldByName("NOM_NAP")->AsAnsiString;
TDateTime DTA_NAP=qOmni->FieldByName("DTA_NAP")->AsDateTime;
AnsiString IST_ANL=qOmni->FieldByName("IST_ANL")->AsAnsiString;
AnsiString ACOD=qOmni->FieldByName("ACOD")->AsAnsiString;
AnsiString PR_ANL=qOmni->FieldByName("PR_ANL")->AsAnsiString;



		/*
		AnsiString NOM_NAP=qOmni->FieldByName("NOM_NAP")->AsAnsiString;
		TDate DTA_NAP=qOmni->FieldByName("DTA_NAP")->AsDateTime;
		AnsiString FRM_MP=qOmni->FieldByName("FRM_MP")->AsAnsiString;
		AnsiString MCOD_STC=qOmni->FieldByName("MCOD_STC")->AsAnsiString;
		//AnsiString MCOD_NAP=qOmni->FieldByName("MCOD_NAP")->AsAnsiString;
		TDate DTA_FKT=qOmni->FieldByName("DTA_FKT")->AsDateTime;
		String sTIM_FKT=qOmni->FieldByName("TIM_FKT")->AsString;
		int h=0,min=0;
		readSqlTime_colon(  sTIM_FKT, h,min);
		TTime TIM_FKT= EncodeTime(h,min,0,0);

		AnsiString VPOLIS=qOmni->FieldByName("VPOLIS")->AsAnsiString;
		AnsiString SPOLIS=qOmni->FieldByName("SPOLIS")->AsAnsiString;
		AnsiString NPOLIS=qOmni->FieldByName("NPOLIS")->AsAnsiString;
//		AnsiString SMOCOD=qOmni->FieldByName("SMOCOD")->AsAnsiString;
		AnsiString FAM=qOmni->FieldByName("FAM")->AsAnsiString;
		AnsiString IM=qOmni->FieldByName("IM")->AsAnsiString;
		AnsiString OT=qOmni->FieldByName("OT")->AsAnsiString;
		AnsiString W=qOmni->FieldByName("W")->AsAnsiString;
		TDate DR=qOmni->FieldByName("DR")->AsDateTime;
		AnsiString KOD_PFK=qOmni->FieldByName("KOD_PFK")->AsAnsiString;
		AnsiString KOD_PFO=qOmni->FieldByName("KOD_PFO")->AsAnsiString;
		AnsiString NHISTORY=qOmni->FieldByName("NHISTORY")->AsAnsiString;
		AnsiString DS=qOmni->FieldByName("DS")->AsAnsiString;
		AnsiString SMOCOD=qOmni->FieldByName("SMOCOD")->AsAnsiString;
		  */
		////////////////
		/*
		AnsiString NOM_NAP =qOmni->FieldByName("NOM_NAP")->AsAnsiString;
		AnsiString DTA_NAP =sqlDateNoQuotes(qOmni->FieldByName("DTA_NAP")->AsDateTime);
		AnsiString FRM_MP=qOmni->FieldByName("FRM_MP")->AsAnsiString;
		//AnsiString MCOD_NAP=qOmni->FieldByName("MCOD_NAP")->AsAnsiString;
		//AnsiString MCOD_NAP=qOmni->FieldByName("MCOD_NAP")->AsAnsiString;
		AnsiString VPOLIS=qOmni->FieldByName("VPOLIS")->AsAnsiString;
		AnsiString SPOLIS=qOmni->FieldByName("SPOLIS")->AsAnsiString;
		AnsiString NPOLIS=qOmni->FieldByName("NPOLIS")->AsAnsiString;
		AnsiString SMOCOD=qOmni->FieldByName("SMOCOD")->AsAnsiString;
		AnsiString FAM=qOmni->FieldByName("FAM")->AsAnsiString;
		AnsiString IM=qOmni->FieldByName("IM")->AsAnsiString;
		AnsiString OT=qOmni->FieldByName("OT")->AsAnsiString;
		AnsiString W=qOmni->FieldByName("W")->AsAnsiString;
		AnsiString DR=sqlDateNoQuotes(qOmni->FieldByName("DR")->AsDateTime);
		AnsiString TLF=qOmni->FieldByName("TLF")->AsAnsiString;
		AnsiString DS=qOmni->FieldByName("DS")->AsAnsiString;
		AnsiString KOD_PFK=qOmni->FieldByName("KOD_PFK")->AsAnsiString;
		AnsiString KOD_PFO=qOmni->FieldByName("KOD_PFO")->AsAnsiString;
		AnsiString KOD_DÑT=qOmni->FieldByName("KOD_DCT")->AsAnsiString; //KOD_DCT probaly has cyrillic symb
		AnsiString DTA_PLN=sqlDateNoQuotes(qOmni->FieldByName("DTA_PLN")->AsDateTime);
		AnsiString USL_OK=qOmni->FieldByName("USL_OK")->AsAnsiString;
		  */
		int faulty   =  qOmni->FieldByName("faulty")->AsInteger;
		int	IDN_PAC  =  qOmni->FieldByName("IDN_PAC")->AsInteger;
		AnsiString FILENAME    =  qOmni->FieldByName("FILENAME")->AsString;

		AnsiString sPerson= "\r\n<ZAP>  \
\r\n<NOM_NAP>"+NOM_NAP+"</NOM_NAP>            \
\r\n<DTA_NAP>"+sqlDateNoQuotes(DTA_NAP)+"</DTA_NAP>            \
\r\n<IST_ANL>"+(IST_ANL)+"</IST_ANL>                     \
\r\n<ACOD>"+/*IntToStr*/(ACOD)+"</ACOD>                        \
\r\n<PR_ANL>"+(PR_ANL)+"</PR_ANL>                       \
\r\n</ZAP>\r\n";

		/*        NOM_NAP
DTA_NAP
FRM_MP
MCOD_STC
MCOD_NAP
DTA_FKT
TIM_FKT
VPOLIS
SPOLIS
NPOLIS
SMOCOD
FAM
IM
OT
W
DR
KOD_PFK
KOD_PFO
NHISTORY
DS

		*/

	   //<!-- Write your comment-->


	   if (sPerson_!=sPerson) {

		if (faulty==0) {


		 sPerson_=sPerson;
		 ssXml->Add(sPerson+"\r\n"+	"<!-- "+IntToStr(IDN_PAC)+" "+FILENAME+" -->  " );

		 ssXmlAll->Add(sPerson+"\r\n"+	"<!-- "+IntToStr(IDN_PAC)+" "+FILENAME+" -->  ");

		 persons_added++;
		 //m->Lines->Add(sPerson);
		}

	   }

		/////////////////

	//

	  qOmni->Next();
  }
  catch(...){};

  ssXml->Free();
  ////////////////


  ssXmlAll->Add("</ZL_LIST>");
  ssXmlAll->SaveToFile(	 fname_new_insurance);
  m->Lines->Add(fname_new_insurance);
  ssXmlAll->Free();

   l_exit:;
  btRun->Enabled=true;





}
//---------------------------------------------------------------------------

void __fastcall TfrmHospRepMain::btRunT5Click(TObject *Sender)
{
  //
    int TNUM=5;
  int persons_added=0;

  String sDirOutcome= iniSet->ReadString("settings","outcoming_dir",ExtractFilePath(Application->ExeName));
  m->Lines->Add("T5 report");
  AnsiString MCOD_="";

  String sql=
  "SELECT p.[IDN_PAC]       \
	  ,p.[NOM_NAP]           \
	  ,p.[DTA_NAP]            \
	  ,p.[FRM_MP]              \
	  ,p.[MCOD_STC]             \
	  ,[DTA_FKT]                 \
	  ,[DTA_END]                  \
	  ,p.[W]                       \
	  ,p.[DR]                       \
	  ,p.[KOD_PFK]                   \
	  ,p.[KOD_PFO]                    \
	  ,[NHISTORY]                      \
	  ,p.[faulty]                       \
	  ,p.[err_codes],                    \
	  t1.MCOD_NAP,                        \
	  f.FILENAME,                          \
	  f.PROC_DT                             \
  FROM [DB_SURVEYS].[dbo].[HOS_T5_PAC] p, [DB_SURVEYS].[dbo].[files_log] f, \
  [DB_SURVEYS].[dbo].HOS_T1_PAC t1 \
  where p.IDN_SCT=f.ID              \
  and p.NOM_NAP=t1.NOM_NAP                               \
  and p.DTA_NAP=t1.DTA_NAP                                \
  and p.MCOD_STC=t1.MCOD_STC                               \
  and f.PROC_DT>=                                           \
  cast(CONVERT(DATE,  GETDATE()-"+(/*cbDay->Text*/IntToStr(NdaysToApplyBack))+") as datetime)  +cast(CONVERT(TIME,  '10:00') as datetime) \
  and f.PROC_DT< \
  cast(CONVERT(DATE,  GETDATE()-0) as datetime)  +cast(CONVERT(TIME,  '"+sTimeEnd+/*23:00*/"') as datetime) \
  order by t1.MCOD_NAP,t1.FAM,t1.IM,t1.OT";

  qOmni->Close();
  qOmni->SQL->Text=sql;//sql_simple;//sql;
  qOmni->Open();

  if (Sender==0) {
	  //goto l_exit;
	  return;
  }

  //////////////
  //////////////
    bool btRun_en=  btRun->Enabled;
  btRun->Enabled=false;

  qOmni->First();
  TStringList *ssXml= new  TStringList;
  TStringList *ssXmlAll = new  TStringList;

  ssXmlAll->Add("<?xml version=\"1.0\" encoding=\"windows-1251\" standalone=\"yes\"?>");
		ssXmlAll->Add("<ZL_LIST>");
		ssXmlAll->Add("<ZGLV>");
		String sDirDropFile = find_or_create_dir_str(sDirOutcome,  "SOGAZ");
		AnsiString sDate= sqlDateNoQuotes(Now() );
		AnsiString fname_new_insurance = sDirDropFile+"T"+IntToStr(TNUM)+"-S-0003"+"-"+sDate+".XML";
		//fname=fname_new;
		ssXmlAll->Add("<FILENAME>"+ExtractFileName(fname_new_insurance)+"</FILENAME>");
		ssXmlAll->Add("<DATA>"+sDate+"</DATA>");
		ssXmlAll->Add("</ZGLV>");


  AnsiString sDir=ExtractFilePath(Application->ExeName)+"\\";

  AnsiString fname="1.xml";
  AnsiString sPerson_= "";

  //int N=0;
  while (1/*!qOmni->Eof*/ )
  try
  {
	  //Caption=IntToStr(N++);
	  //iniSet->WriteInteger("log","n",N);
	  //
	  Application->ProcessMessages();
	  /*
	  m->Lines->Add(
	   qOmni->Fields->Fields[0]->AsAnsiString+" "+
	   qOmni->Fields->Fields[1]->AsAnsiString+" "+
	   qOmni->Fields->Fields[2]->AsAnsiString	  );
	   */
	  //
	  //AnsiString MCOD_NAP=qOmni->FieldByName("MCOD_NAP")   ->AsAnsiString;
	  AnsiString MCOD =qOmni->FieldByName("MCOD_NAP")   ->AsAnsiString;


	  if ((MCOD!=MCOD_) || qOmni->Eof) {

		sPerson_="";
		AnsiString sDate= sqlDateNoQuotes(Now() );
		//AnsiString fname=sDir+"pT1-F-0000-"+sDate+"_"+MCOD_NAP_+".XML";
		//AnsiString fname=sDir+"T1-F-"+MCOD_NAP_+"-"+sDate+".XML";
		//////ssXml->Add("</ZL_LIST>");
		if (MCOD_!="") {
		   ssXml->Add("</ZL_LIST>");
			// file write
			int CODE_MO=StrToFloat( MCOD_ );
			String sDirDropFile = find_or_create_dir(sDirOutcome,  CODE_MO);
			String sFileFullFinal = sDirDropFile+ExtractFileName(fname);
			if ( persons_added>0) {
			  ssXml->SaveToFile(sFileFullFinal);
			  m->Lines->Add(sFileFullFinal) ;
			}
			else
			{
			  m->Lines->Add(sFileFullFinal+" has not been saved due to absence of records") ;
			}
		}
		ssXml->Clear();

		persons_added=0; //counter to decide whether or not we are going to drop this file into a directory
						 // if 0 - we will not, coz they said no need empty files
		ssXml->Add("<?xml version=\"1.0\" encoding=\"windows-1251\" standalone=\"yes\"?>");
		ssXml->Add("<ZL_LIST>");
		ssXml->Add("<ZGLV>");
		AnsiString fname_new = sDir+"T"+IntToStr(TNUM)+"-F-"+MCOD+"-"+sDate+".XML";
		fname=fname_new;
		ssXml->Add("<FILENAME>"+ExtractFileName(fname_new)+"</FILENAME>");
		ssXml->Add("<DATA>"+sDate+"</DATA>");
		ssXml->Add("</ZGLV>");

		MCOD_=MCOD;

		if (qOmni->Eof) {
				  m->Lines->Add("done "+DateTimeToStr(Now())) ;
                  break;
		}
	  }
/*		ssXml->Add("<?xml version=\"1.0\" encoding=\"WINDOWS-1251\" standalone=\"true\"?>");*/

		//ssXml->Add("<REPORT>");
		//ssXml->Add("</REPORT>");
//		ssXml->SaveToFile(sDir+"aaa.xml");


		//////////////////  from Word document
		/*
		AnsiString MCOD_STC=qOmni->FieldByName("MCOD_STC")->AsAnsiString;
TDate DTA_FKT=qOmni->FieldByName("DTA_FKT")->AsDateTime;
//AnsiString TIM_FKT=qOmni->FieldByName("TIM_FKT")->AsAnsiString;
String sTIM_FKT=qOmni->FieldByName("TIM_FKT")->AsString;
		int h=0,min=0;
		readSqlTime_colon(  sTIM_FKT, h,min);
		TTime TIM_FKT= EncodeTime(h,min,0,0);
AnsiString VPOLIS=qOmni->FieldByName("VPOLIS")->AsAnsiString;
AnsiString SPOLIS=qOmni->FieldByName("SPOLIS")->AsAnsiString;
AnsiString NPOLIS=qOmni->FieldByName("NPOLIS")->AsAnsiString;
AnsiString SMOCOD=qOmni->FieldByName("SMOCOD")->AsAnsiString;
AnsiString FAM=qOmni->FieldByName("FAM")->AsAnsiString;
AnsiString IM=qOmni->FieldByName("IM")->AsAnsiString;
AnsiString OT=qOmni->FieldByName("OT")->AsAnsiString;
AnsiString W=qOmni->FieldByName("W")->AsAnsiString;
TDate DR=qOmni->FieldByName("DR")->AsDateTime;
AnsiString KOD_PFK=qOmni->FieldByName("KOD_PFK")->AsAnsiString;
AnsiString KOD_PFO=qOmni->FieldByName("KOD_PFO")->AsAnsiString;
AnsiString NHISTORY=qOmni->FieldByName("NHISTORY")->AsAnsiString;
AnsiString DS=qOmni->FieldByName("DS")->AsAnsiString;
//AnsiString SMOCOD=qOmni->FieldByName("SMOCOD")->AsAnsiString;
		  */




//AnsiString IST_ANL=qOmni->FieldByName("IST_ANL")->AsAnsiString;
//AnsiString ACOD=qOmni->FieldByName("ACOD")->AsAnsiString;
//AnsiString PR_ANL=qOmni->FieldByName("PR_ANL")->AsAnsiString;

/////////////

AnsiString NOM_NAP=qOmni->FieldByName("NOM_NAP")->AsAnsiString;
TDateTime DTA_NAP=qOmni->FieldByName("DTA_NAP")->AsDateTime;

AnsiString FRM_MP=qOmni->FieldByName("FRM_MP")->AsAnsiString;
AnsiString MCOD_STC=qOmni->FieldByName("MCOD_STC")->AsAnsiString;
TDate DTA_FKT=qOmni->FieldByName("DTA_FKT")->AsDateTime;
TDate DTA_END=qOmni->FieldByName("DTA_END")->AsDateTime;
AnsiString W=qOmni->FieldByName("W")->AsAnsiString;
TDate DR=(qOmni->FieldByName("DR")->AsDateTime);
		AnsiString KOD_PFK=qOmni->FieldByName("KOD_PFK")->AsAnsiString;
		AnsiString KOD_PFO=qOmni->FieldByName("KOD_PFO")->AsAnsiString;
AnsiString NHISTORY=qOmni->FieldByName("NHISTORY")->AsAnsiString;

	  


/////////////



		/*
		AnsiString NOM_NAP=qOmni->FieldByName("NOM_NAP")->AsAnsiString;
		TDate DTA_NAP=qOmni->FieldByName("DTA_NAP")->AsDateTime;
		AnsiString FRM_MP=qOmni->FieldByName("FRM_MP")->AsAnsiString;
		AnsiString MCOD_STC=qOmni->FieldByName("MCOD_STC")->AsAnsiString;
		//AnsiString MCOD_NAP=qOmni->FieldByName("MCOD_NAP")->AsAnsiString;
		TDate DTA_FKT=qOmni->FieldByName("DTA_FKT")->AsDateTime;
		String sTIM_FKT=qOmni->FieldByName("TIM_FKT")->AsString;
		int h=0,min=0;
		readSqlTime_colon(  sTIM_FKT, h,min);
		TTime TIM_FKT= EncodeTime(h,min,0,0);

		AnsiString VPOLIS=qOmni->FieldByName("VPOLIS")->AsAnsiString;
		AnsiString SPOLIS=qOmni->FieldByName("SPOLIS")->AsAnsiString;
		AnsiString NPOLIS=qOmni->FieldByName("NPOLIS")->AsAnsiString;
//		AnsiString SMOCOD=qOmni->FieldByName("SMOCOD")->AsAnsiString;
		AnsiString FAM=qOmni->FieldByName("FAM")->AsAnsiString;
		AnsiString IM=qOmni->FieldByName("IM")->AsAnsiString;
		AnsiString OT=qOmni->FieldByName("OT")->AsAnsiString;
		AnsiString W=qOmni->FieldByName("W")->AsAnsiString;
		TDate DR=qOmni->FieldByName("DR")->AsDateTime;
		AnsiString KOD_PFK=qOmni->FieldByName("KOD_PFK")->AsAnsiString;
		AnsiString KOD_PFO=qOmni->FieldByName("KOD_PFO")->AsAnsiString;
		AnsiString NHISTORY=qOmni->FieldByName("NHISTORY")->AsAnsiString;
		AnsiString DS=qOmni->FieldByName("DS")->AsAnsiString;
		AnsiString SMOCOD=qOmni->FieldByName("SMOCOD")->AsAnsiString;
		  */
		////////////////
		/*
		AnsiString NOM_NAP =qOmni->FieldByName("NOM_NAP")->AsAnsiString;
		AnsiString DTA_NAP =sqlDateNoQuotes(qOmni->FieldByName("DTA_NAP")->AsDateTime);
		AnsiString FRM_MP=qOmni->FieldByName("FRM_MP")->AsAnsiString;
		//AnsiString MCOD_NAP=qOmni->FieldByName("MCOD_NAP")->AsAnsiString;
		//AnsiString MCOD_NAP=qOmni->FieldByName("MCOD_NAP")->AsAnsiString;
		AnsiString VPOLIS=qOmni->FieldByName("VPOLIS")->AsAnsiString;
		AnsiString SPOLIS=qOmni->FieldByName("SPOLIS")->AsAnsiString;
		AnsiString NPOLIS=qOmni->FieldByName("NPOLIS")->AsAnsiString;
		AnsiString SMOCOD=qOmni->FieldByName("SMOCOD")->AsAnsiString;
		AnsiString FAM=qOmni->FieldByName("FAM")->AsAnsiString;
		AnsiString IM=qOmni->FieldByName("IM")->AsAnsiString;
		AnsiString OT=qOmni->FieldByName("OT")->AsAnsiString;
		AnsiString W=qOmni->FieldByName("W")->AsAnsiString;
		AnsiString DR=sqlDateNoQuotes(qOmni->FieldByName("DR")->AsDateTime);
		AnsiString TLF=qOmni->FieldByName("TLF")->AsAnsiString;
		AnsiString DS=qOmni->FieldByName("DS")->AsAnsiString;
		AnsiString KOD_PFK=qOmni->FieldByName("KOD_PFK")->AsAnsiString;
		AnsiString KOD_PFO=qOmni->FieldByName("KOD_PFO")->AsAnsiString;
		AnsiString KOD_DÑT=qOmni->FieldByName("KOD_DCT")->AsAnsiString; //KOD_DCT probaly has cyrillic symb
		AnsiString DTA_PLN=sqlDateNoQuotes(qOmni->FieldByName("DTA_PLN")->AsDateTime);
		AnsiString USL_OK=qOmni->FieldByName("USL_OK")->AsAnsiString;
		  */
		int faulty   =  qOmni->FieldByName("faulty")->AsInteger;
		int	IDN_PAC  =  qOmni->FieldByName("IDN_PAC")->AsInteger;
		AnsiString FILENAME    =  qOmni->FieldByName("FILENAME")->AsString;

		AnsiString sPerson= "\r\n<ZAP>  \
\r\n<NOM_NAP>"+NOM_NAP+"</NOM_NAP>            \
\r\n<DTA_NAP>"+sqlDateNoQuotes(DTA_NAP)+"</DTA_NAP>            \
\r\n<FRM_MP>"+   FRM_MP         +  "     </FRM_MP>   \
\r\n<MCOD_STC>"+   MCOD_STC      +  "   </MCOD_STC>   \
\r\n<DTA_FKT>"+    sqlDateNoQuotes(DTA_FKT  )+"   </DTA_FKT> \
\r\n<DTA_END>"+    sqlDateNoQuotes( DTA_END  ) +" </DTA_END> \
\r\n<W>"+       W                              +"  </W>   \
\r\n<DR>"+   sqlDateNoQuotes(DR               )  +"   </DR> \
\r\n<KOD_PFK>"+   KOD_PFK                        +"  </KOD_PFK> \
\r\n<KOD_PFO>"+     KOD_PFO                       +"  </KOD_PFO> \
\r\n<NHISTORY>"+     NHISTORY                      +"   </NHISTORY> \
\r\n</ZAP>\r\n";

		/*        NOM_NAP
DTA_NAP
FRM_MP
MCOD_STC
MCOD_NAP
DTA_FKT
TIM_FKT
VPOLIS
SPOLIS
NPOLIS
SMOCOD
FAM
IM
OT
W
DR
KOD_PFK
KOD_PFO
NHISTORY
DS

		*/

	   //<!-- Write your comment-->

	   //@@@force faulty =0
	   faulty=0;

	   if (sPerson_!=sPerson) {

		if (faulty==0) {


		 sPerson_=sPerson;
		 ssXml->Add(sPerson+"\r\n"+	"<!-- "+IntToStr(IDN_PAC)+" "+FILENAME+" -->  " );

		 ssXmlAll->Add(sPerson+"\r\n"+	"<!-- "+IntToStr(IDN_PAC)+" "+FILENAME+" -->  ");

		 persons_added++;
		 //m->Lines->Add(sPerson);
		}

	   }

		/////////////////

	//

	  qOmni->Next();
  }
  catch(...){};

  ssXml->Free();
  ////////////////


  ssXmlAll->Add("</ZL_LIST>");
  ssXmlAll->SaveToFile(	 fname_new_insurance);
  m->Lines->Add(fname_new_insurance);
  ssXmlAll->Free();

   l_exit:;
  btRun->Enabled=true;







}
//---------------------------------------------------------------------------

void __fastcall TfrmHospRepMain::btRunT6Click(TObject *Sender)
{

  int TNUM=6;
  int persons_added=0;

  String sDirOutcome= iniSet->ReadString("settings","outcoming_dir",ExtractFilePath(Application->ExeName));
  add_slash(   sDirOutcome);

  m->Lines->Add("T6 report");
  AnsiString MCOD_="";

  String sql=
  "SELECT  f.FILENAME, \
	   [IDN_SCT]        \
	  ,[IDN_PAC]         \
	  ,[DTA_RAB]   \
	  ,[MCOD_STC]   \
	  ,[KOD_PFK]     \
	  ,[KOL_PAC]      \
	  ,[KOL_IN]        \
	  ,[KOL_OUT]        \
	  ,[KOL_PLN]   \
	  ,[KOL_PUS]    \
	  ,[KOL_PUS_M]   \
	  ,[KOL_PUS_J]    \
	  ,[KOL_PUS_D]     \
	  ,[VKOL_FKT]       \
	  ,[VUED_FKT]        \
	  ,[faulty]           \
	  ,[err_codes]         \
	  , f.CODE_MO           \
	  ,[KOD_PFO]     \
  FROM [DB_SURVEYS].[dbo].[HOS_T6_PAC] t6, [DB_SURVEYS].[dbo].files_log f \
  where t6.IDN_SCT=f.ID and f.FILE_TYPE=16 \
  and f.PROC_DT>= \
  cast(CONVERT(DATE,  GETDATE()-"+IntToStr(/*cbDay->Text*/NdaysToApplyBack)+") as datetime)  +cast(CONVERT(TIME,  '10:00') as datetime) \
  and f.PROC_DT< \
  cast(CONVERT(DATE,  GETDATE()-0) as datetime)  +cast(CONVERT(TIME,  '"+sTimeEnd+/*23:00*/"') as datetime) \
  order by f.ID,f.CODE_MO,t6.MCOD_STC,t6.KOD_PFK";

  ////////////

  qOmni->Close();
  qOmni->SQL->Text=sql;//sql_simple;//sql;
  qOmni->Open();

  if (Sender==0) {
	  //goto l_exit;
	  return;
  }

  //////////////
  //////////////
    bool btRun_en=  btRun->Enabled;
  btRun->Enabled=false;

  qOmni->First();
  //TStringList *ssXml= new  TStringList;
  TStringList *ssXmlAll = new  TStringList;

  ssXmlAll->Add("<?xml version=\"1.0\" encoding=\"windows-1251\" standalone=\"yes\"?>"); //0
  ssXmlAll->Add("<ZL_LIST>");//1
  ssXmlAll->Add("<ZGLV>");//2

  String sDirDropFile = find_or_create_dir_str(sDirOutcome,  "SOGAZ");
  AnsiString sDate= sqlDateNoQuotes(Now() );
  AnsiString fname_new_insurance = sDirDropFile+"T"+IntToStr(TNUM)+"-S-0003"+"-"+sDate+".XML";
		//fname=fname_new;
  ssXmlAll->Add("<FILENAME>"); //3
  ssXmlAll->Add( ExtractFileName(fname_new_insurance)); //4 - file name
  ssXmlAll->Add("</FILENAME>");//5
  ssXmlAll->Add("<DATA>"+sDate+"</DATA>");//6
  ssXmlAll->Add("</ZGLV>");//7


  AnsiString sDir=ExtractFilePath(Application->ExeName)+"\\";

  AnsiString fname="1.xml";
  AnsiString sPerson_= "";

  //int N=0;
  while (!qOmni->Eof  )
  try
  {

	  Application->ProcessMessages();

//	  AnsiString MCOD = "0000";//qOmni->FieldByName("MCOD_STC")   ->AsAnsiString;






/////////////

TDateTime DTA_RAB   =qOmni->FieldByName("DTA_RAB")->AsDateTime;
AnsiString MCOD_STC =qOmni->FieldByName("MCOD_STC")->AsAnsiString;
AnsiString KOD_PFK  =qOmni->FieldByName("KOD_PFK")->AsAnsiString;
AnsiString KOD_PFO  =qOmni->FieldByName("KOD_PFO")->AsAnsiString;
AnsiString KOL_PAC  =qOmni->FieldByName("KOL_PAC")->AsAnsiString;
AnsiString KOL_IN   =qOmni->FieldByName("KOL_IN")->AsAnsiString;
AnsiString KOL_OUT  =qOmni->FieldByName("KOL_OUT")->AsAnsiString;
AnsiString KOL_PLN  =qOmni->FieldByName("KOL_PLN")->AsAnsiString;
AnsiString KOL_PUS  =qOmni->FieldByName("KOL_PUS")->AsAnsiString;
AnsiString KOL_PUS_M =qOmni->FieldByName("KOL_PUS_M")->AsAnsiString;
AnsiString KOL_PUS_J =qOmni->FieldByName("KOL_PUS_J")->AsAnsiString;
AnsiString KOL_PUS_D =qOmni->FieldByName("KOL_PUS_D")->AsAnsiString;
AnsiString VKOL_FKT  =qOmni->FieldByName("VKOL_FKT")->AsAnsiString;
AnsiString VUED_FKT  =qOmni->FieldByName("VUED_FKT")->AsAnsiString;

  /*
AnsiString NOM_NAP=qOmni->FieldByName("NOM_NAP")->AsAnsiString;
TDateTime DTA_NAP=qOmni->FieldByName("DTA_NAP")->AsDateTime;

AnsiString FRM_MP=qOmni->FieldByName("FRM_MP")->AsAnsiString;
AnsiString MCOD_STC=qOmni->FieldByName("MCOD_STC")->AsAnsiString;
TDate DTA_FKT=qOmni->FieldByName("DTA_FKT")->AsDateTime;
TDate DTA_END=qOmni->FieldByName("DTA_END")->AsDateTime;
AnsiString W=qOmni->FieldByName("W")->AsAnsiString;
TDate DR=(qOmni->FieldByName("DR")->AsDateTime);
		AnsiString KOD_PFK=qOmni->FieldByName("KOD_PFK")->AsAnsiString;
		AnsiString KOD_PFO=qOmni->FieldByName("KOD_PFO")->AsAnsiString;
AnsiString NHISTORY=qOmni->FieldByName("NHISTORY")->AsAnsiString;
	*/



/////////////




		////////////////

		int faulty   =  qOmni->FieldByName("faulty")->AsInteger;
		int	IDN_PAC  =  qOmni->FieldByName("IDN_PAC")->AsInteger;
		AnsiString FILENAME    =  qOmni->FieldByName("FILENAME")->AsString;

		AnsiString sPerson= "\r\n<ZAP>  \
\r\n<DTA_RAB>"+sqlDateNoQuotes(DTA_RAB)+"</DTA_RAB>            \
\r\n<MCOD_STC>"+(MCOD_STC)+"</MCOD_STC>            \
\r\n<KOD_PFK>"+   KOD_PFK         +  "     </KOD_PFK>   \
\r\n<KOD_PFO>"+   KOD_PFO      +  "   </KOD_PFO>   \
\r\n<KOL_PAC>"+    (KOL_PAC  )+"   </KOL_PAC> \
\r\n<KOL_IN>"+    ( KOL_IN  ) +" </KOL_IN> \
\r\n<KOL_OUT>"+       KOL_OUT                              +"  </KOL_OUT>   \
\r\n<KOL_PLN>"+   (KOL_PLN               )  +"   </KOL_PLN> \
\r\n<KOL_PUS>"+   KOL_PUS                        +"  </KOL_PUS> \
\r\n<KOL_PUS_M>"+     KOL_PUS_M                       +"  </KOL_PUS_M> \
\r\n<KOL_PUS_J>"+     KOL_PUS_J                      +"   </KOL_PUS_J> \
\r\n<KOL_PUS_D>"+   KOL_PUS_D                        +"  </KOL_PUS_D> \
\r\n<VKOL_FKT>"+     VKOL_FKT                       +"  </VKOL_FKT> \
\r\n<VUED_FKT>"+     VUED_FKT                      +"   </VUED_FKT> \
\r\n</ZAP>\r\n";



	   //<!-- Write your comment-->

	   //@@@force faulty =0
	   faulty=0;

	   if (sPerson_!=sPerson) {

		if (faulty==0) {


		 sPerson_=sPerson;
//		 ssXml->Add(sPerson+"\r\n"+	"<!-- "+IntToStr(IDN_PAC)+" "+FILENAME+" -->  " );

		 ssXmlAll->Add(sPerson+"\r\n"+	"<!-- "+IntToStr(IDN_PAC)+" "+FILENAME+" -->  ");

		 persons_added++;
		 //m->Lines->Add(sPerson);
		}

	   }

		/////////////////

	//

	  qOmni->Next();
  }
  catch(...){};

//  ssXml->Free();
  ////////////////


  ssXmlAll->Add("</ZL_LIST>");
  ssXmlAll->SaveToFile(	 fname_new_insurance);
  m->Lines->Add(fname_new_insurance);


  //now copy the same file to HFs
  //
  TStringList* ssDirHF=new TStringList;
  SearchInDir_Subdirs( sDirOutcome,"*.*", ssDirHF);
  m->Lines->Add("DIRECTORIES>>>>>>>>>>>>>>>>>> "+sDirOutcome  );
  m->Lines->AddStrings(ssDirHF);


  for (int i = 0; i < ssDirHF->Count; i++) {
	//
	String sdir = ssDirHF->Strings[i].Trim();
	String scode="";
	for (int k = 1; k < sdir.Length(); k++) {
	  if (sdir[k]>='0' && sdir[k]<='9' ) {
		scode+=sdir[k];
	  }  else break;
	}
	int icode=StrToIntDef(scode,-1);
	if (icode!=-1) {
	  //
	  String scode_use = FormatFloat("0000",icode);
	  AnsiString fname_new = "T"+IntToStr(TNUM)+"-F-"+scode_use+"-"+sDate+".XML";
	  AnsiString  fname_new_full=sDirOutcome+ sdir+"\\"+fname_new;
	  m->Lines->Add(fname_new_full);
	  TStringList *ssXmlAll_copy = new  TStringList;  //use copy coz in same cases filename pasting can change the number of lines
	  ssXmlAll_copy->AddStrings(ssXmlAll);
	  if (ssXmlAll_copy->Count>4) {
		   ssXmlAll_copy->Strings[4]=ExtractFileName(fname_new);
	  }
	  ssXmlAll_copy->SaveToFile(fname_new_full);
	  ssXmlAll_copy->Free();
	}
  }


  ssXmlAll->Free();
  l_exit:;
  btRun->Enabled=true;








}
//---------------------------------------------------------------------------

