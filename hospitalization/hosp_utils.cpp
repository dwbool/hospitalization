//---------------------------------------------------------------------------

#pragma hdrstop

#include <vcl.h>
#include <tchar.h>
#include <XMLDoc.hpp>
#include <XMLIntf.hpp>
#include <ComObj.hpp>
#include <stdio.h>
#include <vector>
//#include <thread>
#include <string>

#include <dateutils.hpp>

#include "hosp_utils.h"
#include "file_dir_funcs.hpp"


//---------------------------------------------------------------------------
#pragma package(smart_init)


int scopeIdentity(TADOQuery * q)
{
//try{
  String sql= "select isnull(scope_identity(),0) as sc";
  q->Close();
  q->SQL->Text=sql;
  q->Open();
  int x= q->Fields->Fields[0]->AsInteger;
  //q->FieldValues["sc"];
  return x;
//}catch(Exception &e)
/*{
  return 0;
} */
}


int identCurrent(TADOQuery * q, String tab_name)
{
//try{
  String sql= "select isnull(IDENT_CURRENT('"+tab_name+"'),0) as sc";
  q->Close();
  q->SQL->Text=sql;
  q->Open();
  int x= q->Fields->Fields[0]->AsInteger;
  //q->FieldValues["sc"];
  return x;
//}catch(Exception &e)
/*{
  return 0;
} */
}






/*
//T1-M-1801-2020-05-21.xml

-<ZL_LIST>

-<ZGLV>
 <DATA>2020-05-22</DATA>
 <FILENAME>T1-M-1801-2020-05-21.xml</FILENAME>
</ZGLV>

-<ZAP>----------------
<NOM_NAP>1801001380</NOM_NAP>
<DTA_NAP>2020-05-21</DTA_NAP>
<FRM_MP>3</FRM_MP>
<MCOD_NAP>1801</MCOD_NAP>
<MCOD_STC>1801</MCOD_STC>
<VPOLIS>3</VPOLIS>
<SPOLIS/>
<NPOLIS>3573850895000262</NPOLIS>
<SMO_OGRN>1027739008440</SMO_OGRN>
<ST_OKATO/>
<FAM>ВЫДРИНА</FAM>
<IM>ПОЛИНА</IM>
<OT>ПАВЛИНОВНА</OT>
<W>2</W>
<DR>1941-06-04</DR>
<TLF/>
<DS>I48.0</DS>
<KOD_PFK>82</KOD_PFK>
<KOD_PFO>71</KOD_PFO>
<KOD_DCT>065-594-238 86</KOD_DCT>
<DTA_PLN>2020-05-25</DTA_PLN>
<USL_OK>1</USL_OK>
</ZAP>
*/


struct TErrorDefinition{
	  int id, err_code;
	  String defined_as;
	  String date_begin;
	  String date_end;
	  String field;
	  String operator1;
	  String arg1;
	  String operator2;
	  String arg2;
	  String comment   ;
};


void find_error_definition(TADOQuery *q, String field, int err_from, int err_to, std::vector<TErrorDefinition> & defs )
{
  String sql="select [ID] \
	  ,[err_code]         \
	  ,[defined_as]       \
	  ,[date_begin]       \
	  ,isNull([date_end],'2000-1-1') as date_end        \
	  ,isNull([field],'') as field            \
	  ,isNull([operator1],'') as operator1        \
	  ,isNull([arg1],'') as arg1             \
	  ,isNull([operator2],'') as operator2        \
	  ,isNull([arg2],'') as arg2             \
	  ,isNull([comment],'') as comment from [DB_SURVEYS].[dbo].[hosp_error_define] where field='"
	  +field+"' and err_code between " +IntToStr(err_from)+ " and "+IntToStr( err_to);
  //
  q->Close();
  q->SQL->Text=sql;
  q->Open();

  while (!q->Eof)
  {
  TErrorDefinition def;
  def.id= q->FieldValues["id"];
  def.err_code= q->FieldValues["err_code"];
  def.defined_as= q->FieldValues["defined_as"];
  def.date_begin= q->FieldValues["date_begin"];
  def.date_end= q->FieldValues["date_end"];
  def.field= q->FieldValues["field"];
  def.operator1= q->FieldValues["operator1"];
  def.arg1= q->FieldValues["arg1"];
  def.operator2= q->FieldValues["operator2"];
  def.arg2= q->FieldValues["arg2"];
  def.comment= q->FieldValues["comment"];

  defs.push_back(def);
  q->Next();
  }
}



bool all_required_received(std::vector<String> svReqired, std::vector<String> svReceived, std::vector<String> & svMissing, std::vector<String> & svDuplicated )
{
	svMissing.clear();
    svDuplicated.clear();
	for (int i = 0; i < svReqired.size(); i++) {
		String req=  svReqired[i].UpperCase();
		int found=0;
		for (int k = 0; k < svReceived.size(); k++) {
			String s =  svReceived[k].UpperCase();
			if (s==req) {
				found++;
			}
		}
		if (!found) {
			  svMissing.push_back(req);
		} else
		if (found>1) {
			 svDuplicated.push_back(req);
		}
	}
	bool res=   svMissing.size()==0;
	return res;
}



int process_hosp_xml_t1(String fn, THospFileInfo_T1 &file_info, std::vector<TPersonInfoHosp_T1> &persons, TStrings * ssLog , TADOQuery*q)
{
 file_info.error_code_general=0;
 file_info.ORIGINAL_FILENAME=ExtractFileName(fn);
 int error_code=0;

 try{
//  memset(&file_info,0,sizeof(file_info));

  int flags_summary=0;
  TXMLDocument *x= new TXMLDocument(Application);

  x->LoadFromFile(fn);
  IXMLNode *ndRoot = x->DocumentElement;
  int cnt=  ndRoot->ChildNodes->Count;
  if (ndRoot->NodeName.LowerCase()!="zl_list" ) {
	   file_info.error_code_general=ERROR_GENERAL_ROOT_MISSING;
  }

  ssLog->Add("Line 1002 ");

  unsigned char struct_flags=0;
  unsigned char hdr_flags=0;
  unsigned long pers_flags=0;


  /// checking
  std::vector<String>  svRequired ;
  ///  std::vector<int> ivRequiredErrorCodes;
  std::map<String,int> mpErrorsRequired;
  getRequiredFields(q, 1, svRequired,  mpErrorsRequired);
  file_info.mpErrorsRequired=  mpErrorsRequired;
  file_info.svRequired= svRequired;
   //ivRequiredErrorCodes);



  //@@
  std::vector<TReferenceInfo>  vref_inf;
  getReferenceCheckings( q,  vref_inf /*,"'ref','from'"*/);
  //////////// checking^


  for (int i = 0; i < cnt; i++) {
     Application->ProcessMessages() ;
	 IXMLNode *ndChild=	 ndRoot-> ChildNodes->Get(i);

	 if (ndChild->NodeName.LowerCase()=="zglv" ) {
		 if ( struct_flags & 1<<0 ) {

			 ssLog->Add("ERROR: More than one header");
			 if (error_code==0) error_code=5;

		 }
		 struct_flags|=1<<0;


		 file_info.dt_date=1;
		 for (int k = 0; k < ndChild->ChildNodes->Count; k++) {
			//
			IXMLNode * ndStub =  ndChild-> ChildNodes->Get(k);

			if (ndStub->NodeName.UpperCase()=="FILENAME")
			{
			  hdr_flags|= 1<<0;
			  file_info.FILENAME=ndStub->Text.Trim();
			  ssLog->Add("FILENAME="+file_info.FILENAME );

			  if ( LowerCase( file_info.FILENAME.Trim())!=Trim(LowerCase(ExtractFileName(fn))))
			  {
				   file_info.error_code_general=ERROR_FILENAME_NOT_MATCH;
			  }

			}
			if (ndStub->NodeName.UpperCase()=="DATA")
			{
			  hdr_flags|= 1<<1;
			  int y;
			  int m;
			  int d;
			  readSqlDate(ndStub->Text,y,m,d);
			  try{
			   TDateTime dtStub= EncodeDate(y,m,d);
			   file_info.dt_date=dtStub;
			  }catch(...){};
			  ssLog->Add("DATA="+DateToStr(file_info.dt_date) );
			}

		 }

		 if (hdr_flags != 0x0F) {
			 // header does not contain all the required fields
			 if ( !( hdr_flags & 1<<0 ) ) {
				ssLog->Add("FILENAME is missing");
				if (error_code==0) error_code=1;
			 }
			 if ( !( hdr_flags & 1<<1 ) ) {
				ssLog->Add("DATA (creation date!) is missing");
				if (error_code==0) error_code=2;
			 }
		
		 }

	 }

	//////
	 if (ndChild->NodeName.LowerCase()=="zap" ) {
	     ssLog->Add("Line 1002.1 zap ");
		 struct_flags|=1<<1;
		 TPersonInfoHosp_T1 pinf;
		 memset(&pinf,0,sizeof(pinf));

		 std::vector<String>  svReceived;

		 for (int k = 0; k < ndChild->ChildNodes->Count; k++) {
			//
			std::vector<TErrorDefinition>  defs100;

			IXMLNode * ndStub =  ndChild-> ChildNodes->Get(k);

			svReceived.push_back(ndStub->NodeName.UpperCase())  ; //@@@checking
			//String received_value=ndStub->Text.Trim();  //@@@
			//pinf.nvvReceived.push_back();
			TNameValueInfo nv_rec;
			nv_rec.name=  ndStub->NodeName.UpperCase().Trim();  //@@@
			nv_rec.value=  ndStub->Text.Trim();                 //@@@
			nv_rec.val_type=0;//@@@

			if (ndStub->NodeName.UpperCase()=="NOM_NAP")
			{
			  pers_flags|=1<<0;
			  pinf.NOM_NAP= /*StrToIntDef*/(  ndStub->Text.Trim());
			  ssLog->Add(("NOM_NAP "+/*IntToStr*/ str_limit_len_quot(pinf.NOM_NAP,16)));

			  find_error_definition(q,ndStub->NodeName.UpperCase(),100,199,defs100);

			}
			if (ndStub->NodeName.UpperCase()=="DTA_NAP")
			{
			  pers_flags|=1<<1;
			  int y;
			  int m;
			  int d;
			  readSqlDate(ndStub->Text,y,m,d);
			  try{
			   TDateTime dtStub= EncodeDate(y,m,d);
			   pinf.DTA_NAP=dtStub;
			  }catch(...){
               pinf.DTA_NAP=0;
			  };
			  //pinf.DTA_NAP =   ndStub->Text.Trim() ;
			  nv_rec.val_type=NameValueInfo_TYPE_DATETIME;
			  ssLog->Add("DTA_NAP "+ DateToStr(pinf.DTA_NAP) );

			}
			if (ndStub->NodeName.UpperCase()=="FRM_MP")
			{
			  pers_flags|=1<<2;
			  pinf.FRM_MP= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("FRM_MP "+IntToStr(pinf.FRM_MP)));
			}
			//MCOD_NAP
			if (ndStub->NodeName.UpperCase()=="MCOD_NAP")
			{
			  pers_flags|=1<<3;
			  pinf.MCOD_NAP= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("MCOD_NAP "+IntToStr(pinf.MCOD_NAP)));
			}
			//MCOD_STC
			if (ndStub->NodeName.UpperCase()=="MCOD_STC")
			{
			  pers_flags|=1<<4;
			  pinf.MCOD_STC= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("MCOD_STC "+IntToStr(pinf.MCOD_STC)));
			}
			//VPOLIS
			if (ndStub->NodeName.UpperCase()=="VPOLIS")
			{
			  pers_flags|=1<<5;
			  pinf.VPOLIS= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("VPOLIS "+IntToStr(pinf.VPOLIS)));
			}
			//SPOLIS
			if (ndStub->NodeName.UpperCase()=="SPOLIS")
			{
			  pers_flags|=1<<6;
			  pinf.SPOLIS= (  ndStub->Text.Trim() );
			  ssLog->Add(("SPOLIS "+(pinf.SPOLIS)));
			}
			//NPOLIS
			if (ndStub->NodeName.UpperCase()=="NPOLIS")
			{
			  pers_flags|=1<<7;
			  pinf.NPOLIS= (  ndStub->Text.Trim());
			  ssLog->Add(("NPOLIS "+(pinf.NPOLIS)));
			}
			//SMO_OGRN
			/*   // @@deleted in new protocol
			if (ndStub->NodeName.UpperCase()=="SMO_OGRN")
			{
			  pers_flags|=1<<8;
			  pinf.SMO_OGRN= (  ndStub->Text.Trim());
			  ssLog->Add(("SMO_OGRN "+(pinf.SMO_OGRN)));
			} */
			//ST_OKATO
			/*        // @@deleted in new protocol
			if (ndStub->NodeName.UpperCase()=="ST_OKATO")
			{
			  pers_flags|=1<<9;
			  pinf.ST_OKATO= (  ndStub->Text.Trim());
			  ssLog->Add(("ST_OKATO "+(pinf.ST_OKATO)));
			} */
			if (ndStub->NodeName.UpperCase()=="FAM")
			{
			  pers_flags|=1<<8;
			  pinf.FAM=   ndStub->Text.Trim() ;
			  ssLog->Add("FAM "+pinf.FAM);
			}
			if (ndStub->NodeName.UpperCase()=="IM")
			{
			  pers_flags|=1<<9;
			  pinf.IM=   ndStub->Text.Trim() ;
			  ssLog->Add("IM "+pinf.IM);
			}
			if (ndStub->NodeName.UpperCase()=="OT")
			{
			  pers_flags|=1<<10;
			  pinf.OT=   ndStub->Text.Trim() ;
			  ssLog->Add("OT "+pinf.OT);
			}
			//W
			if (ndStub->NodeName.UpperCase()=="W")
			{
			  pers_flags|=1<<11;
			  pinf.W = StrToIntDef(  ndStub->Text.Trim(),0) ;
			  ssLog->Add("W sex "+IntToStr(pinf.W));
			}
			//DR
			if (ndStub->NodeName.UpperCase()=="DR")
			{
			  pers_flags|=1<<12;
			  int y,m,d;
			  readSqlDate(ndStub->Text,y,m,d);
			  try{
			   pinf.DR= EncodeDate(y,m,d);
			  }catch(...){};
			  ssLog->Add("DR : birth_date "+DateTimeToStr(pinf.DR));
			}
			//TLF
			if (ndStub->NodeName.UpperCase()=="TLF")
			{
			  pers_flags|=1<<13;
			  pinf.TLF=   ndStub->Text.Trim() ;
			  ssLog->Add("TLF "+pinf.TLF);
			}
			//DS
			if (ndStub->NodeName.UpperCase()=="DS")
			{
			  pers_flags|=1<<14;
			  pinf.DS = (  ndStub->Text.Trim() ) ;
			  ssLog->Add("diagnosis "+(pinf.DS));
			}
			//KOD_PFK
			if (ndStub->NodeName.UpperCase()=="KOD_PFK")
			{
			  pers_flags|=1<<15;
			  pinf.KOD_PFK= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("KOD_PFK "+IntToStr(pinf.KOD_PFK)));
			}
			//KOD_PFO
			if (ndStub->NodeName.UpperCase()=="KOD_PFO")
			{
			  pers_flags|=1<<16;
			  pinf.KOD_PFO= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("KOD_PFO "+IntToStr(pinf.KOD_PFO)));
			}
			//KOD_DCT
			if (ndStub->NodeName.UpperCase()=="KOD_DCT")
			{
			  pers_flags|=1<<17;
			  pinf.KOD_DCT = StrToIntDef(  ndStub->Text.Trim() ,-1) ;
			  ssLog->Add("KOD_DCT "+(pinf.KOD_DCT));
			}
			//DTA_PLN
			if (ndStub->NodeName.UpperCase()=="DTA_PLN")
			{
			  pers_flags|=1<<18;
			  int y,m,d;
			  readSqlDate(ndStub->Text,y,m,d);
			  try{
			   pinf.DTA_PLN= EncodeDate(y,m,d);
			  }catch(...){};
			  ssLog->Add("DTA_PLN :  "+DateTimeToStr(pinf.DTA_PLN));
			}
			//USL_OK
			if (ndStub->NodeName.UpperCase()=="USL_OK")
			{
			  pers_flags|=1<<19;
			  pinf.USL_OK= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("USL_OK "+IntToStr(pinf.USL_OK)));
			}

			if (ndStub->NodeName.UpperCase()=="SMOCOD")
			{
			  pers_flags|=1<<20;
			  pinf.SMOCOD= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("SMOCOD "+IntToStr(pinf.USL_OK)));
			}





			//ssLog->Add("Line 4 ");
            pinf.nvvReceived.push_back( nv_rec );//@@@name value pair with type
		 }
		 //ssLog->Add("Line 5 ");
		 ssLog->Add("");
		 pinf.pers_flags=pers_flags;
		 //ssLog->Add("Line 6 ");
		 if ( pers_flags != 0x0FFFFF ) {
			  //ssLog->Add("Line 7 ");
			  ssLog->Add("ERROR: wrong person record");
			  if (error_code==0) error_code=7;
		 }
		 //ssLog->Add("Line 8 ");
		 //persons.push_back(pinf);
		 //ssLog->Add("Line 9 ");

		 /////////////////////*********    for 1 record - finished
		 std::vector<String >    svMissing, svDuplicated;
		 all_required_received(svRequired, svReceived , svMissing, svDuplicated );
		 //pinf.svRequired= svRequired;
		 pinf.svReceived=   svReceived;
		 pinf.svMissing=    svMissing;
		 pinf.svDuplicated= svDuplicated;
		 //pinf.ivErrorCodeRequired =ivRequiredErrorCodes;
		 /////////////////////

		 persons.push_back(pinf);


	 }
	 //ssLog->Add("Line 10 ");


  }

  //ssLog->Add("Line 11 ");
  if ( !(struct_flags & 1<<0 )) {
	  ssLog->Add("ERROR: no header");
	  if (error_code==0) error_code=6;
	  if (!file_info.error_code_general) file_info.error_code_general=ERROR_GENERAL_NO_HEADER;//@@error gen
  }
  if ( !(struct_flags & 1<<1 )) {
	  ssLog->Add("ERROR: not a single record found");
	  if (error_code==0) error_code=8;
	  if (!file_info.error_code_general) file_info.error_code_general=ERROR_GENERAL_NO_RECORDS;//@@error gen
  }
  file_info.rec_emount= persons.size();
  flags_summary = pers_flags |  (hdr_flags<<16) | (struct_flags<<24);

  //ssLog->Add("Line 12 ");
  x->Free();
  //ssLog->Add("Line 13 ");

 }catch(Exception &e)
 {
  //
  file_info.error_code_general=ERROR_GENERAL_PARSER_EXCEPTION;   //@@
  file_info.error_desc_general=e.Message;

 }
  return error_code;
}





