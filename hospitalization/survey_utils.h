//---------------------------------------------------------------------------

#ifndef survey_utilsH
#define survey_utilsH
//---------------------------------------------------------------------------
#include <vector>
#include <Data.DB.hpp>
#include <Data.Win.ADODB.hpp>
#include <map>

class TDirectoryObserver
{
	//
	public:
	String directory;
	void step();
	int ifile_processing;
	TStringList *ssOld;
	TStringList* ssNew;
    TNotifyEvent OnNewFileAdded;

// String sDirScan;
};


class TDirectoryObserverHosp
{
	//
	public:
	String directory;
	void step();
	TStringList *ssOld;
	TStringList* ssNew;
	TNotifyEvent OnNewFileAdded;

// String sDirScan;
};

struct TSurveyInfo{


};


struct TSurveyFileInfo{
 String FILENAME;
 TDateTime dt_date;
 int CODE_MO ;
 TDateTime DATA_OT;
 int rec_emount;
 String sFileType;
 String ORIGINAL_FILENAME;
 String sOrganisationType;
 bool wrong_filename;
};


struct THospFileInfo_T1{
 String FILENAME;
 TDateTime dt_date;
 int CODE_MO ;
 TDateTime DATA_OT;
 int rec_emount;
 String sFileType;
 String ORIGINAL_FILENAME;

 std::vector<String>  svRequired ; //, svReceived ;
// std::vector<String>  svMissing, svDuplicated ;

 std::map<String,int> mpErrorsRequired;

 int error_code_general;
 String error_desc_general;
};



struct TReadFileInfo{
 String FILENAME;
 int CODE_MO ;
 TDate SDATE, //DATA_B
 EDATE;       //DATA_E
};





struct TPersonInfo{
	//
   /*
	ID
LASTNAME	***FAM	фамилия
FIRSTNAME	***IM	имя
PATRONYMIC	***OT	отчество
SEX	***W	пол


MSCARD_NUM	***NPOLIS	номер полиса
ID_Physical	физическое лицо
ID_FILE
  */
  int id;
  int n_rec;
  String lastname, firstname, patronimic;
  int sex;
  String mscard_number;
  int id_physical;
  int id_file;
  TDateTime birth_date;
  //String mscard_number;

  String diagnosis;
  TDateTime start_date, end_date    ;
  String SMOCOD;
  int SURVEY_TYPE;
  unsigned short pers_flags;

};


const int NameValueInfo_TYPE_DATETIME=3;

struct TNameValueInfo
{
   //
   String name;
   String value;
   int val_type;
};


struct TErrorLogInfo{
  //
  int code;
};


struct TPersonInfoHosp{
  String FAM, IM, OT;
  int W;
  TDate DR;

  ////////////
  std::vector<String> // svRequired,
 svReceived ;

 std::vector<TNameValueInfo> nvvReceived;

 std::vector<String>  svMissing, svDuplicated ;

 std::vector<TErrorLogInfo> elvErrors;
 void add_error(int code, String desc);
};



struct TPersonInfoHosp_T1: TPersonInfoHosp{

  unsigned long pers_flags;

  //
  /*int*/String NOM_NAP; //Номер направления RRNNхххххх
  /*
где  RRNN – код МО (cправочник F003, четыре последние знака поля MCOD)
	  хххххх – порядковый        номер внутри МО
	*/
  TDate DTA_NAP; //Дата направления в виде ГГГГ-ММ-ДД
  int FRM_MP; //Форма оказания медицинской помощи 3 – плановая; 2 - неотложная

  int MCOD_NAP;/*Реестровый номер медицинской организации, направившей на госпитализацию	Справочник F003, четыре последние знака поля MCOD*/
  int MCOD_STC;
  int VPOLIS;
  String SPOLIS;
  String NPOLIS;
//  String SMO_OGRN;  //-
//  String ST_OKATO;     //-
//  String FAM, IM, OT;  int W;  TDate DR; //@@inherit
  String TLF;
  String DS;
  int KOD_PFK,KOD_PFO;
  int KOD_DCT  ;
  TDate DTA_PLN;
  int USL_OK;              // db table reviewed and corrected


  String SMOCOD ; //@@@@@@@@ !!!!!!!!!! to add in XML
   /*NOM_NAP
DTA_NAP
FRM_MP
MCOD_NAP
MCOD_STC
VPOLIS
SPOLIS
NPOLIS
SMOCOD       -
FAM
IM
OT
W
DR
TLF
DS
KOD_PFK
KOD_PFO
KOD_DСT
DTA_PLN
USL_OK
*/
		 /*
 std::vector<String> // svRequired,
 svReceived ;

 std::vector<TNameValueInfo> nvvReceived;

 std::vector<String>  svMissing, svDuplicated ;
 */
 // std::vector<int> ivErrorCodeRequired;
 // std::vector<int> ivErrorCodeMissing;
 //std::map<String,int> mpErrorsMissing;

};