int process_hosp_xml_t2(String fn, THospFileInfo_T1 &file_info, std::vector<TPersonInfoHosp_T2> &persons, TStrings * ssLog , TADOQuery*q)
{
  int error_code=0;
  int flags_summary=0;
   file_info.error_code_general=0;
 file_info.ORIGINAL_FILENAME=ExtractFileName(fn);
  try{
  TXMLDocument *x= new TXMLDocument(Application);

  x->LoadFromFile(fn);
  IXMLNode *ndRoot = x->DocumentElement;
  int cnt=  ndRoot->ChildNodes->Count;
  if (ndRoot->NodeName.LowerCase()!="zl_list" ) {
	   file_info.error_code_general=ERROR_GENERAL_ROOT_MISSING;
  }

  unsigned char struct_flags=0;
  unsigned char hdr_flags=0;
  unsigned long pers_flags=0;


  /// checking
  std::vector<String>  svRequired ;
  ///  std::vector<int> ivRequiredErrorCodes;
  std::map<String,int> mpErrorsRequired;
  getRequiredFields(q, 2, svRequired,  mpErrorsRequired);
  file_info.mpErrorsRequired=  mpErrorsRequired;
  file_info.svRequired= svRequired;
   //ivRequiredErrorCodes);

  //@@
  std::vector<TReferenceInfo>  vref_inf;
  getReferenceCheckings( q,  vref_inf /*,"'ref','from','<=','>='"*/ );
  //////////// checking^

  for (int i = 0; i < cnt; i++) {
	 Application->ProcessMessages() ;
	 IXMLNode *ndChild=	 ndRoot-> ChildNodes->Get(i);

	 if (ndChild->NodeName.LowerCase()=="zglv" ) {
		 if ( struct_flags & 1<<0 ) {

			 ssLog->Add("ERROR: More than one header");
			 if (error_code==0) error_code=5;

		 }
		 struct_flags|=1<<0;

		 file_info.dt_date=1;
		 for (int k = 0; k < ndChild->ChildNodes->Count; k++) {
			//
			IXMLNode * ndStub =  ndChild-> ChildNodes->Get(k);
			if (ndStub->NodeName.UpperCase()=="FILENAME")
			{
			  hdr_flags|= 1<<0;
			  file_info.FILENAME=ndStub->Text.Trim();
			  ssLog->Add("FILENAME="+file_info.FILENAME );
			  if ( LowerCase( file_info.FILENAME.Trim())!=Trim(LowerCase(ExtractFileName(fn))))
			  {
				   file_info.error_code_general=ERROR_FILENAME_NOT_MATCH;
			  }
			}
			if (ndStub->NodeName.UpperCase()=="DATA")
			{
			  hdr_flags|= 1<<1;
			  int y;
			  int m;
			  int d;
			  readSqlDate(ndStub->Text,y,m,d);
			  try{
			   TDateTime dtStub= EncodeDate(y,m,d);
			   file_info.dt_date=dtStub;
			  }catch(...){};
			  ssLog->Add("DATA="+DateToStr(file_info.dt_date) );
			}

		 }

		 if (hdr_flags != 0x0F) {
			 // header does not contain all the required fields
			 if ( !( hdr_flags & 1<<0 ) ) {
				ssLog->Add("FILENAME is missing");
				if (error_code==0) error_code=1;
			 }
			 if ( !( hdr_flags & 1<<1 ) ) {
				ssLog->Add("DATA (creation date!) is missing");
				if (error_code==0) error_code=2;
			 }

		 }

	 }

	//////
	 if (ndChild->NodeName.LowerCase()=="zap" ) {
		 struct_flags|=1<<1;
		 TPersonInfoHosp_T2 pinf;
		 memset(&pinf,0,sizeof(pinf));

		 for (int k = 0; k < ndChild->ChildNodes->Count; k++) {
			//


			IXMLNode * ndStub =  ndChild-> ChildNodes->Get(k);

            TNameValueInfo nv_rec;
			nv_rec.name=  ndStub->NodeName.UpperCase().Trim();  //@@@
			nv_rec.value=  ndStub->Text.Trim();                 //@@@
			nv_rec.val_type=0;//@@@



			pinf.svReceived.push_back(ndStub->NodeName.UpperCase())  ; //@@@checking
			//String received_value=ndStub->Text.Trim();  //@@@
			//pinf.nvvReceived.push_back();
	


			if (ndStub->NodeName.UpperCase()=="NOM_NAP")
			{
			  pers_flags|=1<<0;
			  pinf.NOM_NAP= /*StrToIntDef*/(  ndStub->Text.Trim());
			  ssLog->Add(("NOM_NAP "+(pinf.NOM_NAP)));
			}
			if (ndStub->NodeName.UpperCase()=="DTA_NAP")
			{
			  pers_flags|=1<<1;
			  int y;
			  int m;
			  int d;
			  readSqlDate(ndStub->Text,y,m,d);
			  try{
			   TDateTime dtStub= EncodeDate(y,m,d);
			   pinf.DTA_NAP=dtStub;
			  }catch(...){};
			  //pinf.DTA_NAP =   ndStub->Text.Trim() ;
			  ssLog->Add("DTA_NAP "+ DateToStr(pinf.DTA_NAP) );
			}
			if (ndStub->NodeName.UpperCase()=="FRM_MP")
			{
			  pers_flags|=1<<2;
			  pinf.FRM_MP= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("FRM_MP "+IntToStr(pinf.FRM_MP)));
			}
			//MCOD_NAP
			if (ndStub->NodeName.UpperCase()=="MCOD_NAP")
			{
			  pers_flags|=1<<3;
			  pinf.MCOD_NAP= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("MCOD_NAP "+IntToStr(pinf.MCOD_NAP)));
			}
			//MCOD_STC
			if (ndStub->NodeName.UpperCase()=="MCOD_STC")
			{
			  pers_flags|=1<<4;
			  pinf.MCOD_STC= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("MCOD_STC "+IntToStr(pinf.MCOD_STC)));
			}
			//VPOLIS
			if (ndStub->NodeName.UpperCase()=="VPOLIS")
			{
			  pers_flags|=1<<5;
			  pinf.VPOLIS= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("VPOLIS "+IntToStr(pinf.VPOLIS)));
			}
			//SPOLIS
			if (ndStub->NodeName.UpperCase()=="SPOLIS")
			{
			  pers_flags|=1<<6;
			  pinf.SPOLIS= (  ndStub->Text.Trim() );
			  ssLog->Add(("SPOLIS "+(pinf.SPOLIS)));
			}
			//NPOLIS
			if (ndStub->NodeName.UpperCase()=="NPOLIS")
			{
			  pers_flags|=1<<7;
			  pinf.NPOLIS= (  ndStub->Text.Trim());
			  ssLog->Add(("NPOLIS "+(pinf.NPOLIS)));
			}
			//SMO_OGRN   //@@ del in new version
			/*
			if (ndStub->NodeName.UpperCase()=="SMO_OGRN")
			{
			  pers_flags|=1<<8;
			  pinf.SMO_OGRN= (  ndStub->Text.Trim());
			  ssLog->Add(("SMO_OGRN "+(pinf.SMO_OGRN)));
			} */

			if (ndStub->NodeName.UpperCase()=="FAM")
			{
			  pers_flags|=1<<8;
			  pinf.FAM=   ndStub->Text.Trim() ;
			  ssLog->Add("FAM "+pinf.FAM);
			}
			if (ndStub->NodeName.UpperCase()=="IM")
			{
			  pers_flags|=1<<9;
			  pinf.IM=   ndStub->Text.Trim() ;
			  ssLog->Add("IM "+pinf.IM);
			}
			if (ndStub->NodeName.UpperCase()=="OT")
			{
			  pers_flags|=1<<10;
			  pinf.OT=   ndStub->Text.Trim() ;
			  ssLog->Add("OT "+pinf.OT);
			}
			//W
			if (ndStub->NodeName.UpperCase()=="W")
			{
			  pers_flags|=1<<11;
			  pinf.W = StrToIntDef(  ndStub->Text.Trim(),0) ;
			  ssLog->Add("W sex "+IntToStr(pinf.W));
			}
			//DR
			if (ndStub->NodeName.UpperCase()=="DR")
			{
			  pers_flags|=1<<12;
			  int y,m,d;
			  readSqlDate(ndStub->Text,y,m,d);
			  try{
			   pinf.DR= EncodeDate(y,m,d);
			  }catch(...){};
			  ssLog->Add("DR : birth_date "+DateTimeToStr(pinf.DR));
			}

			//DS
			if (ndStub->NodeName.UpperCase()=="DS")
			{
			  pers_flags|=1<<13;
			  pinf.DS = (  ndStub->Text.Trim() ) ;
			  ssLog->Add("diagnosis "+(pinf.DS));
			}
			//KOD_PFK
			if (ndStub->NodeName.UpperCase()=="KOD_PFK")
			{
			  pers_flags|=1<<14;
			  pinf.KOD_PFK= /*StrToIntDef*/  ndStub->Text.Trim() ;//,0);
			  ssLog->Add(("KOD_PFK "+/*IntToStr*/(pinf.KOD_PFK)));
			}
			//KOD_PFO
			if (ndStub->NodeName.UpperCase()=="KOD_PFO")
			{
			  pers_flags|=1<<15;
			  pinf.KOD_PFO= /*StrToIntDef*/  ndStub->Text.Trim() ; //,0);
			  ssLog->Add(("KOD_PFO "+/*IntToStr*/(pinf.KOD_PFO)));
			}


			if (ndStub->NodeName.UpperCase()=="DTA_FKT")
			{
			  pers_flags|=1<<16;
			  int y,m,d;
			  readSqlDate(ndStub->Text,y,m,d);
			  try{
			   pinf.DTA_FKT= EncodeDate(y,m,d);
			  }catch(...){};
			  ssLog->Add("DTA_FKT :  "+DateTimeToStr(pinf.DTA_FKT));

			  nv_rec.val_type=3;
			}

			if (ndStub->NodeName.UpperCase()=="TIM_FKT")
			{
			  pers_flags|=1<<17;
			  int h,m;
			  readSqlTime(ndStub->Text,h,m);
			  try{
			   pinf.TIM_FKT= EncodeTime(h,m,0,0);
			  }catch(...){};
			  ssLog->Add("TIM_FKT :  "+DateTimeToStr(pinf.TIM_FKT));

			  nv_rec.val_type=4;
			}


			if (ndStub->NodeName.UpperCase()=="SMOCOD")    //@@added 15.10.2020
			{
			  pers_flags|=1<<18;
			  pinf.SMOCOD= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("SMOCOD "+(pinf.SMOCOD)));
			}


			//ssLog->Add("Line 4 ");
			pinf.nvvReceived.push_back( nv_rec );//@@@name value pair with type @@check
		 }


		 //ssLog->Add("Line 5 ");
		 ssLog->Add("");
		 pinf.pers_flags=pers_flags;
		 //ssLog->Add("Line 6 ");
		 if ( pers_flags != 0x3FFFF ) {
			  //ssLog->Add("Line 7 ");
			  ssLog->Add("ERROR: wrong person record");
			  if (error_code==0) error_code=7;
		 }


         /////////////////////*********    for 1 record - finished
		 std::vector<String >    svMissing, svDuplicated;
		 all_required_received(svRequired, pinf.svReceived , svMissing, svDuplicated );
		 //pinf.svRequired= svRequired;
		 //pinf.svReceived=   svReceived;
		 pinf.svMissing=    svMissing;
		 pinf.svDuplicated= svDuplicated;
		 //pinf.ivErrorCodeRequired =ivRequiredErrorCodes;
		 /////////////////////


		 //ssLog->Add("Line 8 ");
		 persons.push_back(pinf);
		 //ssLog->Add("Line 9 ");


	 }
	 //ssLog->Add("Line 10 ");


  }

  //ssLog->Add("Line 11 ");
  if ( !(struct_flags & 1<<0 )) {
	  ssLog->Add("ERROR: no header");
	  if (error_code==0) error_code=6;
	  if (!file_info.error_code_general) file_info.error_code_general=ERROR_GENERAL_NO_HEADER;//@@error gen
  }
  if ( !(struct_flags & 1<<1 )) {
	  ssLog->Add("ERROR: not a single record found");
	  if (error_code==0) error_code=8;
  	  if (!file_info.error_code_general) file_info.error_code_general=ERROR_GENERAL_NO_RECORDS;//@@error gen

  }
  file_info.rec_emount= persons.size();
  flags_summary = pers_flags |  (hdr_flags<<16) | (struct_flags<<24);

  //ssLog->Add("Line 12 ");
  x->Free();
  //ssLog->Add("Line 13 ");
  }catch(Exception &e)
 {
  //
  file_info.error_code_general=ERROR_GENERAL_PARSER_EXCEPTION;   //@@
  file_info.error_desc_general=e.Message;

 }
  return error_code;
}







int process_hosp_xml_t4(String fn, THospFileInfo_T1 &file_info, std::vector<TPersonInfoHosp_T4> &persons, TStrings * ssLog , TADOQuery*q)
{
  int error_code=0;
  int flags_summary=0;
   file_info.error_code_general=0;
 file_info.ORIGINAL_FILENAME=ExtractFileName(fn);
  try{
  TXMLDocument *x= new TXMLDocument(Application);

  x->LoadFromFile(fn);
  IXMLNode *ndRoot = x->DocumentElement;
  int cnt=  ndRoot->ChildNodes->Count;
  if (ndRoot->NodeName.LowerCase()!="zl_list" ) {
	   file_info.error_code_general=ERROR_GENERAL_ROOT_MISSING;
  }

  unsigned char struct_flags=0;
  unsigned char hdr_flags=0;
  unsigned long pers_flags=0;


   /// checking
  std::vector<String>  svRequired ;
  ///  std::vector<int> ivRequiredErrorCodes;
  std::map<String,int> mpErrorsRequired;
  getRequiredFields(q, 4, svRequired,  mpErrorsRequired);
  file_info.mpErrorsRequired=  mpErrorsRequired;
  file_info.svRequired= svRequired;
   //ivRequiredErrorCodes);

  //@@
  std::vector<TReferenceInfo>  vref_inf;
  getReferenceCheckings( q,  vref_inf /*,"'ref','from','<=','>='"*/ );
  //////////// checking^


  for (int i = 0; i < cnt; i++) {
	 Application->ProcessMessages() ;
	 IXMLNode *ndChild=	 ndRoot-> ChildNodes->Get(i);

	 if (ndChild->NodeName.LowerCase()=="zglv" ) {
		 if ( struct_flags & 1<<0 ) {

			 ssLog->Add("ERROR: More than one header");
			 if (error_code==0) error_code=5;

		 }
		 struct_flags|=1<<0;

		  TPersonInfoHosp_T4 pinf;
		 memset(&pinf,0,sizeof(pinf));

		 file_info.dt_date=1;
		 for (int k = 0; k < ndChild->ChildNodes->Count; k++) {
			//
			IXMLNode * ndStub =  ndChild-> ChildNodes->Get(k);

				pinf.svReceived.push_back(ndStub->NodeName.UpperCase())  ; //@@@checking
			//String received_value=ndStub->Text.Trim();  //@@@
			//pinf.nvvReceived.push_back();
			TNameValueInfo nv_rec;
			nv_rec.name=  ndStub->NodeName.UpperCase().Trim();  //@@@
			nv_rec.value=  ndStub->Text.Trim();                 //@@@
			nv_rec.val_type=0;//@@@


			if (ndStub->NodeName.UpperCase()=="FILENAME")
			{
			  hdr_flags|= 1<<0;
			  file_info.FILENAME=ndStub->Text.Trim();
			  ssLog->Add("FILENAME="+file_info.FILENAME );
			  if ( LowerCase( file_info.FILENAME.Trim())!=Trim(LowerCase(ExtractFileName(fn))))
			  {
				   file_info.error_code_general=ERROR_FILENAME_NOT_MATCH;
			  }
			}
			if (ndStub->NodeName.UpperCase()=="DATA")
			{
			  hdr_flags|= 1<<1;
			  int y;
			  int m;
			  int d;
			  readSqlDate(ndStub->Text,y,m,d);
			  try{
			   TDateTime dtStub= EncodeDate(y,m,d);
			   file_info.dt_date=dtStub;
			  }catch(...){};
			  ssLog->Add("DATA="+DateToStr(file_info.dt_date) );
			}

		 }

		 if (hdr_flags != 0x0F) {
			 // header does not contain all the required fields
			 if ( !( hdr_flags & 1<<0 ) ) {
				ssLog->Add("FILENAME is missing");
				if (error_code==0) error_code=1;
			 }
			 if ( !( hdr_flags & 1<<1 ) ) {
				ssLog->Add("DATA (creation date!) is missing");
				if (error_code==0) error_code=2;
			 }

		 }

	 }

	//////
	 if (ndChild->NodeName.LowerCase()=="zap" ) {
		 struct_flags|=1<<1;
		 TPersonInfoHosp_T4 pinf;
		 memset(&pinf,0,sizeof(pinf));

		 for (int k = 0; k < ndChild->ChildNodes->Count; k++) {
			//
			IXMLNode * ndStub =  ndChild-> ChildNodes->Get(k);

            TNameValueInfo nv_rec;
			nv_rec.name=  ndStub->NodeName.UpperCase().Trim();  //@@@
			nv_rec.value=  ndStub->Text.Trim();                 //@@@
			nv_rec.val_type=0;//@@@

			pinf.svReceived.push_back(ndStub->NodeName.UpperCase())  ; //@@@2020-09

			if (ndStub->NodeName.UpperCase()=="NOM_NAP")
			{
			  pers_flags|=1<<0;
			  pinf.NOM_NAP= (  ndStub->Text.Trim());
			  ssLog->Add(("NOM_NAP "+(pinf.NOM_NAP)));
			}
			if (ndStub->NodeName.UpperCase()=="DTA_NAP")
			{
			  pers_flags|=1<<1;
			  int y;
			  int m;
			  int d;
			  readSqlDate(ndStub->Text,y,m,d);
			  try{
			   TDateTime dtStub= EncodeDate(y,m,d);
			   pinf.DTA_NAP=dtStub;
			  }catch(...){};
			  //pinf.DTA_NAP =   ndStub->Text.Trim() ;
			  ssLog->Add("DTA_NAP "+ DateToStr(pinf.DTA_NAP) );
			}
			if (ndStub->NodeName.UpperCase()=="IST_ANL")  //4
			{
			  pers_flags|=1<<2;
			  pinf.IST_ANL= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("IST_ANL "+IntToStr(pinf.IST_ANL)));
			}
			if (ndStub->NodeName.UpperCase()=="ACOD")     //4
			{
			  pers_flags|=1<<3;
			  pinf.ACOD= (  ndStub->Text.Trim());
			  ssLog->Add(("ACOD "+(pinf.ACOD)));
			}
			if (ndStub->NodeName.UpperCase()=="PR_ANL")  //4
			{
			  pers_flags|=1<<4;
			  pinf.PR_ANL= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("PR_ANL "+IntToStr(pinf.PR_ANL)));
			}






			//ssLog->Add("Line 4 ");
			pinf.nvvReceived.push_back( nv_rec );//@@@name value pair with type @@check
		 }
		 //ssLog->Add("Line 5 ");
		 ssLog->Add("");
		 pinf.pers_flags=pers_flags;
		 //ssLog->Add("Line 6 ");
		 if ( pers_flags != 0x1F ) {
			  //ssLog->Add("Line 7 ");
			  ssLog->Add("ERROR: wrong person record");
			  if (error_code==0) error_code=7;
		 }
		 //ssLog->Add("Line 8 ");


           /////////////////////*********    for 1 record - finished
		 std::vector<String >    svMissing, svDuplicated;
		 all_required_received(svRequired, pinf.svReceived , svMissing, svDuplicated );
		 //pinf.svRequired= svRequired;
		 //pinf.svReceived=   svReceived;
		 pinf.svMissing=    svMissing;
		 pinf.svDuplicated= svDuplicated;
		 //pinf.ivErrorCodeRequired =ivRequiredErrorCodes;
		 /////////////////////


		 persons.push_back(pinf);
		 //ssLog->Add("Line 9 ");


	 }
	 //ssLog->Add("Line 10 ");


  }

  //ssLog->Add("Line 11 ");
  if ( !(struct_flags & 1<<0 )) {
	  ssLog->Add("ERROR: no header");
	  if (error_code==0) error_code=6;
  	  if (!file_info.error_code_general) file_info.error_code_general=ERROR_GENERAL_NO_HEADER;//@@error gen
  }
  if ( !(struct_flags & 1<<1 )) {
	  ssLog->Add("ERROR: not a single record found");
	  if (error_code==0) error_code=8;
  	  if (!file_info.error_code_general) file_info.error_code_general=ERROR_GENERAL_NO_RECORDS;//@@error gen

  }
  file_info.rec_emount= persons.size();
  flags_summary = pers_flags |  (hdr_flags<<16) | (struct_flags<<24);

  //ssLog->Add("Line 12 ");
  x->Free();
  //ssLog->Add("Line 13 ");
  }catch(Exception &e)
 {
  //
  file_info.error_code_general=ERROR_GENERAL_PARSER_EXCEPTION;   //@@
  file_info.error_desc_general=e.Message;

 }
  return error_code;
}






int process_hosp_xml_t5(String fn, THospFileInfo_T1 &file_info, std::vector<TPersonInfoHosp_T5> &persons, TStrings * ssLog , TADOQuery*q)
{
  int error_code=0;
  int flags_summary=0;
  file_info.error_code_general=0;
  file_info.ORIGINAL_FILENAME=ExtractFileName(fn);
  try{
  TXMLDocument *x= new TXMLDocument(Application);

  x->LoadFromFile(fn);
  IXMLNode *ndRoot = x->DocumentElement;
  int cnt=  ndRoot->ChildNodes->Count;
  if (ndRoot->NodeName.LowerCase()!="zl_list" ) {
	   file_info.error_code_general=ERROR_GENERAL_ROOT_MISSING;
  }

  unsigned char struct_flags=0;
  unsigned char hdr_flags=0;
  unsigned long pers_flags=0;


   /// checking
  std::vector<String>  svRequired ;
  ///  std::vector<int> ivRequiredErrorCodes;
  std::map<String,int> mpErrorsRequired;
  getRequiredFields(q, 5, svRequired,  mpErrorsRequired);
  file_info.mpErrorsRequired=  mpErrorsRequired;
  file_info.svRequired= svRequired;
   //ivRequiredErrorCodes);

  //@@
  std::vector<TReferenceInfo>  vref_inf;
  getReferenceCheckings( q,  vref_inf /*,"'ref','from','<=','>='"*/ );
  //////////// checking^

  for (int i = 0; i < cnt; i++) {
	 Application->ProcessMessages() ;
	 IXMLNode *ndChild=	 ndRoot-> ChildNodes->Get(i);

	 if (ndChild->NodeName.LowerCase()=="zglv" ) {
		 if ( struct_flags & 1<<0 ) {

			 ssLog->Add("ERROR: More than one header");
			 if (error_code==0) error_code=5;

		 }
		 struct_flags|=1<<0;

		 file_info.dt_date=1;
		 for (int k = 0; k < ndChild->ChildNodes->Count; k++) {
			//
			IXMLNode * ndStub =  ndChild-> ChildNodes->Get(k);


			if (ndStub->NodeName.UpperCase()=="FILENAME")
			{
			  hdr_flags|= 1<<0;
			  file_info.FILENAME=ndStub->Text.Trim();
			  ssLog->Add("FILENAME="+file_info.FILENAME );
			  if ( LowerCase( file_info.FILENAME.Trim())!=Trim(LowerCase(ExtractFileName(fn))))
			  {
				   file_info.error_code_general=ERROR_FILENAME_NOT_MATCH;
			  }
			}
			if (ndStub->NodeName.UpperCase()=="DATA")
			{
			  hdr_flags|= 1<<1;
			  int y;
			  int m;
			  int d;
			  readSqlDate(ndStub->Text,y,m,d);
			  try{
			   TDateTime dtStub= EncodeDate(y,m,d);
			   file_info.dt_date=dtStub;
			  }catch(...){};
			  ssLog->Add("DATA="+DateToStr(file_info.dt_date) );
			}

		 }

		 if (hdr_flags != 0x0F) {
			 // header does not contain all the required fields
			 if ( !( hdr_flags & 1<<0 ) ) {
				ssLog->Add("FILENAME is missing");
				if (error_code==0) error_code=1;
			 }
			 if ( !( hdr_flags & 1<<1 ) ) {
				ssLog->Add("DATA (creation date!) is missing");
				if (error_code==0) error_code=2;
			 }

		 }

	 }

	//////
	 if (ndChild->NodeName.LowerCase()=="zap" ) {
		 struct_flags|=1<<1;
		 TPersonInfoHosp_T5 pinf;
		 memset(&pinf,0,sizeof(pinf));

		 for (int k = 0; k < ndChild->ChildNodes->Count; k++) {
			//


			IXMLNode * ndStub =  ndChild-> ChildNodes->Get(k);




            	pinf.svReceived.push_back(ndStub->NodeName.UpperCase())  ; //@@@checking
			//String received_value=ndStub->Text.Trim();  //@@@
			//pinf.nvvReceived.push_back();
			TNameValueInfo nv_rec;
			nv_rec.name=  ndStub->NodeName.UpperCase().Trim();  //@@@
			nv_rec.value=  ndStub->Text.Trim();                 //@@@
			nv_rec.val_type=0;//@@@




			if (ndStub->NodeName.UpperCase()=="NOM_NAP")
			{
			  pers_flags|=1<<0;
			  pinf.NOM_NAP= (  ndStub->Text.Trim());
			  ssLog->Add(("NOM_NAP "+(pinf.NOM_NAP)));
			}
			if (ndStub->NodeName.UpperCase()=="DTA_NAP")
			{
			  pers_flags|=1<<1;
			  int y;
			  int m;
			  int d;
			  readSqlDate(ndStub->Text,y,m,d);
			  try{
			   TDateTime dtStub= EncodeDate(y,m,d);
			   pinf.DTA_NAP=dtStub;
			  }catch(...){
               pinf.DTA_NAP=0;
			  };
			  //pinf.DTA_NAP =   ndStub->Text.Trim() ;
			  ssLog->Add("DTA_NAP "+ DateToStr(pinf.DTA_NAP) );
			}
			if (ndStub->NodeName.UpperCase()=="FRM_MP")
			{
			  pers_flags|=1<<2;
			  pinf.FRM_MP= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("FRM_MP "+IntToStr(pinf.FRM_MP)));
			}
			//MCOD_NAP
			/*
			if (ndStub->NodeName.UpperCase()=="MCOD_NAP")
			{
			  pers_flags|=1<<3;
			  pinf.MCOD_NAP= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("MCOD_NAP "+IntToStr(pinf.MCOD_NAP)));
			} */
			//MCOD_STC
			if (ndStub->NodeName.UpperCase()=="MCOD_STC")
			{
			  pers_flags|=1<<3;
			  pinf.MCOD_STC= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("MCOD_STC "+IntToStr(pinf.MCOD_STC)));
			}
			/*
			//VPOLIS
			if (ndStub->NodeName.UpperCase()=="VPOLIS")
			{
			  pers_flags|=1<<5;
			  pinf.VPOLIS= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("VPOLIS "+IntToStr(pinf.VPOLIS)));
			}
			//SPOLIS
			if (ndStub->NodeName.UpperCase()=="SPOLIS")
			{
			  pers_flags|=1<<6;
			  pinf.SPOLIS= (  ndStub->Text.Trim() );
			  ssLog->Add(("SPOLIS "+(pinf.SPOLIS)));
			}
			//NPOLIS
			if (ndStub->NodeName.UpperCase()=="NPOLIS")
			{
			  pers_flags|=1<<7;
			  pinf.NPOLIS= (  ndStub->Text.Trim());
			  ssLog->Add(("NPOLIS "+(pinf.NPOLIS)));
			} */
			/*
			//SMO_OGRN
			if (ndStub->NodeName.UpperCase()=="SMO_OGRN")
			{
			  pers_flags|=1<<4;
			  pinf.SMO_OGRN= (  ndStub->Text.Trim());
			  ssLog->Add(("SMO_OGRN "+(pinf.SMO_OGRN)));
			} */
			/*
			//ST_OKATO
			if (ndStub->NodeName.UpperCase()=="ST_OKATO")
			{
			  pers_flags|=1<<9;
			  pinf.ST_OKATO= (  ndStub->Text.Trim());
			  ssLog->Add(("ST_OKATO "+(pinf.ST_OKATO)));
			} */
			/*
			if (ndStub->NodeName.UpperCase()=="FAM")
			{
			  pers_flags|=1<<10;
			  pinf.FAM=   ndStub->Text.Trim() ;
			  ssLog->Add("FAM "+pinf.FAM);
			}
			if (ndStub->NodeName.UpperCase()=="IM")
			{
			  pers_flags|=1<<11;
			  pinf.IM=   ndStub->Text.Trim() ;
			  ssLog->Add("IM "+pinf.IM);
			}
			if (ndStub->NodeName.UpperCase()=="OT")
			{
			  pers_flags|=1<<12;
			  pinf.OT=   ndStub->Text.Trim() ;
			  ssLog->Add("OT "+pinf.OT);
			}     */
			//W
			if (ndStub->NodeName.UpperCase()=="W")
			{
			  pers_flags|=1<<4;
			  pinf.W = StrToIntDef(  ndStub->Text.Trim(),0) ;
			  ssLog->Add("W sex "+IntToStr(pinf.W));
			}
			//DR
			if (ndStub->NodeName.UpperCase()=="DR")
			{
			  pers_flags|=1<<5;
			  int y,m,d;
			  readSqlDate(ndStub->Text,y,m,d);
			  try{
			   pinf.DR= EncodeDate(y,m,d);
			  }catch(...){};
			  ssLog->Add("DR : birth_date "+DateTimeToStr(pinf.DR));
			}
			/*
			//TLF
			if (ndStub->NodeName.UpperCase()=="TLF")
			{
			  pers_flags|=1<<15;
			  pinf.TLF=   ndStub->Text.Trim() ;
			  ssLog->Add("TLF "+pinf.TLF);
			} */
			/*
			//DS
			if (ndStub->NodeName.UpperCase()=="DS")
			{
			  pers_flags|=1<<16;
			  pinf.DS = (  ndStub->Text.Trim() ) ;
			  ssLog->Add("diagnosis "+(pinf.DS));
			} */
			//KOD_PFK
			if (ndStub->NodeName.UpperCase()=="KOD_PFK")
			{
			  pers_flags|=1<<6;
			  pinf.KOD_PFK= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("KOD_PFK "+IntToStr(pinf.KOD_PFK)));
			}
			//KOD_PFO
			if (ndStub->NodeName.UpperCase()=="KOD_PFO")
			{
			  pers_flags|=1<<7;
			  pinf.KOD_PFO= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("KOD_PFO "+IntToStr(pinf.KOD_PFO)));
			}
			/*
			//KOD_DCT
			if (ndStub->NodeName.UpperCase()=="KOD_DCT")
			{
			  pers_flags|=1<<19;
			  pinf.KOD_DCT = (  ndStub->Text.Trim() ) ;
			  ssLog->Add("KOD_DCT "+(pinf.KOD_DCT));
			}
			//DTA_PLN
			if (ndStub->NodeName.UpperCase()=="DTA_PLN")
			{
			  pers_flags|=1<<20;
			  int y,m,d;
			  readSqlDate(ndStub->Text,y,m,d);
			  try{
			   pinf.DTA_PLN= EncodeDate(y,m,d);
			  }catch(...){};
			  ssLog->Add("DTA_PLN :  "+DateTimeToStr(pinf.DTA_PLN));
			}
			//USL_OK
			if (ndStub->NodeName.UpperCase()=="USL_OK")
			{
			  pers_flags|=1<<21;
			  pinf.USL_OK= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("USL_OK "+IntToStr(pinf.USL_OK)));
			}  */

			if (ndStub->NodeName.UpperCase()=="DTA_FKT")
			{
			  pers_flags|=1<<8;
			  int y,m,d;
			  readSqlDate(ndStub->Text,y,m,d);
			  try{
			   pinf.DTA_FKT= EncodeDate(y,m,d);
			  }catch(...){};
			  ssLog->Add("DTA_FKT :  "+DateTimeToStr(pinf.DTA_FKT));
			}

			if (ndStub->NodeName.UpperCase()=="DTA_END")
			{
			  pers_flags|=1<<9;
			  int y,m,d;
			  readSqlDate(ndStub->Text,y,m,d);
			  try{
			   pinf.DTA_END= EncodeDate(y,m,d);
			  }catch(...){};
			  ssLog->Add("DTA_END :  "+DateTimeToStr(pinf.DTA_END));
			}

			//KOD_PFO
			if (ndStub->NodeName.UpperCase()=="NHISTORY")
			{
			  pers_flags|=1<<10;
			  pinf.NHISTORY= (  ndStub->Text.Trim());
			  ssLog->Add(("NHISTORY "+(pinf.NHISTORY)));
			}
			  /*
			if (ndStub->NodeName.UpperCase()=="TIM_FKT")
			{
			  pers_flags|=1<<20;
			  int h,m;
			  readSqlTime(ndStub->Text,h,m);
			  try{
			   pinf.TIM_FKT= EncodeTime(h,m,0,0);
			  }catch(...){};
			  ssLog->Add("TIM_FKT :  "+DateTimeToStr(pinf.TIM_FKT));
			}   */




			//ssLog->Add("Line 4 ");
			pinf.nvvReceived.push_back( nv_rec );//@@@name value pair with type
		 }
		 //ssLog->Add("Line 5 ");
		 ssLog->Add("");
		 pinf.pers_flags=pers_flags;
		 //ssLog->Add("Line 6 ");
		 if ( pers_flags != 0x7FF ) {
			  //ssLog->Add("Line 7 ");
			  ssLog->Add("ERROR: wrong person record");
			  if (error_code==0) error_code=7;
		 }
		 //ssLog->Add("Line 8 ");


		   /////////////////////*********    for 1 record - finished
		 std::vector<String >    svMissing, svDuplicated;
		 all_required_received(svRequired, pinf.svReceived , svMissing, svDuplicated );
		 //pinf.svRequired= svRequired;
		 //pinf.svReceived=   svReceived;
		 pinf.svMissing=    svMissing;
		 pinf.svDuplicated= svDuplicated;
		 //pinf.ivErrorCodeRequired =ivRequiredErrorCodes;
		 /////////////////////


		 persons.push_back(pinf);
		 //ssLog->Add("Line 9 ");


	 }
	 //ssLog->Add("Line 10 ");


  }

  //ssLog->Add("Line 11 ");
  if ( !(struct_flags & 1<<0 )) {
	  ssLog->Add("ERROR: no header");
	  if (error_code==0) error_code=6;
  	  if (!file_info.error_code_general) file_info.error_code_general=ERROR_GENERAL_NO_HEADER;//@@error gen
  }
  if ( !(struct_flags & 1<<1 )) {
	  ssLog->Add("ERROR: not a single record found");
	  if (error_code==0) error_code=8;
  	  if (!file_info.error_code_general) file_info.error_code_general=ERROR_GENERAL_NO_RECORDS;//@@error gen

  }
  file_info.rec_emount= persons.size();
  flags_summary = pers_flags |  (hdr_flags<<16) | (struct_flags<<24);

  //ssLog->Add("Line 12 ");
  x->Free();
  //ssLog->Add("Line 13 ");
  }catch(Exception &e)
 {
  //
  file_info.error_code_general=ERROR_GENERAL_PARSER_EXCEPTION;   //@@
  file_info.error_desc_general=e.Message;

 }
  return error_code;
}