struct TPersonInfoHosp_T2:TPersonInfoHosp{

  unsigned long pers_flags;

  //
  String NOM_NAP;    //+
  TDate DTA_NAP;      //+
  int FRM_MP;   //+
  int MCOD_NAP;    //+
  int MCOD_STC;       //+
  int VPOLIS;  //+
  String SPOLIS;         //+
  String NPOLIS;  //+
//  String SMO_OGRN;        //--
//  String ST_OKATO;     //-
//  String FAM, IM, OT;    int W;  TDate DR;  //@@inherit                //+
//  String TLF;                   //-
  String DS;    //+
  String KOD_PFK, //+
  KOD_PFO;        //+
//  String KOD_DCT  ;  //-
//  TDate DTA_PLN;        //-
//  int USL_OK;              //-
  //
  TTime TIM_FKT;              //+
  TDate DTA_FKT;                 //+
  String SMOCOD;    //added

  String NHISTORY;//added          // db table reviewed and corrected
  /*NOM_NAP
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

 std::vector<String> // svRequired,
 svReceived ;

 std::vector<TNameValueInfo> nvvReceived;

 std::vector<String>  svMissing, svDuplicated ;

};



/////////////    T3
struct TPersonInfoHosp_T3 :TPersonInfoHosp
{

  unsigned long pers_flags;
int  MCOD_STC;
/*String*/TDate

  DTA_FKT ;
/*String*/TTime

  TIM_FKT ;
String  VPOLIS  ;
String  SPOLIS  ;
String  NPOLIS  ;
String  SMOCOD  ;   //exist
String  FAM     ;
String  IM      ;
String  OT      ;
int  W       ;
TDate  DR      ;
String  KOD_PFK ;
String  KOD_PFO ;
String  NHISTORY ;
String  DS       ;         // db table reviewed and corrected
//FRM_MP - removed

  /*
std::vector<String> // svRequired,
 svReceived ;

 std::vector<TNameValueInfo> nvvReceived;

 std::vector<String>  svMissing, svDuplicated ;
    */

};
/*MCOD_STC
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
////////////////////



struct TPersonInfoHosp_T4  :TPersonInfoHosp
{

  unsigned long pers_flags;

  String NOM_NAP;          //+
  TDate DTA_NAP;        //+
/*  int FRM_MP;
  int MCOD_NAP;
  int MCOD_STC;
  int VPOLIS;
  String SPOLIS;
  String NPOLIS;
  String SMO_OGRN;
  String ST_OKATO;
  String FAM, IM, OT;
  int W;
  TDate DR;
  String TLF;
  String DS;
  int KOD_PFK,KOD_PFO;
  String KOD_DCT  ;
  TDate DTA_PLN;
  int USL_OK;
  //
  TTime TIM_FKT;
  TDate DTA_FKT;*/
  int IST_ANL;          //+      Источник аннулирования
  String ACOD;          //+      Реестровый номер источника аннулирования
  int PR_ANL;


		 /*
std::vector<String> // svRequired,
 svReceived ;

 std::vector<TNameValueInfo> nvvReceived;

 std::vector<String>  svMissing, svDuplicated ;
           */

   };    //+       // db table reviewed and corrected
/*NOM_NAP
DTA_NAP
IST_ANL
ACOD
PR_ANL
*/









struct TPersonInfoHosp_T5  :TPersonInfoHosp
{

  unsigned long pers_flags;

  String NOM_NAP;     //+
  TDate DTA_NAP;   //+
  int FRM_MP;      //+
//  int MCOD_NAP;
  int MCOD_STC;    //+
  //int VPOLIS;
  //String SPOLIS;
  //String NPOLIS;
/*  String SMO_OGRN;
  String ST_OKATO;*/
  //String FAM, IM, OT;
  int W;                  //+
  TDate DR;               //+
//  String TLF;
  //String DS;
  int KOD_PFK,            //+
  KOD_PFO;                //+
/*  String KOD_DCT  ;
  TDate DTA_PLN;
  int USL_OK; */
  String NHISTORY;           //+

  //
 // TTime TIM_FKT;
  TDate DTA_FKT,        //+
		DTA_END;        //+               // db table reviewed and corrected