int process_hosp_xml_t6(String fn, THospFileInfo_T1 &file_info, std::vector<TPersonInfoHosp_T6> &persons, TStrings * ssLog , TADOQuery* q)
{
  int error_code=0;
  int flags_summary=0;
  file_info.error_code_general=0;
  file_info.ORIGINAL_FILENAME=ExtractFileName(fn);
  try{
  TXMLDocument *x= new TXMLDocument(Application);

  x->LoadFromFile(fn);
  IXMLNode *ndRoot = x->DocumentElement;
  int cnt=  ndRoot->ChildNodes->Count;
  if (ndRoot->NodeName.LowerCase()!="zl_list" ) {
	   file_info.error_code_general=ERROR_GENERAL_ROOT_MISSING;
  }

  unsigned char struct_flags=0;
  unsigned char hdr_flags=0;
  unsigned long pers_flags=0;



   /// checking
  std::vector<String>  svRequired ;
  ///  std::vector<int> ivRequiredErrorCodes;
  std::map<String,int> mpErrorsRequired;
  getRequiredFields(q, 6, svRequired,  mpErrorsRequired);
  file_info.mpErrorsRequired=  mpErrorsRequired;
  file_info.svRequired= svRequired;
   //ivRequiredErrorCodes);

  //@@
  std::vector<TReferenceInfo>  vref_inf;
  getReferenceCheckings( q,  vref_inf /*,"'ref','from','<=','>='"*/ );
  //////////// checking^


  for (int i = 0; i < cnt; i++) {
	 Application->ProcessMessages() ;
	 IXMLNode *ndChild=	 ndRoot-> ChildNodes->Get(i);

	 if (ndChild->NodeName.LowerCase()=="zglv" ) {
		 if ( struct_flags & 1<<0 ) {

			 ssLog->Add("ERROR: More than one header");
			 if (error_code==0) error_code=5;

		 }
		 struct_flags|=1<<0;

		 file_info.dt_date=1;
		 for (int k = 0; k < ndChild->ChildNodes->Count; k++) {
			//
			IXMLNode * ndStub =  ndChild-> ChildNodes->Get(k);
			if (ndStub->NodeName.UpperCase()=="FILENAME")
			{
			  hdr_flags|= 1<<0;
			  file_info.FILENAME=ndStub->Text.Trim();
			  ssLog->Add("FILENAME="+file_info.FILENAME );
			  if ( LowerCase( file_info.FILENAME.Trim())!=Trim(LowerCase(ExtractFileName(fn))))
			  {
				   file_info.error_code_general=ERROR_FILENAME_NOT_MATCH;
			  }
			}
			if (ndStub->NodeName.UpperCase()=="DATA")
			{
			  hdr_flags|= 1<<1;
			  int y;
			  int m;
			  int d;
			  readSqlDate(ndStub->Text,y,m,d);
			  try{
			   TDateTime dtStub= EncodeDate(y,m,d);
			   file_info.dt_date=dtStub;
			  }catch(...){};
			  ssLog->Add("DATA="+DateToStr(file_info.dt_date) );
			}

		 }

		 if (hdr_flags != 0x0F) {
			 // header does not contain all the required fields
			 if ( !( hdr_flags & 1<<0 ) ) {
				ssLog->Add("FILENAME is missing");
				if (error_code==0) error_code=1;
			 }
			 if ( !( hdr_flags & 1<<1 ) ) {
				ssLog->Add("DATA (creation date!) is missing");
				if (error_code==0) error_code=2;
			 }

		 }

	 }

	//////
	 if (ndChild->NodeName.LowerCase()=="zap" ) {
		 struct_flags|=1<<1;
		 TPersonInfoHosp_T6 pinf;
		 memset(&pinf,0,sizeof(pinf));

		 for (int k = 0; k < ndChild->ChildNodes->Count; k++) {
			//


			IXMLNode * ndStub =  ndChild-> ChildNodes->Get(k);


            	pinf.svReceived.push_back(ndStub->NodeName.UpperCase())  ; //@@@checking
			//String received_value=ndStub->Text.Trim();  //@@@
			//pinf.nvvReceived.push_back();
			TNameValueInfo nv_rec;
			nv_rec.name=  ndStub->NodeName.UpperCase().Trim();  //@@@
			nv_rec.value=  ndStub->Text.Trim();                 //@@@
			nv_rec.val_type=0;//@@@




			if (ndStub->NodeName.UpperCase()=="DTA_RAB")
			{
			  pers_flags|=1<<0;
			  int y;
			  int m;
			  int d;
			  readSqlDate(ndStub->Text,y,m,d);
			  try{
			   TDateTime dtStub= EncodeDate(y,m,d);
			   pinf.DTA_RAB=dtStub;
			  }catch(...){};
			  //pinf.DTA_RAB =   ndStub->Text.Trim() ;
			  ssLog->Add("DTA_RAB "+ DateToStr(pinf.DTA_RAB) );
			}
			if (ndStub->NodeName.UpperCase()=="MCOD_STC")
			{
			  pers_flags|=1<<1;
			  pinf.MCOD_STC= (  ndStub->Text.Trim(),0);
			  ssLog->Add(("MCOD_STC "+(pinf.MCOD_STC)));
			}
			if (ndStub->NodeName.UpperCase()=="KOD_PFK")
			{
			  pers_flags|=1<<2;
			  pinf.KOD_PFK= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("KOD_PFK "+(pinf.KOD_PFK)));
			}
			/*
<KOL_PAC>21</KOL_PAC>
<KOL_IN>0</KOL_IN>
<KOL_OUT>2</KOL_OUT>
<KOL_PLN>0</KOL_PLN>
<KOL_PUS>11</KOL_PUS>
<KOL_PUS_M>11</KOL_PUS_M>
<KOL_PUS_J>0</KOL_PUS_J>
<KOL_PUS_D>0</KOL_PUS_D>
<VKOL_FKT>0</VKOL_FKT>
<VUED_FKT>0</VUED_FKT>			*/
			if (ndStub->NodeName.UpperCase()=="KOL_PAC")
			{
			  pers_flags|=1<<3;
			  pinf.KOL_PAC= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("KOL_PAC "+IntToStr(pinf.KOL_PAC)));
			}
			if (ndStub->NodeName.UpperCase()=="KOL_IN")
			{
			  pers_flags|=1<<4;
			  pinf.KOL_IN= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("KOL_IN "+IntToStr(pinf.KOL_IN)));
			}
			if (ndStub->NodeName.UpperCase()=="KOL_OUT")
			{
			  pers_flags|=1<<5;
			  pinf.KOL_OUT= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("KOL_OUT "+IntToStr(pinf.KOL_OUT)));
			}
			if (ndStub->NodeName.UpperCase()=="KOL_PLN")
			{
			  pers_flags|=1<<6;
			  pinf.KOL_PLN= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("KOL_PLN "+IntToStr(pinf.KOL_PLN)));
			}
			if (ndStub->NodeName.UpperCase()=="KOL_PUS")
			{
			  pers_flags|=1<<7;
			  pinf.KOL_PUS= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("KOL_PUS "+IntToStr(pinf.KOL_PUS)));
			}
			if (ndStub->NodeName.UpperCase()=="KOL_PUS_M")
			{
			  pers_flags|=1<<8;
			  pinf.KOL_PUS_M= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("KOL_PUS_M "+IntToStr(pinf.KOL_PUS_M)));
			}
			if (ndStub->NodeName.UpperCase()=="KOL_PUS_J")
			{
			  pers_flags|=1<<9;
			  pinf.KOL_PUS_J= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("KOL_PUS_J "+IntToStr(pinf.KOL_PUS_J)));
			}
			if (ndStub->NodeName.UpperCase()=="KOL_PUS_D")
			{
			  pers_flags|=1<<10;
			  pinf.KOL_PUS_D= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("KOL_PUS_D "+IntToStr(pinf.KOL_PUS_D)));
			}
			if (ndStub->NodeName.UpperCase()=="VKOL_FKT")
			{
			  pers_flags|=1<<11;
			  pinf.VKOL_FKT= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("VKOL_FKT "+IntToStr(pinf.VKOL_FKT)));
			}
			if (ndStub->NodeName.UpperCase()=="VUED_FKT")
			{
			  pers_flags|=1<<12;
			  pinf.VUED_FKT= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("VUED_FKT "+IntToStr(pinf.VUED_FKT)));
			}


			if (ndStub->NodeName.UpperCase()=="KOD_PFO")  //@@2020 dec
			{
			  pers_flags|=1<<13;
			  pinf.KOD_PFO= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("KOD_PFO "+(pinf.KOD_PFO)));
			}


            pinf.nvvReceived.push_back( nv_rec );//@@@name value pair with type
		 }


			ssLog->Add("");
		 pinf.pers_flags=pers_flags;
		 //ssLog->Add("Line 6 ");
		 if ( pers_flags != 0x1FFF ) {
			  //ssLog->Add("Line 7 ");
			  ssLog->Add("ERROR: wrong person record");
			  if (error_code==0) error_code=7;
		 }
		 //ssLog->Add("Line 8 ");


		 	   /////////////////////*********    for 1 record - finished
		 std::vector<String >    svMissing, svDuplicated;
		 all_required_received(svRequired, pinf.svReceived , svMissing, svDuplicated );
		 //pinf.svRequired= svRequired;
		 //pinf.svReceived=   svReceived;
		 pinf.svMissing=    svMissing;
		 pinf.svDuplicated= svDuplicated;
		 //pinf.ivErrorCodeRequired =ivRequiredErrorCodes;
		 /////////////////////

		 persons.push_back(pinf);
		 }
	 }


 //ssLog->Add("Line 11 ");
  if ( !(struct_flags & 1<<0 )) {
	  ssLog->Add("ERROR: no header");
	  if (error_code==0) error_code=6;
	  if (!file_info.error_code_general) file_info.error_code_general=ERROR_GENERAL_NO_HEADER;//@@error gen
  }
  if ( !(struct_flags & 1<<1 )) {
	  ssLog->Add("ERROR: not a single record found");
	  if (error_code==0) error_code=8;
	  if (!file_info.error_code_general) file_info.error_code_general=ERROR_GENERAL_NO_RECORDS;//@@error gen

  }
  file_info.rec_emount= persons.size();
  flags_summary = pers_flags |  (hdr_flags<<16) | (struct_flags<<24);

  //ssLog->Add("Line 12 ");
  x->Free();
  //ssLog->Add("Line 13 ");
  }catch(Exception &e)
 {
  //
  file_info.error_code_general=ERROR_GENERAL_PARSER_EXCEPTION;   //@@
  file_info.error_desc_general=e.Message;

 }
  return error_code;
  }







int process_hosp_xml_t3(String fn, THospFileInfo_T1 &file_info, std::vector<TPersonInfoHosp_T3> &persons, TStrings * ssLog  , TADOQuery*q)
{
  int error_code=0;
  int flags_summary=0;
  file_info.error_code_general=0;
  file_info.ORIGINAL_FILENAME=ExtractFileName(fn);
  try{
  TXMLDocument *x= new TXMLDocument(Application);

  x->LoadFromFile(fn);
  IXMLNode *ndRoot = x->DocumentElement;
  int cnt=  ndRoot->ChildNodes->Count;
  if (ndRoot->NodeName.LowerCase()!="zl_list" ) {
	   file_info.error_code_general=ERROR_GENERAL_ROOT_MISSING;
  }

  unsigned char struct_flags=0;
  unsigned char hdr_flags=0;
  unsigned long pers_flags=0;

  /// checking
  std::vector<String>  svRequired ;
  ///  std::vector<int> ivRequiredErrorCodes;
  std::map<String,int> mpErrorsRequired;
  getRequiredFields(q, 3, svRequired,  mpErrorsRequired); //@@glitch  getRequiredFields(q, 2, svRequired,  mpErrorsRequired);
  												// parameter must be 3 -revealed 28oct2020
  file_info.mpErrorsRequired=  mpErrorsRequired;
  file_info.svRequired= svRequired;
   //ivRequiredErrorCodes);

  for (int i = 0; i < cnt; i++) {
     Application->ProcessMessages() ;
	 IXMLNode *ndChild=	 ndRoot-> ChildNodes->Get(i);


	 if (ndChild->NodeName.LowerCase()=="zglv" ) {
		 if ( struct_flags & 1<<0 ) {

			 ssLog->Add("ERROR: More than one header");
			 if (error_code==0) error_code=5;

		 }
		 struct_flags|=1<<0;

		 TPersonInfoHosp_T3 pinf;
		 memset(&pinf,0,sizeof(pinf));

		 file_info.dt_date=1;
		 for (int k = 0; k < ndChild->ChildNodes->Count; k++) {
			//
			IXMLNode * ndStub =  ndChild-> ChildNodes->Get(k);

            	pinf.svReceived.push_back(ndStub->NodeName.UpperCase())  ; //@@@checking
			//String received_value=ndStub->Text.Trim();  //@@@
			//pinf.nvvReceived.push_back();
			TNameValueInfo nv_rec;
			nv_rec.name=  ndStub->NodeName.UpperCase().Trim();  //@@@
			nv_rec.value=  ndStub->Text.Trim();                 //@@@
			nv_rec.val_type=0;//@@@



			if (ndStub->NodeName.UpperCase()=="FILENAME")
			{
			  hdr_flags|= 1<<0;
			  file_info.FILENAME=ndStub->Text.Trim();
			  ssLog->Add("FILENAME="+file_info.FILENAME );
			  if ( LowerCase( file_info.FILENAME.Trim())!=Trim(LowerCase(ExtractFileName(fn))))
			  {
				   file_info.error_code_general=ERROR_FILENAME_NOT_MATCH;
			  }
			}
			if (ndStub->NodeName.UpperCase()=="DATA")
			{
			  hdr_flags|= 1<<1;
			  int y;
			  int m;
			  int d;
			  readSqlDate(ndStub->Text,y,m,d);
			  try{
			   TDateTime dtStub= EncodeDate(y,m,d);
			   file_info.dt_date=dtStub;
			  }catch(...){};
			  ssLog->Add("DATA="+DateToStr(file_info.dt_date) );
			}


           pinf.nvvReceived.push_back( nv_rec );//@@@name value pair with type
		 }

		 if (hdr_flags != 0x0F) {
			 // header does not contain all the required fields
			 if ( !( hdr_flags & 1<<0 ) ) {
				ssLog->Add("FILENAME is missing");
				if (error_code==0) error_code=1;
			 }
			 if ( !( hdr_flags & 1<<1 ) ) {
				ssLog->Add("DATA (creation date!) is missing");
				if (error_code==0) error_code=2;
			 }

		 }

	 }

	//////
	 if (ndChild->NodeName.LowerCase()=="zap" ) {
		 struct_flags|=1<<1;
		 TPersonInfoHosp_T3 pinf;
		 memset(&pinf,0,sizeof(pinf));

		 for (int k = 0; k < ndChild->ChildNodes->Count; k++) {
			//


			IXMLNode * ndStub =  ndChild-> ChildNodes->Get(k);

			TNameValueInfo nv_rec;
			nv_rec.name=  ndStub->NodeName.UpperCase().Trim();  //@@@
			nv_rec.value=  ndStub->Text.Trim();                 //@@@
			nv_rec.val_type=0;//@@@

			pinf.svReceived.push_back(ndStub->NodeName.UpperCase())  ; //@@@2020-09
			//
			if (ndStub->NodeName.UpperCase()=="MCOD_STC")
			{
			  pers_flags|=1<<0;
			  pinf.MCOD_STC= StrToInt(  ndStub->Text.Trim());
			  ssLog->Add(("MCOD_STC "+(pinf.MCOD_STC)));
			}

			if (ndStub->NodeName.UpperCase()=="DTA_FKT")
			{
			  pers_flags|=1<<0;
			  pinf.DTA_FKT= hyphenStrToDate(  ndStub->Text.Trim());
			  ssLog->Add(("DTA_FKT "+(pinf.DTA_FKT)));
			}

			if (ndStub->NodeName.UpperCase()=="TIM_FKT")
			{
			  pers_flags|=1<<0;
			  pinf.TIM_FKT= hyphenStrToTime(  ndStub->Text.Trim());
			  ssLog->Add(("TIM_FKT "+(pinf.TIM_FKT)));
			}

			if (ndStub->NodeName.UpperCase()=="VPOLIS")
			{
			  pers_flags|=1<<0;
			  pinf.VPOLIS= (  ndStub->Text.Trim());
			  ssLog->Add(("VPOLIS "+(pinf.VPOLIS)));
			}

			if (ndStub->NodeName.UpperCase()=="SPOLIS")
			{
			  pers_flags|=1<<0;
			  pinf.SPOLIS= (  ndStub->Text.Trim());
			  ssLog->Add(("SPOLIS "+(pinf.SPOLIS)));
			}

			if (ndStub->NodeName.UpperCase()=="NPOLIS")
			{
			  pers_flags|=1<<0;
			  pinf.NPOLIS= (  ndStub->Text.Trim());
			  ssLog->Add(("NPOLIS "+(pinf.NPOLIS)));
			}

			if (ndStub->NodeName.UpperCase()=="SMOCOD")
			{
			  pers_flags|=1<<0;
			  pinf.SMOCOD= (  ndStub->Text.Trim());
			  ssLog->Add(("SMOCOD "+(pinf.SMOCOD)));
			}

			if (ndStub->NodeName.UpperCase()=="FAM")
			{
			  pers_flags|=1<<0;
			  pinf.FAM= (  ndStub->Text.Trim());
			  ssLog->Add(("FAM "+(pinf.FAM)));
			}

			if (ndStub->NodeName.UpperCase()=="IM")
			{
			  pers_flags|=1<<0;
			  pinf.IM= (  ndStub->Text.Trim());
			  ssLog->Add(("IM "+(pinf.IM)));
			}

			if (ndStub->NodeName.UpperCase()=="OT")
			{
			  pers_flags|=1<<0;
			  pinf.OT= (  ndStub->Text.Trim());
			  ssLog->Add(("OT "+(pinf.OT)));
			}

			if (ndStub->NodeName.UpperCase()=="W")
			{
			  pers_flags|=1<<0;
			  pinf.W= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("W "+IntToStr(pinf.W)));
			}
			if (ndStub->NodeName.UpperCase()=="DR")
			{
			  pers_flags|=1<<0;
			  int y, m, d;
			  readSqlDate(ndStub->Text,y,m,d);
			  try{
			   TDateTime dtStub= EncodeDate(y,m,d);
			   pinf.DR=dtStub;
			  }catch(...){};
			  //pinf.DR =   ndStub->Text.Trim() ;
			  ssLog->Add("DR "+ DateToStr(pinf.DR) );
			}


			if (ndStub->NodeName.UpperCase()=="KOD_PFK")
			{
			  pers_flags|=1<<0;
			  pinf.KOD_PFK= (  ndStub->Text.Trim());
			  ssLog->Add(("KOD_PFK "+(pinf.KOD_PFK)));
			}

			if (ndStub->NodeName.UpperCase()=="KOD_PFO")
			{
			  pers_flags|=1<<0;
			  pinf.KOD_PFO= (  ndStub->Text.Trim());
			  ssLog->Add(("KOD_PFO "+(pinf.KOD_PFO)));
			}

			if (ndStub->NodeName.UpperCase()=="NHISTORY")
			{
			  pers_flags|=1<<0;
			  pinf.NHISTORY= (  ndStub->Text.Trim());
			  ssLog->Add(("NHISTORY "+(pinf.NHISTORY)));
			}

			if (ndStub->NodeName.UpperCase()=="DS")
			{
			  pers_flags|=1<<0;
			  pinf.DS= (  ndStub->Text.Trim());
			  ssLog->Add(("DS "+(pinf.DS)));
			}


           pinf.nvvReceived.push_back( nv_rec );//@@@name value pair with type
		 }


		 ssLog->Add("");
		 pinf.pers_flags=pers_flags;
		 //ssLog->Add("Line 6 ");
		 if ( pers_flags != 0x1FFF ) {
			  //ssLog->Add("Line 7 ");
			  ssLog->Add("ERROR: wrong person record");
			  if (error_code==0) error_code=7;
		 }
		 //ssLog->Add("Line 8 ");


		  /////////////////////*********    for 1 record - finished
		 std::vector<String >    svMissing, svDuplicated;
		 all_required_received(svRequired, pinf.svReceived , svMissing, svDuplicated );
		 //pinf.svRequired= svRequired;
		 //pinf.svReceived=   svReceived;
		 pinf.svMissing=    svMissing;
		 pinf.svDuplicated= svDuplicated;
		 //pinf.ivErrorCodeRequired =ivRequiredErrorCodes;
		 /////////////////////


		 persons.push_back(pinf);
		 }

	 }

     if ( !(struct_flags & 1<<0 )) {
	  ssLog->Add("ERROR: no header");
	  if (error_code==0) error_code=6;
  	  if (!file_info.error_code_general) file_info.error_code_general=ERROR_GENERAL_NO_HEADER;//@@error gen
  }
  if ( !(struct_flags & 1<<1 )) {
	  ssLog->Add("ERROR: not a single record found");
	  if (error_code==0) error_code=8;
	  if (!file_info.error_code_general) file_info.error_code_general=ERROR_GENERAL_NO_RECORDS;//@@error gen

  }
  file_info.rec_emount= persons.size();
  flags_summary = pers_flags |  (hdr_flags<<16) | (struct_flags<<24);

  //ssLog->Add("Line 12 ");
  x->Free();
  //ssLog->Add("Line 13 ");
  }catch(Exception &e)
 {
  //
  file_info.error_code_general=ERROR_GENERAL_PARSER_EXCEPTION;   //@@
  file_info.error_desc_general=e.Message;

 }
  return error_code;
}



String quoted(String s)
{
 return "'"+s+"'";
}

String str_limit_len(String s,int len)
{
  while(s.Length()>len)
  {
	s.Delete(1,1);
  }
  return s;
}

String str_limit_len_quot(String s,int len)
{
  while(s.Length()>len)
  {
	s.Delete(1,1);
  }
  s="'"+s+"'";
  return s;
}


int hosp_file_header_to_log(TADOQuery*q, THospFileInfo_T1 file_info, int persons_size, TStrings * ssLog, int TIP_SCT )
{
  /////////////////////
  /*SELECT TOP 1000 [ID]
	  ,[FILENAME]  ***
	  ,[DATA]
	  ,[DATA_OT] ***
	  ,[REC_AMOUNT] ***
	  ,[FILE_TYPE]   ***
	  ,[PROC_DT]   ***
	  ,[ERROR_CODE]  ***
	  ,[CODE_MO] ***
  FROM [DB_SURVEYS].[dbo].[files_log]*/
   String	sql1=
   "insert into [DB_SURVEYS].[dbo].[files_log] (file_type,FILENAME,PROC_DT,ERROR_CODE,CODE_MO,REC_AMOUNT,DATA_OT,DATA)\
   values ("+IntToStr(TIP_SCT)
   +",'"+file_info.FILENAME+"',"+/*sqlDate(file_info.dt_date)*/

   sqlDateTime(Now()) +","+IntToStr(file_info.error_code_general)
   +",'"+ FormatFloat("0000",file_info.CODE_MO)  //IntToStr(file_info.CODE_MO)
   +"',"+IntToStr((int)persons_size)
   +","+sqlDate(file_info.dt_date )  // DATA_OT
   +","+sqlDate(file_info.dt_date ) //  DATA


   +")" ;
   ssLog->Add(sql1);

   q->Close();
   q->SQL->Text=sql1;
   q->ExecSQL();
   ssLog->Add("Line 1102 ");

   int ID_files_log =identCurrent(q,"[DB_SURVEYS].[dbo].[files_log]");
   /////////////////////

   return  ID_files_log        ;
}

bool do_write_old_hosp_log =0;

int hosp_to_base_t1(TADOQuery*q, THospFileInfo_T1 file_info, std::vector<TPersonInfoHosp_T1> persons, TStrings * ssLog )
{
ssLog->Add("Line 1101 ");
   int TIP_SCT=11; //1;
   String sql1=
   "insert into [DB_SURVEYS].[dbo].[HOS_T1_SCT] (TIP_SCT,FILENAME,DTA_OBR)\
   values ("+IntToStr(TIP_SCT)
   +",'"+file_info.FILENAME+"',"+/*sqlDate(file_info.dt_date)*/

   sqlDateTime(Now())
   +")" ;
   ssLog->Add(sql1);

   q->Close();
   q->SQL->Text=sql1;
   if (do_write_old_hosp_log) q->ExecSQL();
   ssLog->Add("Line 1102 ");

   /////////////////////
  /*SELECT TOP 1000 [ID]
	  ,[FILENAME]  ***
	  ,[DATA]
	  ,[DATA_OT] ***
	  ,[REC_AMOUNT] ***
	  ,[FILE_TYPE]   ***
	  ,[PROC_DT]   ***
	  ,[ERROR_CODE]  ***
	  ,[CODE_MO] ***
  FROM [DB_SURVEYS].[dbo].[files_log]      *  /
	sql1=
   "insert into [DB_SURVEYS].[dbo].[files_log] (file_type,FILENAME,PROC_DT,ERROR_CODE,CODE_MO,REC_AMOUNT,DATA_OT,DATA)\
   values ("+IntToStr(TIP_SCT)
   +",'"+file_info.FILENAME+"',"+

   sqlDateTime(Now()) +","+IntToStr(file_info.error_code_general)
   +","+IntToStr(file_info.CODE_MO)
   +","+IntToStr((int)persons.size())
   +","+sqlDate(file_info.dt_date )  // DATA_OT
   +","+sqlDate(file_info.dt_date ) //  DATA


   +")" ;
   ssLog->Add(sql1);

   q->Close();
   q->SQL->Text=sql1;
   q->ExecSQL();
   ssLog->Add("Line 1102 ");

   int ID_files_log =identCurrent(q,"[DB_SURVEYS].[dbo].[files_log]");
   /////////////////////         */

   /*int hosp_file_header_to_log(
	TADOQuery*q,
	THospFileInfo_T1 file_info,
	int persons_size,
	TStrings * ssLog,
	int TIP_SCT )
*/
   int ID_files_log =  hosp_file_header_to_log(q,file_info,persons.size() , ssLog, TIP_SCT );

   //int IDN_SCT=identCurrent(q,"[DB_SURVEYS].[dbo].[HOS_T1_SCT]");


   //scopeIdentity(q);
   ssLog->Add("Line 1102.111 id "+IntToStr(ID_files_log));//  IDN_SCT) );
   //return IDN_SCT;
	ssLog->Add("Line 1102.22 persons "+IntToStr((int)persons.size()));

   for (int i = 0; i <persons.size()  ; i++) {
	 //
	 
	 ssLog->Add("Line 1103 ");
   String sql;
   // IDN_SCT
   sql= "insert into [DB_SURVEYS].[dbo].[HOS_T1_PAC] ( \
	IDN_SCT ,\
	NOM_NAP  ,\
	DTA_NAP  ,\
	FRM_MP   ,\
	MCOD_NAP ,\
	MCOD_STC ,\
	VPOLIS   ,\
	SPOLIS   ,\
	NPOLIS   ,\
	SMOCOD   ,\
	FAM      ,\
	IM       ,\
	OT       ,\
	W        ,\
	DR       ,\
	TLF      ,\
	DS       ,\
	USL_OK   ,\
	KOD_PFK  ,\
	KOD_PFO  ,\
	KOD_DCT  ,\
	DTA_PLN, faulty, err_codes  ) values ( "   +
   IntToStr(/*IDN_SCT*/ID_files_log )+","+
   /*IntToStr*/str_limit_len_quot(persons[i].NOM_NAP,16)+","+
   sqlDate( persons[i].DTA_NAP)+","+
   IntToStr(persons[i].FRM_MP) +","+
   "'"+str_limit_len(FormatFloat("0000",persons[i].MCOD_NAP),4)+"'" +","+
   "'"+str_limit_len(FormatFloat("0000",persons[i].MCOD_STC),4)+"'" +","+//@@@todo//IntToStr(persons[i].MCOD_STC)+","+    //Реестровый номер медицинской организации, куда направлен пациент
   IntToStr(persons[i].VPOLIS)+","+
   quoted(persons[i].SPOLIS.Trim())+","+
   quoted(persons[i].NPOLIS.Trim())+","+

   quoted(persons[i].SMOCOD.Trim())+","+   // @@to add in XML
   quoted(persons[i].FAM.Trim())+","+
   quoted(persons[i].IM.Trim())+","+
   quoted(persons[i].OT.Trim())+","+

   IntToStr( persons[i].W)+","+
   sqlDate( persons[i].DR)+","  +
   quoted(persons[i].TLF.Trim())+","+
   quoted(persons[i].DS.Trim())+","+
   IntToStr(persons[i].USL_OK)+","+
   IntToStr(persons[i].KOD_PFK)+","+
   IntToStr(persons[i].KOD_PFO)+","+
   IntToStr(persons[i].KOD_DCT)+","+
   sqlDate(persons[i].DTA_PLN)+","+

   IntToStr((int)(persons[i].elvErrors.size()>0))
   +","+elv_errors_to_str(persons[i].elvErrors)
   +
   ")" ;

   ssLog->Add(sql);
   q->Close();
   q->SQL->Text=sql;
   q->ExecSQL();
   }

   return 0;
}




int hosp_to_base_t2(TADOQuery*q, THospFileInfo_T1 file_info, std::vector<TPersonInfoHosp_T2> persons, TStrings * ssLog )
{
//sqlDateTime(0);
   int TIP_SCT=12;
   String sql1=
   "insert into [DB_SURVEYS].[dbo].[HOS_T2_SCT] (TIP_SCT,FILENAME,DTA_OBR)\
   values ("+IntToStr(TIP_SCT)
   +",'"+file_info.FILENAME+"',"+
   //sqlDate(file_info.dt_date)
   sqlDateTime(Now())

   +")" ;

   q->Close();
   q->SQL->Text=sql1;
   if (do_write_old_hosp_log) q->ExecSQL();
   //int IDN_SCT= scopeIdentity(q);

   int ID_files_log =  hosp_file_header_to_log(q,file_info,persons.size(),ssLog,TIP_SCT );

   //return IDN_SCT;


   for (int i = 0; i <persons.size()  ; i++) {
	 //
	 
   String sql;
   // IDN_SCT
   sql= "insert into [DB_SURVEYS].[dbo].[HOS_T2_PAC] ( \
	   [IDN_SCT]  \
	  ,[NOM_NAP]  \
	  ,[DTA_NAP]  \
	  ,[FRM_MP]   \
	  ,[MCOD_STC] \
	  ,[MCOD_NAP] \
	  ,[DTA_FKT]  \
	  ,[TIM_FKT]  \
	  ,[VPOLIS]   \
	  ,[SPOLIS]   \
	  ,[NPOLIS]   \
	  ,[FAM]      \
	  ,[IM]       \
	  ,[OT]       \
	  ,[W]        \
	  ,[DR]       \
	  ,[KOD_PFK]  \
	  ,[KOD_PFO]  \
	  ,[NHISTORY] \
	  ,[DS] ,faulty, err_codes, SMOCOD ) values ( "   +
   IntToStr(/*IDN_SCT*/ID_files_log)+","+
   str_limit_len_quot(persons[i].NOM_NAP,16)+","+
   sqlDate( persons[i].DTA_NAP)+","+
   IntToStr(persons[i].FRM_MP) +","+
   "'"+str_limit_len(FormatFloat("0000",persons[i].MCOD_STC),4)+"'" +","+    //Реестровый номер медицинской организации, куда направлен пациент
   "'"+str_limit_len(FormatFloat("0000",persons[i].MCOD_NAP),4)+"'" +","+

   sqlDate(persons[i].DTA_FKT)+","+
   /*IntToStr*/ sqlTime(persons[i].TIM_FKT)+","+    //
   IntToStr(persons[i].VPOLIS)+","+
   quoted(persons[i].SPOLIS.Trim())+","+
   quoted(persons[i].NPOLIS.Trim())+","+

   quoted(persons[i].FAM.Trim())+","+
   quoted(persons[i].IM.Trim())+","+
   quoted(persons[i].OT.Trim())+","+

   IntToStr( persons[i].W)+","+
   sqlDate( persons[i].DR)+","  +

   quoted(persons[i].KOD_PFK)+","+
   quoted(persons[i].KOD_PFO)+","+

   str_limit_len_quot(persons[i].NHISTORY,50)+","+
   quoted(persons[i].DS)+","+

   IntToStr((int)(persons[i].elvErrors.size()>0))
   +","+elv_errors_to_str(persons[i].elvErrors)
   +","+quoted(persons[i].SMOCOD)
   +")" ;

   q->Close();
   q->SQL->Text=sql;
   q->ExecSQL();
   }

   return 0;
}





//not tested
int hosp_to_base_t3(TADOQuery*q, THospFileInfo_T1 file_info, std::vector<TPersonInfoHosp_T3> persons, TStrings * ssLog )
{
   int TIP_SCT=13;
   String sql1=
   "insert into [DB_SURVEYS].[dbo].[HOS_T3_SCT] (TIP_SCT,FILENAME,DTA_OBR)\
   values ("+IntToStr(TIP_SCT)
   +",'"+file_info.FILENAME+"',"+
   //sqlDate(file_info.dt_date)
   sqlDateTime(Now())
   +")" ;

   q->Close();
   q->SQL->Text=sql1;
   if (do_write_old_hosp_log) q->ExecSQL();
//   int IDN_SCT= scopeIdentity(q);

   int ID_files_log =  hosp_file_header_to_log(q,file_info,persons.size(),ssLog,TIP_SCT );
   //return IDN_SCT;


   for (int i = 0; i <persons.size()  ; i++) {
	 //

   String sql;
   // IDN_SCT
   sql= "insert into [DB_SURVEYS].[dbo].[HOS_T3_PAC] ( \
	   [IDN_SCT]     \
	  ,[MCOD_STC]    \
	  ,[DTA_FKT]     \
	  ,[TIM_FKT]     \
	  ,[VPOLIS]      \
	  ,[SPOLIS]      \
	  ,[NPOLIS]      \
	  ,[FAM]         \
	  ,[IM]          \
	  ,[OT]          \
	  ,[W]           \
	  ,[DR]          \
	  ,[KOD_PFK]     \
	  ,[KOD_PFO]     \
	  ,[NHISTORY]    \
	  ,[DS], faulty ,err_codes, SMOCOD ) values ( "   +
   IntToStr(/*IDN_SCT*/ID_files_log)+","+
   "'"+str_limit_len(FormatFloat("0000",persons[i].MCOD_STC),4)+"'" +","+    //Реестровый номер медицинской организации, куда направлен пациент
   sqlDate(persons[i].DTA_FKT)+","+
   sqlTime(persons[i].TIM_FKT)+","+

   quoted(persons[i].VPOLIS)+","+
   quoted(persons[i].SPOLIS.Trim())+","+
   quoted(persons[i].NPOLIS.Trim())+","+
   quoted(persons[i].FAM.Trim())+","+
   quoted(persons[i].IM.Trim())+","+
   quoted(persons[i].OT.Trim())+","+
   IntToStr( persons[i].W)+","+
   sqlDate( persons[i].DR)+","  +


   str_limit_len_quot(persons[i].KOD_PFK,50)+","+
   str_limit_len_quot(persons[i].KOD_PFO,50)+","+

   str_limit_len_quot(persons[i].NHISTORY,50)+","+
   quoted(persons[i].DS)+","+

   IntToStr((int)(persons[i].elvErrors.size()>0))
   +","+elv_errors_to_str(persons[i].elvErrors)

   +","+quoted(persons[i].SMOCOD.Trim())
   +")" ;

   q->Close();
   q->SQL->Text=sql;
   q->ExecSQL();
   }

   return 0;
}



int hosp_to_base_t4(TADOQuery*q, THospFileInfo_T1 file_info, std::vector<TPersonInfoHosp_T4> persons, TStrings * ssLog )
{
   int TIP_SCT=14;
   String sql1=
   "insert into [DB_SURVEYS].[dbo].[HOS_T4_SCT] (TIP_SCT,FILENAME,DTA_OBR)\
   values ("+IntToStr(TIP_SCT)
   +",'"+file_info.FILENAME+"',"+
   //sqlDate(file_info.dt_date)
   sqlDateTime(Now())
   +")" ;

   q->Close();
   q->SQL->Text=sql1;
   if (do_write_old_hosp_log) q->ExecSQL();
   //int IDN_SCT= scopeIdentity(q);

   int ID_files_log =  hosp_file_header_to_log(q,file_info,persons.size(),ssLog,TIP_SCT );

   //return IDN_SCT;


   for (int i = 0; i <persons.size()  ; i++) {
	 //

   String sql;
   // IDN_SCT
   sql= "insert into [DB_SURVEYS].[dbo].[HOS_T4_PAC] ( \
	   [IDN_SCT]  \
	  ,[NOM_NAP]  \
	  ,[DTA_NAP]  \
	  ,[IST_ANL]  \
	  ,[ACOD]     \
	  ,[PR_ANL], faulty, err_codes   \
		) values ( "   +
   IntToStr(/*IDN_SCT*/ID_files_log)+","+
   str_limit_len_quot(persons[i].NOM_NAP,16)+","+    //Реестровый номер медицинской организации, куда направлен пациент
   sqlDate(persons[i].DTA_NAP)+","+
   IntToStr(persons[i].IST_ANL)+","+
   quoted(persons[i].ACOD)+","+
   IntToStr(persons[i].PR_ANL)+","+

   IntToStr((int)(persons[i].elvErrors.size()>0))
   +","+elv_errors_to_str(persons[i].elvErrors)
   +" ) " ;

   q->Close();
   q->SQL->Text=sql;
   q->ExecSQL();
   }

   return 0;
}