   /*
std::vector<String> // svRequired,
 svReceived ;

 std::vector<TNameValueInfo> nvvReceived;

 std::vector<String>  svMissing, svDuplicated ;
     */

};
/*
NOM_NAP
DTA_NAP
FRM_MP
MCOD_STC
DTA_FKT
DTA_END
W
DR
KOD_PFK
KOD_PFO
NHISTORY
*/


/*DTA_RAB>2020-04-03</DTA_RAB>

<MCOD_STC>0001</MCOD_STC>

<KOD_PFK>74</KOD_PFK>

<KOL_PAC>21</KOL_PAC>

<KOL_IN>0</KOL_IN>

<KOL_OUT>2</KOL_OUT>

<KOL_PLN>0</KOL_PLN>

<KOL_PUS>11</KOL_PUS>

<KOL_PUS_M>11</KOL_PUS_M>

<KOL_PUS_J>0</KOL_PUS_J>

<KOL_PUS_D>0</KOL_PUS_D>

<VKOL_FKT>0</VKOL_FKT>

<VUED_FKT*/



struct TPersonInfoHosp_T6 :TPersonInfoHosp
{

 unsigned long pers_flags;
 TDate  DTA_RAB;
 /*String*/int MCOD_STC  ;      //@@todo
 String KOD_PFK    ;
  String KOD_PFO    ;//@@added 2020dec

 int KOL_PAC     ;
 int KOL_IN;
 int KOL_OUT;
 int KOL_PLN ;
 int KOL_PUS  ;
 int KOL_PUS_M ;
 int KOL_PUS_J  ;
 int KOL_PUS_D   ;
 int VKOL_FKT     ;
 int VUED_FKT      ;         // db table reviewed and corrected



	 /*
std::vector<String> // svRequired,
 svReceived ;

 std::vector<TNameValueInfo> nvvReceived;

 std::vector<String>  svMissing, svDuplicated ;
       */
};
/*
DTA_RAB
MCOD_STC
KOD_PFK
KOD_PFO
KOL_PAC
KOL_IN
KOL_OUT
KOL_PLN
KOL_PUS
KOL_PUS_M
KOL_PUS_J
KOL_PUS_D
VKOL_FKT
VUED_FKT
*/




int process_survey_xml(String fn, TSurveyFileInfo &file_info, std::vector<TPersonInfo> &persons , TStrings * ssLog);
int process_read_xml(String fn, TReadFileInfo& file_info, TStrings* ssLog);
AnsiString sqlDate(TDateTime dt );
AnsiString sqlTime(TDateTime dt );
AnsiString sqlDateNoQuotes(TDateTime dt );
AnsiString sqlDateTime(TDateTime dt );
AnsiString surveyTime_NoQuotes(TDateTime dt );

int select_surveys(TADOQuery  * qOmni, int CODE_MO, TDate dateBegin,TDate dateEnd, std::vector<TPersonInfo>& vp , TStrings* ssLog);
void surveys_to_xml(  std::vector<TPersonInfo> vp, String fn, TSurveyFileInfo sfi);
String makeResponseFileName( TReadFileInfo fi );
int find_file_log(TADOQuery  * qOmni, String fn);

void readSqlDate(String s, int &y, int &m,int &d);
void readSqlTime(String s, int &h, int &m);
TDateTime readSqlDateAsDT(String s);
void readSqlTime_colon(String s, int &h, int &m);

TTime hyphenStrToTime(AnsiString s);
TTime hyphenStrToDate(AnsiString s);


String elv_errors_to_str(std::vector<TErrorLogInfo> elvErrors);





////////////////////


int select_surveys(TADOQuery *qOmni, int CODE_MO, TDate dt1, TDate dt2  );



int find_survey(   TADOQuery *qOmni,
	  TDate SDATE     ,
	  TDate EDATE     ,
	  int SMOCOD      ,
	  int DISP_TYP    ,
	  int CODE_MO      ,
	  String DS         ,
	  int ID_PERSON
	  );




int add_survey(   TADOQuery *qOmni,
	  TDate SDATE     ,
	  TDate EDATE     ,
	  int SMOCOD      ,
	  int DISP_TYP    ,
	  int CODE_MO      ,
	  String DS         ,
	  int ID_FILE        ,
	  int ID_PERSON       ,
	  int N_REC
);


int add_file(               // for Surveys system, similar func exists in Hospitalization called hosp_file_header_to_log
	   TADOQuery* qOmni,
	   String FILENAME,
	   String DATA,
	   String DATA_OT,
	   int REC_AMOUNT,
	   int FILE_TYPE,
	   TDateTime PROC_DT,
	   int CODE_MO,
	   int ERROR_CODE );




int fileNameInspect(String fname, TStrings * ss, TSurveyFileInfo & fi);





#endif