int hosp_to_base_t5(TADOQuery*q, THospFileInfo_T1 file_info, std::vector<TPersonInfoHosp_T5> persons, TStrings * ssLog )
{
   int TIP_SCT=15;
   String sql1=
   "insert into [DB_SURVEYS].[dbo].[HOS_T5_SCT] (TIP_SCT,FILENAME,DTA_OBR)\
   values ("+IntToStr(TIP_SCT)
   +",'"+file_info.FILENAME+"',"+
   //sqlDate(file_info.dt_date)
   sqlDateTime(Now())
   +")" ;

   q->Close();
   q->SQL->Text=sql1;
   if (do_write_old_hosp_log) q->ExecSQL();
   //int IDN_SCT= scopeIdentity(q);

   int ID_files_log =  hosp_file_header_to_log(q,file_info,persons.size(),ssLog,TIP_SCT );

   //return IDN_SCT;


   for (int i = 0; i <persons.size()  ; i++) {
	 //

   String sql;
   // IDN_SCT
   sql= "insert into [DB_SURVEYS].[dbo].[HOS_T5_PAC] ( \
	   [IDN_SCT]   \
	  ,[NOM_NAP]  \
	  ,[DTA_NAP]  \
	  ,[FRM_MP]   \
	  ,[MCOD_STC] \
	  ,[DTA_FKT]  \
	  ,[DTA_END]  \
	  ,[W]        \
	  ,[DR]       \
	  ,[KOD_PFK]  \
	  ,[KOD_PFO]  \
	  ,[NHISTORY] , faulty, err_codes \
	  ) values ( "   +
   IntToStr(/*IDN_SCT*/ID_files_log)+","+
   str_limit_len_quot(persons[i].NOM_NAP,16)+","+    //Реестровый номер медицинской организации, куда направлен пациент
   sqlDate(persons[i].DTA_NAP)+","+
   IntToStr(persons[i].FRM_MP)+","+
   "'"+FormatFloat("0000",persons[i].MCOD_STC)+"'" +","+//quoted(persons[i].MCOD_STC)+","+
   sqlDate(persons[i].DTA_FKT)+","+
   sqlDate(persons[i].DTA_END)+","+
   IntToStr(persons[i].W)+","+
   sqlDate(persons[i].DR)+","+
   IntToStr(persons[i].KOD_PFK)+","+
   IntToStr( persons[i].KOD_PFO)+","+
   /*sqlDate*/str_limit_len_quot( persons[i].NHISTORY, 50)   +","+

   IntToStr((int)(persons[i].elvErrors.size()>0))
   +","+elv_errors_to_str(persons[i].elvErrors)
   +" ) " ;

   q->Close();
   q->SQL->Text=sql;
   q->ExecSQL();
   }

   return 0;
}







int hosp_to_base_t6(TADOQuery*q, THospFileInfo_T1 file_info, std::vector<TPersonInfoHosp_T6> persons, TStrings * ssLog )
{
   int TIP_SCT=16;
   String sql1=
   "insert into [DB_SURVEYS].[dbo].[HOS_T6_SCT] (TIP_SCT,FILENAME,DTA_OBR)\
   values ("+IntToStr(TIP_SCT)
   +",'"+file_info.FILENAME+"',"+
   //sqlDate(file_info.dt_date)
   sqlDateTime(Now())
   +")" ;

   q->Close();
   q->SQL->Text=sql1;
   if (do_write_old_hosp_log) q->ExecSQL();
   //int IDN_SCT= scopeIdentity(q);

   int ID_files_log =  hosp_file_header_to_log(q,file_info,persons.size(),ssLog,TIP_SCT );

   //return IDN_SCT;


   for (int i = 0; i <persons.size()  ; i++) {
	 //
	 
   String sql;
   // IDN_SCT
   sql= "insert into [DB_SURVEYS].[dbo].[HOS_T6_PAC] ( \
	  [IDN_SCT]   \
	  ,[DTA_RAB]  \
	  ,[MCOD_STC] \
	  ,[KOD_PFK]  \
	  ,[KOL_PAC]  \
	  ,[KOL_IN]   \
	  ,[KOL_OUT]  \
	  ,[KOL_PLN]  \
	  ,[KOL_PUS]  \
	  ,[KOL_PUS_M]\
	  ,[KOL_PUS_J]\
	  ,[KOL_PUS_D]\
	  ,[VKOL_FKT] \
	  ,[VUED_FKT], faulty, err_codes ,[KOD_PFO] \
	  ) values ( "   +
   IntToStr(/*IDN_SCT*/ID_files_log)+","+
   sqlDate(persons[i].DTA_RAB)+","+

   "'"+str_limit_len(FormatFloat("0000",persons[i].MCOD_STC),4)+"'" +","+//quoted(persons[i].MCOD_STC)+","+
   quoted(persons[i].KOD_PFK)+","+
   IntToStr(persons[i].KOL_PAC)+","+
   IntToStr(persons[i].KOL_IN)+","+
   IntToStr(persons[i].KOL_OUT)+","+
   IntToStr(persons[i].KOL_PLN)+","+
   IntToStr(persons[i].KOL_PUS)+","+

   IntToStr( persons[i].KOL_PUS_M)+","+
   IntToStr(persons[i].KOL_PUS_J)+","+
   IntToStr( persons[i].KOL_PUS_D)+","+
   IntToStr(persons[i].VKOL_FKT)+","+
   IntToStr( persons[i].VUED_FKT) +","+

   IntToStr((int)(persons[i].elvErrors.size()>0))
   +","+elv_errors_to_str(persons[i].elvErrors)
   +","+quoted(persons[i].KOD_PFO)
   +" ) " ;

   q->Close();
   q->SQL->Text=sql;
   q->ExecSQL();
   }

   return 0;
}


void vect_to_ss(std::vector<String> a, TStrings* ss )
{
   for (int i = 0; i < a.size() ; i++) {
	 ss->Add(a[i]);
   }
}


void getRequiredFields(TADOQuery* q, int n, std::vector<String> & res ,
//std::vector<int> &err_codes
 std::map<String,int>& mpErrorsRequired
)
{
	//
   String sql=
   "select [ID]    \
	  ,[err_code]   \
	  ,[defined_as] \
	  ,[date_begin] \
	  ,[date_end]    \
	  ,[field]       \
	  ,[operator1]   \
	  ,[arg1]        \
	  ,[operator2]   \
	  ,[arg2]        \
	  ,[comment]     \
   FROM [DB_SURVEYS].[dbo].[hosp_error_define] where operator1='required_on_item' ";

   q->Close();
   q->SQL->Text=sql;
   q->Open();

   while(!q->Eof)
   {
	  //
	  String f=q->FieldValues[ "field" ];
	  f=f.UpperCase().Trim();
	  String a=q->FieldValues[ "arg1" ];
	  a=a.UpperCase().Trim();
	  int err_cod  =   q->FieldValues[ "err_code" ];

	  TStringList *sl=new    TStringList;
	  sl->CommaText=a;
	  for (int k = 0; k < sl->Count; k++) {
		//
		String g =sl->Strings[k];
		g=g.Trim();
		if (StrToIntDef(g,-1)==n) {
			//found
			res.push_back(f);
			//err_codes.push_back(err_cod);
            mpErrorsRequired[f]=err_cod;
			break;
		}

	  }
      sl->Free();

      q->Next();
   }


}


int findFieldErrorCode(std::vector<String> fields , std::vector<int> err_codes, String fldSearch)
{
  if (fields.size()!=err_codes.size()) {
	 throw Exception("fields and err_codes sizes are different");
	 return -1;
  }
  for (int i = 0; i < fields.size() ; i++) {
	if (fields[i].UpperCase()==fldSearch.UpperCase() ) {
	  return err_codes[i];
	}
  }
  return -1;
}



void make_error_report_wrong_file( TSurveyFileInfo fi, TStrings* ss )
{
  //
  ss->Add("<REPORT>");
  ss->Add(" <HEADER>");
  ss->Add("  <FILENAME>");
  ss->Add(fi.ORIGINAL_FILENAME);// !!!!!
  ss->Add("  </FILENAME>");

  ss->Add("  <DATE>");
  ss->Add(sqlDate( Now() ));
  ss->Add("  </DATE>");

  /*
  ss->Add("  <CODE_MO>");
  ss->Add(IntToStr(fi.CODE_MO));
  ss->Add("  </CODE_MO>");
    */
  if (fi.wrong_filename){
	 ss->Add("  <ERROR>");
	 ss->Add("  "+IntToStr(ERROR_INCORRECT_FILENAME));
	 ss->Add("  </ERROR>");
  }

  ss->Add(" </HEADER>");
  ss->Add("</REPORT>");

}



void make_error_report_T1( THospFileInfo_T1 fi, std::vector<TPersonInfoHosp_T1> & persons, TStrings* ss , TADOQuery* q)
{
  ss->Add("<?xml version=\"1.0\" encoding=\"windows-1251\" standalone=\"yes\"?>");
  ss->Add("<REPORT>");
  ss->Add(" <HEADER>");
  ss->Add("  <FILENAME>");
  ss->Add(fi.ORIGINAL_FILENAME); //FILENAME);
  ss->Add("  </FILENAME>");

  ss->Add("  <DATE>");
  ss->Add(sqlDate( /*fi.dt_date*/Now() ));
  ss->Add("  </DATE>");

  ss->Add("  <CODE_MO>");
  ss->Add(IntToStr(fi.CODE_MO));
  ss->Add("  </CODE_MO>");

  ss->Add("  <ERROR>");
  ss->Add("  "+IntToStr(fi.error_code_general));
  ss->Add("  </ERROR>");

  ss->Add(" </HEADER>");


  std::vector<TReferenceInfo>  vref_inf, vref_date;
  getReferenceCheckings(q,  vref_inf /*, "'ref','from'"*/);
  getDateTimeCheckings(q,vref_date);

  TStringList * ssPerson=new  TStringList;
  for (int i = 0; i < persons.size() ; i++)
  {
	   ssPerson->Clear();
	/*
	if (persons[i].elvErrors.size()==0) {
		//ss->Add("Person does not contain errors: "+IntToStr(i) );
		continue;
	 }*/
	   ssPerson->Add("  <PERSON>");
	   ssPerson->Add("    <FAM>"+(AnsiString)persons[i].FAM+"</FAM>");
	   ssPerson->Add("    <IM>"+(AnsiString)persons[i].IM+"</IM>");
	   ssPerson->Add("    <OT>"+(AnsiString)persons[i].OT+"</OT>");
	   ssPerson->Add("    <DR>"+sqlDate(persons[i].DR)+"</DR>");
	   for (int k = 0; k < persons[i].svMissing.size(); k++) {
	   //int findFieldErrorCode(persons[i].svRequired,persons[i].ivErrorCodeRequired,persons[i].svMissing[k]);
		int err_code = fi.mpErrorsRequired[persons[i].svMissing[k]];
		ssPerson->Add("    <ERROR>");
		ssPerson->Add("     <DESCRIPTION>Field is missing</DESCRIPTION>");
		ssPerson->Add("     <COMMENT>Поле отсутствует</COMMENT>");  //DESCRIPTION_LCL
		ssPerson->Add("     <FIELD>"+persons[i].svMissing[k]+"</FIELD>");
		ssPerson->Add("     <ERROR_CODE>"+IntToStr(err_code)+"</ERROR_CODE>");
		ssPerson->Add("    </ERROR>");
		persons[i].add_error(err_code,"");
	   }

	   //int performReferenceCheckings(TADOQuery * q, std::vector<TReferenceInfo>  vref_inf, String xml_field, String value, int val_type, int & vref_inf_index, String & error_message )
	   for (int k = 0; k < persons[i].nvvReceived.size(); k++) {
	    // Check correspondence to reference tables
		String xml_field=  persons[i].nvvReceived[k].name;
		String xml_value =  persons[i].nvvReceived[k].value;
		int xml_val_type =  persons[i].nvvReceived[k].val_type;

		int  vref_inf_index; String  error_message, comment;
		int error_code_ref = performReferenceCheckings(q,vref_inf,xml_field,xml_value,xml_val_type,vref_inf_index, error_message, comment);
		if (error_code_ref!=0) {
			ssPerson->Add("    <ERROR>");
			ssPerson->Add("     <DESCRIPTION>"+error_message+"</DESCRIPTION>");
			ssPerson->Add("     <COMMENT>"+comment+"</COMMENT>");
			ssPerson->Add("     <FIELD>"+xml_field+"</FIELD>");
			ssPerson->Add("     <VALUE>"+xml_value+"</VALUE>");
			ssPerson->Add("     <ERROR_CODE>"+IntToStr(error_code_ref)+"</ERROR_CODE>");
			ssPerson->Add("    </ERROR>");
			persons[i].add_error(error_code_ref,"");
		}
		//
		int vref_error_date_index;
		int error_code_date = performDateTimeCheckings(q,vref_date,xml_field,xml_value,xml_val_type,persons[i].nvvReceived ,vref_error_date_index, error_message);
		if (error_code_date!=0) {
			ssPerson->Add("    <ERROR>");
			ssPerson->Add("     <DESCRIPTION>"+error_message+"</DESCRIPTION>");
			ssPerson->Add("     <COMMENT>"+vref_date[vref_error_date_index].comment+"</COMMENT>");
			ssPerson->Add("     <FIELD>"+xml_field+"</FIELD>");
			ssPerson->Add("     <VALUE>"+xml_value+"</VALUE>");
			ssPerson->Add("     <ERROR_CODE>"+IntToStr(error_code_date)+"</ERROR_CODE>");
			ssPerson->Add("    </ERROR>");
			persons[i].add_error(error_code_date,"");
		}
		/*int performDateTimeCheckings(
   TADOQuery * q,
   std::vector<TReferenceInfo>  vref_inf,
   String xml_field,
   String value,
   int val_type,
   std::vector<TNameValueInfo> nvvReceived,
   int & vref_inf_index,
   String & error_message
	);*/

	   }

	   ssPerson->Add("  </PERSON>");

	   if (persons[i].elvErrors.size()>0) {
		  ss->AddStrings(ssPerson);
	   }


  } //for
  ssPerson->Free() ;

  ss->Add("</REPORT>");

}




void make_error_report_T2( THospFileInfo_T1 fi, std::vector<TPersonInfoHosp_T2> & persons, TStrings* ss , TADOQuery* q)
{
  ss->Add("<?xml version=\"1.0\" encoding=\"windows-1251\" standalone=\"yes\"?>");
  ss->Add("<REPORT>");
  ss->Add(" <HEADER>");
  ss->Add("  <FILENAME>");
  ss->Add(fi.FILENAME);
  ss->Add("  </FILENAME>");

  ss->Add("  <DATE>");
  ss->Add(sqlDate( fi.dt_date ));
  ss->Add("  </DATE>");

  ss->Add("  <CODE_MO>");
  ss->Add(IntToStr(fi.CODE_MO));
  ss->Add("  </CODE_MO>");

  ss->Add("  <ERROR>");
  ss->Add("  "+IntToStr(fi.error_code_general));
  ss->Add("  </ERROR>");

  ss->Add(" </HEADER>");


  std::vector<TReferenceInfo>  vref_inf;
  getReferenceCheckings(q,  vref_inf/*,"'ref','from','<=','>='" */);

  std::vector<TReferenceInfo>   vref_date;
  getDateTimeCheckings(q,vref_date);

  TStringList * ssPerson=new  TStringList;
  for (int i = 0; i < persons.size() ; i++)
  {        /*
   if (persons[i].elvErrors.size()==0) {
		//ss->Add("Person does not contain errors: "+IntToStr(i) );
		continue;
	 }       */
	   ssPerson->Clear();
	   ssPerson->Add("  <PERSON>");
	   ssPerson->Add("    <FAM>"+(AnsiString)persons[i].FAM+"</FAM>");
	   ssPerson->Add("    <IM>"+(AnsiString)persons[i].IM+"</IM>");
	   ssPerson->Add("    <OT>"+(AnsiString)persons[i].OT+"</OT>");
	   ssPerson->Add("    <DR>"+sqlDate(persons[i].DR)+"</DR>");
	   for (int k = 0; k < persons[i].svMissing.size(); k++) {
	   //int findFieldErrorCode(persons[i].svRequired,persons[i].ivErrorCodeRequired,persons[i].svMissing[k]);
		int err_code = fi.mpErrorsRequired[persons[i].svMissing[k]];
		ssPerson->Add("    <ERROR>");
		ssPerson->Add("     <DESCRIPTION>Field is missing</DESCRIPTION>");
		ssPerson->Add("     <DESCRIPTION_LCL>Поле отсутствует</DESCRIPTION_LCL>");
		ssPerson->Add("     <FIELD>"+persons[i].svMissing[k]+"</FIELD>");
		ssPerson->Add("     <ERROR_CODE>"+IntToStr(err_code)+"</ERROR_CODE>");
		ssPerson->Add("    </ERROR>");
		persons[i].add_error(err_code,"");      //persons[i].add_error( ,"");
	   }

	   //int performReferenceCheckings(TADOQuery * q, std::vector<TReferenceInfo>  vref_inf, String xml_field, String value, int val_type, int & vref_inf_index, String & error_message )
	   for (int k = 0; k < persons[i].nvvReceived.size(); k++) {
	    // Check correspondence to reference tables
		String xml_field=  persons[i].nvvReceived[k].name;
		String xml_value =  persons[i].nvvReceived[k].value;
		int xml_val_type =  persons[i].nvvReceived[k].val_type;

		int  vref_inf_index; String  error_message, comment;
		int error_code_ref = performReferenceCheckings(q,vref_inf,xml_field,xml_value,xml_val_type,vref_inf_index, error_message, comment);
		if (error_code_ref!=0) {
			ssPerson->Add("    <ERROR>");
			ssPerson->Add("     <DESCRIPTION>"+error_message+"</DESCRIPTION>");
			ssPerson->Add("     <COMMENT>"+comment+"</COMMENT>");
			ssPerson->Add("     <FIELD>"+xml_field+"</FIELD>");
			ssPerson->Add("     <VALUE>"+xml_value+"</VALUE>");
			ssPerson->Add("     <ERROR_CODE>"+IntToStr(error_code_ref)+"</ERROR_CODE>");
			ssPerson->Add("    </ERROR>");
			persons[i].add_error(error_code_ref,"");
		}
        int vref_error_date_index;
		int error_code_date = performDateTimeCheckings(q,vref_date,xml_field,xml_value,xml_val_type,persons[i].nvvReceived ,vref_error_date_index, error_message);
		if (error_code_date!=0) {
			ssPerson->Add("    <ERROR>");
			ssPerson->Add("     <DESCRIPTION>"+error_message+"</DESCRIPTION>");
			ssPerson->Add("     <COMMENT>"+vref_date[vref_error_date_index].comment+"</COMMENT>");
			ssPerson->Add("     <FIELD>"+xml_field+"</FIELD>");
			ssPerson->Add("     <VALUE>"+xml_value+"</VALUE>");
			ssPerson->Add("     <ERROR_CODE>"+IntToStr(error_code_date)+"</ERROR_CODE>");
			ssPerson->Add("    </ERROR>");
			persons[i].add_error(error_code_date,"");
		}

	   }

	   ssPerson->Add("  </PERSON>");
	   if (persons[i].elvErrors.size()>0) {
		  ss->AddStrings(ssPerson);
	   }
  }

  ssPerson->Free() ;

  ss->Add("</REPORT>");

}





void make_error_report_T3( THospFileInfo_T1 fi, std::vector<TPersonInfoHosp_T3> &persons, TStrings* ss , TADOQuery* q)
{
  ss->Add("<?xml version=\"1.0\" encoding=\"windows-1251\" standalone=\"yes\"?>");
  ss->Add("<REPORT>");
  ss->Add(" <HEADER>");
  ss->Add("  <FILENAME>");
  ss->Add(fi.FILENAME);
  ss->Add("  </FILENAME>");

  ss->Add("  <DATE>");
  ss->Add(sqlDate( fi.dt_date ));
  ss->Add("  </DATE>");

  ss->Add("  <CODE_MO>");
  ss->Add(IntToStr(fi.CODE_MO));
  ss->Add("  </CODE_MO>");

  ss->Add("  <ERROR>");
  ss->Add("  "+IntToStr(fi.error_code_general));
  ss->Add("  </ERROR>");

  ss->Add(" </HEADER>");


  std::vector<TReferenceInfo>  vref_inf;
  getReferenceCheckings(q,  vref_inf/*,"'ref','from','<=','>='" */);

  std::vector<TReferenceInfo>   vref_date;
  getDateTimeCheckings(q,vref_date);

  TStringList * ssPerson=new  TStringList;
  for (int i = 0; i < persons.size() ; i++)
  {                           /*
   if (persons[i].elvErrors.size()==0) {
		//ss->Add("Person does not contain errors: "+IntToStr(i) );
		continue;
	 }                          */
	   ssPerson->Clear() ;
	   ssPerson->Add("  <PERSON>");
	   ssPerson->Add("    <FAM>"+(AnsiString)persons[i].FAM+"</FAM>");
	   ssPerson->Add("    <IM>"+(AnsiString)persons[i].IM+"</IM>");
	   ssPerson->Add("    <OT>"+(AnsiString)persons[i].OT+"</OT>");
	   ssPerson->Add("    <DR>"+sqlDate(persons[i].DR)+"</DR>");
	   for (int k = 0; k < persons[i].svMissing.size(); k++) {
	   //int findFieldErrorCode(persons[i].svRequired,persons[i].ivErrorCodeRequired,persons[i].svMissing[k]);
		int err_code = fi.mpErrorsRequired[persons[i].svMissing[k]];
		ssPerson->Add("    <ERROR>");
		ssPerson->Add("     <DESCRIPTION>Field is missing</DESCRIPTION>");
		ssPerson->Add("     <DESCRIPTION_LCL>Поле отсутствует</DESCRIPTION_LCL>");
		ssPerson->Add("     <FIELD>"+persons[i].svMissing[k]+"</FIELD>");
		ssPerson->Add("     <ERROR_CODE>"+IntToStr(err_code)+"</ERROR_CODE>");
		ssPerson->Add("    </ERROR>");
		persons[i].add_error( err_code,"");
	   }

	   //int performReferenceCheckings(TADOQuery * q, std::vector<TReferenceInfo>  vref_inf, String xml_field, String value, int val_type, int & vref_inf_index, String & error_message )
	   for (int k = 0; k < persons[i].nvvReceived.size(); k++) {
	    // Check correspondence to reference tables
		String xml_field=  persons[i].nvvReceived[k].name;
		String xml_value =  persons[i].nvvReceived[k].value;
		int xml_val_type =  persons[i].nvvReceived[k].val_type;

		int  vref_inf_index; String  error_message, comment;
		int error_code_ref = performReferenceCheckings(q,vref_inf,xml_field,xml_value,xml_val_type,vref_inf_index, error_message, comment);
		if (error_code_ref!=0) {
			ssPerson->Add("    <ERROR>");
			ssPerson->Add("     <DESCRIPTION>"+error_message+"</DESCRIPTION>");
			ssPerson->Add("     <COMMENT>"+comment+"</COMMENT>");
			ssPerson->Add("     <FIELD>"+xml_field+"</FIELD>");
			ssPerson->Add("     <VALUE>"+xml_value+"</VALUE>");
			ssPerson->Add("     <ERROR_CODE>"+IntToStr(error_code_ref)+"</ERROR_CODE>");
			ssPerson->Add("    </ERROR>");
			persons[i].add_error(error_code_ref ,"");
		}
        int vref_error_date_index;
		int error_code_date = performDateTimeCheckings(q,vref_date,xml_field,xml_value,xml_val_type,persons[i].nvvReceived ,vref_error_date_index, error_message);
		if (error_code_date!=0) {
			ssPerson->Add("    <ERROR>");
			ssPerson->Add("     <DESCRIPTION>"+error_message+"</DESCRIPTION>");
			ssPerson->Add("     <COMMENT>"+vref_date[vref_error_date_index].comment+"</COMMENT>");
			ssPerson->Add("     <FIELD>"+xml_field+"</FIELD>");
			ssPerson->Add("     <VALUE>"+xml_value+"</VALUE>");
			ssPerson->Add("     <ERROR_CODE>"+IntToStr(error_code_date)+"</ERROR_CODE>");
			ssPerson->Add("    </ERROR>");
			persons[i].add_error(error_code_date ,"");
		}

	   }

	   ssPerson->Add("  </PERSON>");
	   if (persons[i].elvErrors.size()>0) {
		  ss->AddStrings(ssPerson);
	   }
  }

  ss->Add("</REPORT>");
  ssPerson->Free();
}





void make_error_report_T4( THospFileInfo_T1 fi, std::vector<TPersonInfoHosp_T4> &persons, TStrings* ss , TADOQuery* q)
{
  ss->Add("<?xml version=\"1.0\" encoding=\"windows-1251\" standalone=\"yes\"?>");
  ss->Add("<REPORT>");
  ss->Add(" <HEADER>");
  ss->Add("  <FILENAME>");
  ss->Add(fi.FILENAME);
  ss->Add("  </FILENAME>");

  ss->Add("  <DATE>");
  ss->Add(sqlDate( fi.dt_date ));
  ss->Add("  </DATE>");

  ss->Add("  <CODE_MO>");
  ss->Add(IntToStr(fi.CODE_MO));
  ss->Add("  </CODE_MO>");

  ss->Add("  <ERROR>");
  ss->Add("  "+IntToStr(fi.error_code_general));
  ss->Add("  </ERROR>");

  ss->Add(" </HEADER>");


  std::vector<TReferenceInfo>  vref_inf;
  getReferenceCheckings(q,  vref_inf/*,"'ref','from','<=','>='" */);

  std::vector<TReferenceInfo>   vref_date;
  getDateTimeCheckings(q,vref_date);

  TStringList * ssPerson=new  TStringList;
  for (int i = 0; i < persons.size() ; i++)
  {
   if (persons[i].elvErrors.size()==0) {
		//ssPerson->Add("Person does not contain errors: "+IntToStr(i) );
	    continue;
	 }
	   ssPerson->Add("  <PERSON>");
//	   ssPerson->Add("    <FAM>"+(AnsiString)persons[i].FAM+"</FAM>");
//	   ssPerson->Add("    <IM>"+(AnsiString)persons[i].IM+"</IM>");
  //	   ssPerson->Add("    <OT>"+(AnsiString)persons[i].OT+"</OT>");
//	   ssPerson->Add("    <DR>"+sqlDate(persons[i].DR)+"</DR>");
	   for (int k = 0; k < persons[i].svMissing.size(); k++) {
	   //int findFieldErrorCode(persons[i].svRequired,persons[i].ivErrorCodeRequired,persons[i].svMissing[k]);
		int err_code = fi.mpErrorsRequired[persons[i].svMissing[k]];
		ssPerson->Add("    <ERROR>");
		ssPerson->Add("     <DESCRIPTION>Field is missing</DESCRIPTION>");
		ssPerson->Add("     <DESCRIPTION_LCL>Поле отсутствует</DESCRIPTION_LCL>");
		ssPerson->Add("     <FIELD>"+persons[i].svMissing[k]+"</FIELD>");
		ssPerson->Add("     <ERROR_CODE>"+IntToStr(err_code)+"</ERROR_CODE>");
		ssPerson->Add("    </ERROR>");
		persons[i].add_error( err_code,"");
	   }

	   //int performReferenceCheckings(TADOQuery * q, std::vector<TReferenceInfo>  vref_inf, String xml_field, String value, int val_type, int & vref_inf_index, String & error_message )
	   for (int k = 0; k < persons[i].nvvReceived.size(); k++) {
	    // Check correspondence to reference tables
		String xml_field=  persons[i].nvvReceived[k].name;
		String xml_value =  persons[i].nvvReceived[k].value;
		int xml_val_type =  persons[i].nvvReceived[k].val_type;

		int  vref_inf_index; String  error_message, comment;
		int error_code_ref = performReferenceCheckings(q,vref_inf,xml_field,xml_value,xml_val_type,vref_inf_index, error_message, comment);
		if (error_code_ref!=0) {
			ssPerson->Add("    <ERROR>");
			ssPerson->Add("     <DESCRIPTION>"+error_message+"</DESCRIPTION>");
			ssPerson->Add("     <COMMENT>"+comment+"</COMMENT>");
			ssPerson->Add("     <FIELD>"+xml_field+"</FIELD>");
			ssPerson->Add("     <VALUE>"+xml_value+"</VALUE>");
			ssPerson->Add("     <ERROR_CODE>"+IntToStr(error_code_ref)+"</ERROR_CODE>");
			ssPerson->Add("    </ERROR>");
			persons[i].add_error(error_code_ref ,"");
		}
        int vref_error_date_index;
		int error_code_date = performDateTimeCheckings(q,vref_date,xml_field,xml_value,xml_val_type,persons[i].nvvReceived ,vref_error_date_index, error_message);
		if (error_code_date!=0) {
			ssPerson->Add("    <ERROR>");
			ssPerson->Add("     <DESCRIPTION>"+error_message+"</DESCRIPTION>");
			ssPerson->Add("     <COMMENT>"+vref_date[vref_error_date_index].comment+"</COMMENT>");
			ssPerson->Add("     <FIELD>"+xml_field+"</FIELD>");
			ssPerson->Add("     <VALUE>"+xml_value+"</VALUE>");
			ssPerson->Add("     <ERROR_CODE>"+IntToStr(error_code_date)+"</ERROR_CODE>");
			ssPerson->Add("    </ERROR>");
			persons[i].add_error(error_code_date ,"");
		}

	   }

	   ssPerson->Add("  </PERSON>");
	   if (persons[i].elvErrors.size()>0) {
		  ss->AddStrings(ssPerson);
	   }
  }

  ss->Add("</REPORT>");
  ssPerson->Free();
}






void make_error_report_T5( THospFileInfo_T1 fi, std::vector<TPersonInfoHosp_T5> &persons, TStrings* ss , TADOQuery* q)
{
  ss->Add("<?xml version=\"1.0\" encoding=\"windows-1251\" standalone=\"yes\"?>");
  ss->Add("<REPORT>");
  ss->Add(" <HEADER>");
  ss->Add("  <FILENAME>");
  ss->Add(fi.FILENAME);
  ss->Add("  </FILENAME>");

  ss->Add("  <DATE>");
  ss->Add(sqlDate( fi.dt_date ));
  ss->Add("  </DATE>");

  ss->Add("  <CODE_MO>");
  ss->Add(IntToStr(fi.CODE_MO));
  ss->Add("  </CODE_MO>");

  ss->Add("  <ERROR>");
  ss->Add("  "+IntToStr(fi.error_code_general));
  ss->Add("  </ERROR>");

  ss->Add(" </HEADER>");


  std::vector<TReferenceInfo>  vref_inf;
  getReferenceCheckings(q,  vref_inf/*,"'ref','from','<=','>='" */);

  std::vector<TReferenceInfo>   vref_date;
  getDateTimeCheckings(q,vref_date);

  TStringList * ssPerson=new  TStringList;
  for (int i = 0; i < persons.size() ; i++)
  {        /*
   if (persons[i].elvErrors.size()==0) {
		//ss->Add("Person does not contain errors: "+IntToStr(i) );
		continue;
	 }       */
	   ssPerson->Add("  <PERSON>");
//	   ssPerson->Add("    <FAM>"+(AnsiString)persons[i].FAM+"</FAM>");
//	   ssPerson->Add("    <IM>"+(AnsiString)persons[i].IM+"</IM>");
  //	   ssPerson->Add("    <OT>"+(AnsiString)persons[i].OT+"</OT>");
//	   ssPerson->Add("    <DR>"+sqlDate(persons[i].DR)+"</DR>");
	   for (int k = 0; k < persons[i].svMissing.size(); k++) {
	   //int findFieldErrorCode(persons[i].svRequired,persons[i].ivErrorCodeRequired,persons[i].svMissing[k]);
		int err_code = fi.mpErrorsRequired[persons[i].svMissing[k]];
		ssPerson->Add("    <ERROR>");
		ssPerson->Add("     <DESCRIPTION>Field is missing</DESCRIPTION>");
		ssPerson->Add("     <DESCRIPTION_LCL>Поле отсутствует</DESCRIPTION_LCL>");
		ssPerson->Add("     <FIELD>"+persons[i].svMissing[k]+"</FIELD>");
		ssPerson->Add("     <ERROR_CODE>"+IntToStr(err_code)+"</ERROR_CODE>");
		ssPerson->Add("    </ERROR>");
		persons[i].add_error(err_code ,"");
	   }

	   //int performReferenceCheckings(TADOQuery * q, std::vector<TReferenceInfo>  vref_inf, String xml_field, String value, int val_type, int & vref_inf_index, String & error_message )
	   for (int k = 0; k < persons[i].nvvReceived.size(); k++) { //@@@todo always size 0
	    // Check correspondence to reference tables
		String xml_field=  persons[i].nvvReceived[k].name;
		String xml_value =  persons[i].nvvReceived[k].value;
		int xml_val_type =  persons[i].nvvReceived[k].val_type;

		int  vref_inf_index; String  error_message, comment;
		int error_code_ref = performReferenceCheckings(q,vref_inf,xml_field,xml_value,xml_val_type,vref_inf_index, error_message, comment);
		if (error_code_ref!=0) {
			ssPerson->Add("    <ERROR>");
			ssPerson->Add("     <DESCRIPTION>"+error_message+"</DESCRIPTION>");
			ssPerson->Add("     <COMMENT>"+comment+"</COMMENT>");
			ssPerson->Add("     <FIELD>"+xml_field+"</FIELD>");
			ssPerson->Add("     <VALUE>"+xml_value+"</VALUE>");
			ssPerson->Add("     <ERROR_CODE>"+IntToStr(error_code_ref)+"</ERROR_CODE>");
			ssPerson->Add("    </ERROR>");
			persons[i].add_error( error_code_ref,"");
		}
        int vref_error_date_index;
		int error_code_date = performDateTimeCheckings(q,vref_date,xml_field,xml_value,xml_val_type,persons[i].nvvReceived ,vref_error_date_index, error_message);
		if (error_code_date!=0) {
			ssPerson->Add("    <ERROR>");
			ssPerson->Add("     <DESCRIPTION>"+error_message+"</DESCRIPTION>");
			ssPerson->Add("     <COMMENT>"+vref_date[vref_error_date_index].comment+"</COMMENT>");
			ssPerson->Add("     <FIELD>"+xml_field+"</FIELD>");
			ssPerson->Add("     <VALUE>"+xml_value+"</VALUE>");
			ssPerson->Add("     <ERROR_CODE>"+IntToStr(error_code_date)+"</ERROR_CODE>");
			ssPerson->Add("    </ERROR>");
			persons[i].add_error(error_code_date ,"");
		}

	   }

	   ssPerson->Add("  </PERSON>");
	   if (persons[i].elvErrors.size()>0) {
		  ss->AddStrings(ssPerson);
	   }
  }

  ss->Add("</REPORT>");
  ssPerson->Free();
}






void make_error_report_T6( THospFileInfo_T1 fi, std::vector<TPersonInfoHosp_T6> &persons, TStrings* ss , TADOQuery* q)
{
  ss->Add("<?xml version=\"1.0\" encoding=\"windows-1251\" standalone=\"yes\"?>");
  ss->Add("<REPORT>");
  ss->Add(" <HEADER>");
  ss->Add("  <FILENAME>");
  ss->Add(fi.FILENAME);
  ss->Add("  </FILENAME>");

  ss->Add("  <DATE>");
  ss->Add(sqlDate( fi.dt_date ));
  ss->Add("  </DATE>");

  ss->Add("  <CODE_MO>");
  ss->Add(IntToStr(fi.CODE_MO));
  ss->Add("  </CODE_MO>");

  ss->Add("  <ERROR>");
  ss->Add("  "+IntToStr(fi.error_code_general));
  ss->Add("  </ERROR>");

  ss->Add(" </HEADER>");


  std::vector<TReferenceInfo>  vref_inf;
  getReferenceCheckings(q,  vref_inf/*,"'ref','from','<=','>='" */);

  std::vector<TReferenceInfo>   vref_date;
  getDateTimeCheckings(q,vref_date);

  for (int i = 0; i < persons.size() ; i++)
  {
   if (persons[i].elvErrors.size()==0) {
	    //ss->Add("Person does not have errors: "+IntToStr(i) );
	    continue;
	 }
	   ss->Add("  <PERSON>");
//	   ss->Add("    <FAM>"+(AnsiString)persons[i].FAM+"</FAM>");
//	   ss->Add("    <IM>"+(AnsiString)persons[i].IM+"</IM>");
  //	   ss->Add("    <OT>"+(AnsiString)persons[i].OT+"</OT>");
//	   ss->Add("    <DR>"+sqlDate(persons[i].DR)+"</DR>");
	   for (int k = 0; k < persons[i].svMissing.size(); k++) {
	   //int findFieldErrorCode(persons[i].svRequired,persons[i].ivErrorCodeRequired,persons[i].svMissing[k]);
		int err_code = fi.mpErrorsRequired[persons[i].svMissing[k]];
		ss->Add("    <ERROR>");
		ss->Add("     <DESCRIPTION>Field is missing</DESCRIPTION>");
		ss->Add("     <DESCRIPTION_LCL>Поле отсутствует</DESCRIPTION_LCL>");
		ss->Add("     <FIELD>"+persons[i].svMissing[k]+"</FIELD>");
		ss->Add("     <ERROR_CODE>"+IntToStr(err_code)+"</ERROR_CODE>");
		ss->Add("    </ERROR>");
		persons[i].add_error(err_code ,"");
	   }

	   //int performReferenceCheckings(TADOQuery * q, std::vector<TReferenceInfo>  vref_inf, String xml_field, String value, int val_type, int & vref_inf_index, String & error_message )
	   for (int k = 0; k < persons[i].nvvReceived.size(); k++) {
	    // Check correspondence to reference tables
		String xml_field=  persons[i].nvvReceived[k].name;
		String xml_value =  persons[i].nvvReceived[k].value;
		int xml_val_type =  persons[i].nvvReceived[k].val_type;

		int  vref_inf_index; String  error_message, comment;
		int error_code_ref = performReferenceCheckings(q,vref_inf,xml_field,xml_value,xml_val_type,vref_inf_index, error_message, comment);
		if (error_code_ref!=0) {
			ss->Add("    <ERROR>");
			ss->Add("     <DESCRIPTION>"+error_message+"</DESCRIPTION>");
			ss->Add("     <COMMENT>"+comment+"</COMMENT>");
			ss->Add("     <FIELD>"+xml_field+"</FIELD>");
			ss->Add("     <VALUE>"+xml_value+"</VALUE>");
			ss->Add("     <ERROR_CODE>"+IntToStr(error_code_ref)+"</ERROR_CODE>");
			ss->Add("    </ERROR>");
			persons[i].add_error( error_code_ref,"");
		}
        int vref_error_date_index;
		int error_code_date = performDateTimeCheckings(q,vref_date,xml_field,xml_value,xml_val_type,persons[i].nvvReceived ,vref_error_date_index, error_message);
		if (error_code_date!=0) {
			ss->Add("    <ERROR>");
			ss->Add("     <DESCRIPTION>"+error_message+"</DESCRIPTION>");
			ss->Add("     <COMMENT>"+vref_date[vref_error_date_index].comment+"</COMMENT>");
			ss->Add("     <FIELD>"+xml_field+"</FIELD>");
			ss->Add("     <VALUE>"+xml_value+"</VALUE>");
			ss->Add("     <ERROR_CODE>"+IntToStr(error_code_date)+"</ERROR_CODE>");
			ss->Add("    </ERROR>");
			persons[i].add_error(error_code_date ,"");
		}

	   }

	   ss->Add("  </PERSON>");
  }

  ss->Add("</REPORT>");

}






int performReferenceCheckings(TADOQuery * q, std::vector<TReferenceInfo>  vref_inf, String xml_field, String value, int val_type, int & vref_inf_index, String & error_message , String & comment)
{

  error_message="";//  ref_inf.defined_as;
  comment="";
  vref_inf_index=-1;
  TReferenceInfo  ref_inf;
  bool found=0;
  for (int i = 0; i < vref_inf.size(); i++) {
	 if (vref_inf[i].field.UpperCase().Trim()==xml_field.UpperCase().Trim() ) {
		ref_inf=  vref_inf[i];
		found=1;
		vref_inf_index= i;
	 }
  }
  if (found) {
	  //String sql="select "+xml_field  ;
	  if (ref_inf.operator1.LowerCase().Trim()=="ref") {
		//
		/*
        @@@todo: check if opoerator2 = 'last4'
		*/

		String table_name = ref_inf.arg1;
		String table_field = ref_inf.arg2;
		String val_sql=value;
		int err_code=ref_inf.err_code;
		if (val_type==1) { //String
			//val_sql="'"+value+"'"; //its encoded by ref_inf.operator2.Trim().LowerCase()=="fld"
		}
		String sql="";

		if (ref_inf.operator2.Trim().LowerCase()=="fld") {

 //err219  :select * from [DB_NSI].[DBO].MKB_DAG where KOD_MKB like '%I%11.9%'
		  for (int k = 1; k <= val_sql.Length() ; k++) {
			 if (val_sql[k]==' ') {
                 val_sql[k]='%';
			 }
		  }
//		  sql="select "+table_field+" from "+table_name+" where "+table_field+" = '"+ val_sql +"'";

		  sql="select "+table_field+" from "+table_name+" where "+table_field+" like '"+ val_sql +"'";

		}else
		if (ref_inf.operator2.Trim().LowerCase()=="last4") {


		  sql="select "+table_field+" from "+table_name+" where "+table_field+" like '%"+ val_sql+"'" ;

		}else
		if (ref_inf.operator2.Trim().LowerCase()=="int") {


		  sql="select "+table_field+" from "+table_name+" where "+table_field+" = "+ val_sql +"+0";//@@2020nov2 add +0 to save SQL from crash in case of empty value
               //@@@@glitch
		}
		else
		{  //fld
		  sql="select "+table_field+" from "+table_name+" where "+table_field+" = '"+ val_sql +"'";
		}

		try{
		q->Close();
		q->SQL->Text=sql;
		q->Open();
		if (q->RecordCount>0) {
		  //found in table
		  return 0;
		}
		else
		{
		  //not found
		  error_message=  ref_inf.defined_as;
		  comment=ref_inf.comment;
		  return err_code;
		}
		}catch(Exception &e)
		{
		  error_message=e.Message;
		  return 200;
		}

	  }
	  else if (ref_inf.operator1.LowerCase().Trim()=="from")
	  {
		int ifrom =	StrToIntDef(ref_inf.arg1,-1);
		int ito =	StrToIntDef(ref_inf.arg2,-1);
		int ival =  StrToIntDef(value,-1);
		int err_code=ref_inf.err_code;
		if (ifrom==-1 || ito==-1) {
		  error_message="invlid argument of FROM TO";
		  comment="неправильный аргумент FROM TO";
		  return 200;
		}
		if (ival==-1) {
		  error_message="invlid value assessed by FROM TO";
		  comment="неправильное значение проверенное FROM TO";
		  return 200;
		}
		if (ival>=ifrom && ival<=ito ) {
		  //
		  return 0;
		}
		else
		{
          return err_code;
		}

	  }
	  else if (ref_inf.operator1.LowerCase().Trim()=="<="||
	  ref_inf.operator1.LowerCase().Trim()==">="){
		 if (ref_inf.arg2.Trim().LowerCase()=="as_date") {
			String f =  ref_inf.field;
			String arg = ref_inf.arg1;
			return 0;
		 }
	  }

  }
  return 0;
}


void getReferenceCheckings(TADOQuery * q, std::vector<TReferenceInfo> & vref_inf/*, String operat_ */)
{
  	//
   String sql=
   "select [ID]    \
	  ,isnull([err_code],0) as err_code \
	  ,isnull([defined_as],'') as defined_as \
	  ,isnull([date_begin],'') as date_begin \
	  ,isnull([date_end],'') as date_end   \
	  ,isnull([field],'') as   field   \
	  ,isnull([operator1],'') as operator1  \
	  ,isnull([arg1],'') as arg1      \
	  ,isnull([operator2],'') as operator2  \
	  ,isnull([arg2],'') as  arg2      \
	  ,isnull([comment],'') as comment   \
   FROM [DB_SURVEYS].[dbo].[hosp_error_define] where operator1 in ('ref','from')" ;//+operat_+") ";//'ref','from'

   q->Close();
   q->SQL->Text=sql;
   q->Open();

   while(!q->Eof)
   {
	  //
	  String defined_as=q->FieldValues[ "defined_as" ];
	  defined_as=defined_as.Trim();

	  String comment=q->FieldValues[ "comment" ];
	  comment=comment.Trim();

	  String operator1=q->FieldValues[ "operator1" ];
	  operator1=operator1.Trim();

      String arg1=q->FieldValues[ "arg1" ];
	  arg1=arg1.UpperCase().Trim();

	  String operator2=q->FieldValues[ "operator2" ];
	  operator2=operator2.Trim();

	  String arg2=q->FieldValues[ "arg2" ];
	  arg2=arg2.UpperCase().Trim();

	  String field=q->FieldValues[ "field" ];
	  field=field.UpperCase().Trim();

	  int err_code  =   q->FieldValues[ "err_code" ];
	  //

	  TReferenceInfo inf;
	  memset(&inf,0,sizeof(inf));
	  inf.err_code = err_code;
	  inf. defined_as = defined_as;
	  inf. field      = field;
	  inf. operator1  = operator1;
	  inf. arg1       = arg1;
	  inf. operator2  = operator2;
	  inf. arg2       = arg2;
	  inf. comment    = comment;

	  vref_inf.push_back(inf);
	  q->Next();

   }




}




void getDateTimeCheckings(TADOQuery * q, std::vector<TReferenceInfo> & vref_inf/*, String operat_ */)
{
  	//
   String sql=
   "select [ID]    \
	  ,isnull([err_code],0) as err_code \
	  ,isnull([defined_as],'') as defined_as \
	  ,isnull([date_begin],'') as date_begin \
	  ,isnull([date_end],'') as date_end   \
	  ,isnull([field],'') as   field   \
	  ,isnull([operator1],'') as operator1  \
	  ,isnull([arg1],'') as arg1      \
	  ,isnull([operator2],'') as operator2  \
	  ,isnull([arg2],'') as  arg2      \
	  ,isnull([comment],'') as comment   \
   FROM [DB_SURVEYS].[dbo].[hosp_error_define] where operator1 in ('>=','<=','proper_date','proper_time') \
   and arg2 in ('as_date','as_time','as_integer')";


   q->Close();//+operat_+") ";//'ref','from'
   q->SQL->Text=sql;
   q->Open();

   while(!q->Eof)
   {
	  //
	  String defined_as=q->FieldValues[ "defined_as" ];
	  defined_as=defined_as.Trim();

	  String comment=q->FieldValues[ "comment" ];
	  comment=comment.Trim();

	  String operator1=q->FieldValues[ "operator1" ];
	  operator1=operator1.Trim();

      String arg1=q->FieldValues[ "arg1" ];
	  arg1=arg1.UpperCase().Trim();

	  String operator2=q->FieldValues[ "operator2" ];
	  operator2=operator2.Trim();

	  String arg2=q->FieldValues[ "arg2" ];
	  arg2=arg2.UpperCase().Trim();

	  String field=q->FieldValues[ "field" ];
	  field=field.UpperCase().Trim();

	  int err_code  =   q->FieldValues[ "err_code" ];
	  //

	  TReferenceInfo inf;
	  memset(&inf,0,sizeof(inf));
	  inf.err_code = err_code;
	  inf. defined_as = defined_as;
	  inf. field      = field;
	  inf. operator1  = operator1;
	  inf. arg1       = arg1;
	  inf. operator2  = operator2;
	  inf. arg2       = arg2;
	  inf. comment    = comment;

	  vref_inf.push_back(inf);
	  q->Next();

   }




}



TDateTime findDate(std::vector<TNameValueInfo> nvvReceived,String name, bool & found)
{
   found=0;
   for (int i = 0; i < nvvReceived.size(); i++) {
	 if(   nvvReceived[i].name.Trim().LowerCase()
	   ==name.Trim().LowerCase() )
	   {
		  int y,m,d;
		  TDateTime dt;
		  readSqlDate(nvvReceived[i].value,y,m,d);
		  TryEncodeDate(y,m,d,dt);
		  found=1;
		  return dt;
	   }

   }
   return 0;
}



int findInt(std::vector<TNameValueInfo> nvvReceived,String name)
{
   for (int i = 0; i < nvvReceived.size(); i++) {
	 if(   nvvReceived[i].name.Trim().LowerCase()
	   ==name.Trim().LowerCase() )
	   {
		  int t = StrToIntDef(nvvReceived[i].value,0);
		  return t;
	   }
   }
}






int performDateTimeCheckings(
   TADOQuery * q,
   std::vector<TReferenceInfo>  vref_inf,
   String xml_field,
   String value,
   int val_type,
   std::vector<TNameValueInfo> nvvReceived,
   int & vref_inf_index,
   String & error_message
	)
{

  vref_inf_index=-1;
  TReferenceInfo  ref_inf;
  bool found=0;

  int y,m,d;
  TDateTime dt_first_value=0;
  readSqlDate(value,y,m,d);
  TryEncodeDate(y,m,d,dt_first_value);
  if (dt_first_value==(TDateTime)0) {
	  // incorrect date
	  //return 310;   //fires for all


  }


  for (int i = 0; i < vref_inf.size(); i++) {
	 if (vref_inf[i].field.UpperCase().Trim()==xml_field.UpperCase().Trim() ) {
		ref_inf=  vref_inf[i];
		found=1;
		vref_inf_index= i;
	 }
  }
  if (found) {
	  //String sql="select "+xml_field  ;
	  if (ref_inf.operator1.LowerCase().Trim()=="<=" || ref_inf.operator1.LowerCase().Trim()==">=" ) {


		if (ref_inf.arg2.Trim().LowerCase()=="as_date") {

			 TDateTime dt_first_value=0;
			 int y,m,d;
			 readSqlDate(value,y,m,d);
			 TryEncodeDate(y,m,d,dt_first_value);

			 if (dt_first_value==(TDateTime)0) {
				// incorrect date
				error_message="Incorrect date";
				return 310;
			 }


			 bool found=0;
			 TDateTime dt_second_value=findDate( nvvReceived,ref_inf.arg1,found);
			 if (!found) {
				 return 0;
				 /*
				 although we have not found the second value (arg1)
				 the fact that it is missing means that we don't have to
				 assess this condition, therefore return 0 as if
				 everything is fine with the first value (xml_field).
				 Attention <KOD_PFO>71</KOD_PFO> assigned to T019_otd.kod_otd
				 which has long values
				 */
			 }
			 bool res=0;
			 if (ref_inf.operator1.LowerCase().Trim()=="<=")
			 {
				 res=  dt_first_value<=  dt_second_value;
			 }
			 if (ref_inf.operator1.LowerCase().Trim()==">=")
			 {
				 res=  dt_first_value>=  dt_second_value;
			 }
			 if (!res) {
				error_message=ref_inf.defined_as.Trim();
                //vref_inf_index=i;
				return ref_inf.err_code ;
			 }
			 else
			 {
                return 0;
			 }
		}
		else
		if (ref_inf.arg2.Trim().LowerCase()=="as_integer") {
			 int ifirst_value=StrToIntDef(value,0);
			 int isecond_value=findInt(nvvReceived,ref_inf.arg1 );
			 bool res=0;
             if (ref_inf.operator1.LowerCase().Trim()=="<=")
			 {
				 res=  ifirst_value<=  isecond_value;
			 }
			 if (ref_inf.operator1.LowerCase().Trim()==">=")
			 {
				 res=  ifirst_value>=  isecond_value;
			 }
			 if (!res) {
				error_message=ref_inf.defined_as.Trim();
                //vref_inf_index=i;
				return ref_inf.err_code ;
			 }
			 else
			 {
                return 0;
			 }
        }

  }
  }
  return 0;
}






int fileNameInspect_T(String fname, TStrings * ss, TSurveyFileInfo & fi)
{
	//
    /*Имя файла T1-X-RRNN-ГГГГ-MM-ДД.XML
Где T – латинская буква T;
Х – буква М (латинская) для МО, буква F для фонда и буква S для СМО;
RRNN – код МО (четыре последние знака поля MCOD справочника F003, например, для БУЗ "Вологодская городская поликлиника №3" MCOD=352505, следовательно, RRNN=2505), для фонда 0000, для СМО – 0003;
*/
	//
	int err=0;
	String ext=  (ExtractFileExt(fname));
	if (ext.LowerCase()!=".xml")
	{
	  err|=1<<3;
    }
	fi.FILENAME=fname;
	String sshort= ExtractFileName(fname);
	String sFileType = sshort.SubString(1,2);
	fi.sFileType=sFileType;
	ss->Add("sFileType "+sFileType);
	sshort.Delete(1,3);
	String sOrganisationType = sshort.SubString(1,1);
	fi.sOrganisationType=sOrganisationType;
	ss->Add("sOrganisationType "+sOrganisationType);
	sshort.Delete(1,2);
	String sOrganisation = sshort.SubString(1,4);
	fi.CODE_MO=StrToIntDef(sOrganisation,-1);
	if (fi.CODE_MO==-1) {
	   err|=1<<1;
	}
	sshort.Delete(1,5);
	String sDate = sshort.SubString(1,10);
	TDate dt = readSqlDateAsDT(sDate);
	fi.dt_date=dt;
	if (fi.dt_date==(TDate)0) {
	   err|=1<<2;
	}
	ss->Add("sDate "+DateToStr(fi.dt_date));
	//ss->Add("");
	return err;
}


void add_slash(String & s)
{
  s=s.Trim();
  if (s.Length()>0) {
   if (s[s.Length()]!='\\') {
	 s+='\\';
   }
  }
}



String find_or_create_dir(String sDirOutcome, int CODE_MO)
{
  	  add_slash(sDirOutcome);
					  // @@subdirectories
					  String sDirOutFinal = sDirOutcome;
					  String sDirMask = FormatFloat("000000",CODE_MO);
 					   //IntToStr( CODE_MO)+"";
					  //sDirMask.Delete(1,2);
					  while (sDirMask.Length()>4)sDirMask.Delete(1,1);

					  String sSubdirAuto = sDirMask;
					  sDirMask=sDirMask+"*";
					  TStringList *ssSubdirs=new  TStringList;
					  SearchInDir_Subdirs(sDirOutcome,sDirMask,ssSubdirs);
					  if (ssSubdirs->Count>0) {
						  sDirOutFinal=sDirOutFinal+ssSubdirs->Strings[0];
					  }else
					  {
						  try{
						   add_slash(sDirOutFinal);
						   sDirOutFinal=sDirOutFinal+sSubdirAuto+"_auto";
						   ForceDirectories( sDirOutFinal);
						  }catch (Exception &e)
						  {
						   sDirOutFinal=   sDirOutcome;
						  }
					  }
					  ssSubdirs->Free();
					  add_slash(sDirOutFinal);
					  return   sDirOutFinal;
}




String find_or_create_dir_str(String sDirOutcome, String CODE_MO)
{
  	  add_slash(sDirOutcome);
					  // @@subdirectories
					  String sDirOutFinal = sDirOutcome;
					  String sDirMask = //FormatFloat("000000",
					                                    CODE_MO;//);

//					  while (sDirMask.Length()>4)sDirMask.Delete(1,1);

					  String sSubdirAuto = sDirMask;
					  sDirMask=sDirMask+"*";
					  TStringList *ssSubdirs=new  TStringList;
					  SearchInDir_Subdirs(sDirOutcome,sDirMask,ssSubdirs);
					  if (ssSubdirs->Count>0) {
						  sDirOutFinal=sDirOutFinal+ssSubdirs->Strings[0];
					  }else
					  {
						  try{
						   add_slash(sDirOutFinal);
						   sDirOutFinal=sDirOutFinal+sSubdirAuto+"_auto";
						   ForceDirectories( sDirOutFinal);
						  }catch (Exception &e)
						  {
						   sDirOutFinal=   sDirOutcome;
						  }
					  }
					  ssSubdirs->Free();
					  add_slash(sDirOutFinal);
					  return   sDirOutFinal;
}



void delay(int ms)
{
unsigned long  t0;
t0=GetTickCount();
while( GetTickCount()-t0 < ms ) Application->ProcessMessages();
}









