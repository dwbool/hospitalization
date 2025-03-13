unit ats_utils;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, fileCtrl, ComObj;


const
    // excel file columns

    ColTime: Integer = 1;         // time column
    ColEvent: Integer = 2;        // event: connect, disconnect
    ColDialed: Integer = 5;       // dialed phone
    ColCallID: Integer = 8;       // unique id
    ColOwnerID: Integer = 11;     // owner id (room)
    

type
 
 PCallInfo = ^TCallInfo ;
 TCallInfo = record 
  dtConnect:TDateTime;
  dtDisconnect:TDateTime;
  callID:String;
  OwnerID,DialledPhone:String;
 end;

function ATStimeToDelphi(s:String):TDAteTime;
//function DateTimeToATS(dt:TDateTime):String;
function TrimAllUnneccessary(s:String):String;
function CallInfoToText(ci:PCallInfo):String;
function CallsToText(ss:TStrings; LineDelimiter:String):String;

function DateToATS(dt:TDateTime):String;
function TimeToATS(dt:TDateTime):String;

function LastFileNameAsInt(dir:String):Integer;
function LastFileNameAsString(dir:String):String;
function IntFileNameToString( iname:DWORD ) :String;

function IntToFileName(iname:Integer):String;
function FileNameToInt(name:String):DWORD;

function LogTextToCallInfo(S:String; var ci:TCallInfo):Integer;
function FileCallInfo(fname:String ; var ci:TCallInfo):Integer;
function LastFileCallInfo(dir:String; var ci:TCallInfo ):Integer;
function DirPlusName(dir,name:String):String;
function LastFileDisconnectTime(dir:String):TDateTime;
function FileDisconnectTime(sfile:String):TDateTime;

function SortDisconnectTimeAscend(ss:TStrings):Integer;
function SortFileNameAscend(ss:TStrings):Integer;

function OpenSheet( fname:String ; var ExcelFile:Variant ):Variant;                   // MaxRow,MaxCol:Integer;
function ScanSheet(ssCalls:TStringList; Sheet : Variant; dtLastDisconnect:TDateTime ):Integer;
function CloseSheet(ExcelFile:Variant):Variant;

function ReadCallsLaterThan(dir:String; dtLastDisconnect:TDateTime; iFileLast:Integer; ssCalls:TStringList ):Integer;

procedure CallsToFiles(ssCalls:TStringList ; sDir:String; iFileNameStart:Integer; dtLastDisconnect:TDateTime );

function FindFileDateTime(fname:String):TDateTime;
function FindFileDateTimeAndSize(fname:String; var size:Integer):TDateTime;


procedure DestroyCallsInfo(ssCalls:TStringList);


      
implementation

uses DateUtils;


procedure DestroyCallsInfo(ssCalls:TStringList);
var i:Integer; pc:PCallInfo;
begin

for I:=0 to ssCalls.Count-1 do
 begin
  //
  pc:=PCallInfo(ssCalls.Objects[i]);
  Dispose(pc);

 end;
ssCalls.Clear;

end;


function FindFileDateTimeAndSize(fname:String; var size:Integer):TDateTime;
var r:TSearchRec; res:Integer;  dt:TDateTime;
begin
  Result:=0;
  res:= FindFirst(fname,-1,r);

  //r.Time

  //form1.m2.Lines.Add(Inttostr(r.Time)) ;
  //form1.m2.Lines.Add(DateTimetostr(dt)) ;

  if res=0 then
   begin
   size:=r.Size;
   dt:=FileDateToDateTime(r.Time);
   Result:=dt;

   end;
  FindClose(r);
end;



function FindFileDateTime(fname:String):TDateTime;
var r:TSearchRec; res:Integer;  dt:TDateTime;
begin
  Result:=0;
  res:= FindFirst(fname,-1,r);

  //r.Time

  //form1.m2.Lines.Add(Inttostr(r.Time)) ;
  //form1.m2.Lines.Add(DateTimetostr(dt)) ;

  if res=0 then
   begin
   dt:=FileDateToDateTime(r.Time);
   Result:=dt;

   end;
  FindClose(r);
end;


function ReadCallsLaterThan(dir:String; dtLastDisconnect:TDateTime; iFileLast:Integer; ssCalls:TStringList ):Integer;
var iFile:Integer; sFile:String;   ci:PCallInfo;
begin
ssCalls.Clear;
Result:=0;
iFile:= iFileLast;
repeat
sFile:= dirPlusname(Dir, IntToFileName(iFile));
if FileExists(sFile) then
 begin
 new(ci);
 FileCallInfo(sFile, ci^);
 if ci.dtDisconnect < dtLastDisconnect then
  begin
  Dispose(ci);
  break;
  end;
 ssCalls.AddObject(sFile,TObject(ci));
 end;
iFile:=iFile-1;
until (iFile<0)or((iFileLast-iFile)>10000);
end;


function CallInfoToText_NoCallID(ci:PCallInfo):String;
var s,sTime,sDate,sDuration:String; secBtw,dh,dm,ds:Integer;
begin
 {s:=DateTimetostr(ci.dtConnect)+ ' ' + DateTimetostr(ci.dtDisConnect) + ' ' +ci.OwnerID+' '+ci.callID;}
 sTime:=  TimeToATS(ci.dtConnect); // time of call
 sDate:=  DateToATS(ci.dtConnect);

 secBtw:= SecondsBetween(  ci.dtConnect, ci.dtDisConnect); //duration in seconds
 dh:= secBtw div 3600;
 dm:= (secBtw - dh*3600) div 60;
 ds:= (secBtw - dh*3600) - dm*60;
 sDuration := FormatFloat('00:',dh)+FormatFloat('00:',dm)+FormatFloat('00',ds);

 s:=sDate+' '+sTime+' '+sDuration;    // output string
 while Length(s)<30 do s:=s+' ';
 s:=s+ci.DialledPhone;                // from 31 - dialled phone
 while Length(s)<70 do s:=s+' ';
 s:=s+'O';
 while Length(s)<75 do s:=s+' ';
 s:=s+ci.OwnerID;
 while Length(s)<79 do s:=s+' ';
 s:=s+#13#10;
 Result:=s;
end;

function CallsAreSame(ci1,ci2:PCallInfo):Boolean;
var s1,s2:String;
begin
s1:= CallInfoToText(ci1);
s2:= CallInfoToText(ci2);
Result:= {(ci1.dtConnect=ci2.dtConnect)and
    (ci1.dtdisConnect=ci2.dtdisConnect)and
     (ci1.OwnerID=ci2.OwnerID)and
     (ci1.DialledPhone=ci2.DialledPhone);
     }
s1 =  s2;
end;

function SameCallInList(ci:PCallInfo ; ssCalls:TStrings):Integer;
var i:Integer;
begin

 Result:=-1;
 for i:=0 to ssCalls.Count-1 do
  begin
   if CallsAreSame( PCallInfo(ssCalls.Objects[i]), ci) then
    begin
     //
     Result:=i;
     break;
    end;
  end;

end;


procedure CallsToFiles(ssCalls:TStringList ; sDir:String; iFileNameStart:Integer; dtLastDisconnect:TDateTime );
var i,iFileNum : Integer; s,sLine:String; sFName:String;
ssCallLines,ssLine:TStringList;
//
ssCallsLatest :TStringList;
ci:  PCallInfo;
begin
if not DirectoryExists(sDir) then ForceDirectories(sDir);

SortDisconnectTimeAscend(ssCalls);


//  000012FA.$qi

ssCallsLatest := TStringList.Create;   // recent calls with Discon time same as  dtLastDisconnect
ReadCallsLaterThan(sDir,  dtLastDisconnect,  iFileNameStart-1,  ssCallsLatest);
//form1.m2.Lines.Add('##latest calls list');
for i :=0  to ssCallsLatest.Count-1 do
 begin
  //
  //form1.m2.Lines.Add('## '+CallInfoToText( PCallInfo( ssCallsLatest.Objects[i]) ));
 end;



s:= CallsToText(ssCalls,'');
ssCallLines:=TStringList.Create;
ssLine:=TStringList.Create;

ssCallLines.Text:=s;
 //
iFileNum:=   iFileNameStart;
for i :=0  to ssCalls.Count-1 do
 begin
 ci:=  PCallInfo(ssCalls.Objects[i]);

 //form1.m2.Lines.Add('comparing to list ## '+CallInfoToText( PCallInfo( ci ) ));
 if SameCallInList(ci, ssCallsLatest) =-1 then
  begin
  //form1.m2.Lines.Add('## list does not contain the call');
  //   this call not written into file yet
  sLine:= CallInfoToText( ci  );
  ssLine.Clear;
  ssLine.Add(sLine);
  sFName:=Inttohex( iFileNum, 8 )+'.$qi';

  ssLine.SaveToFile(dirPlusName (sDir, sFName));
  end
 else
  begin
    //form1.m2.Lines.Add('## list contains the call, no need to Add');
  end; 
  //
  iFileNum:= iFileNum+1;
 end;

 //m2.Lines.Text:=s;


end;


function OpenSheet( fname:String ; var ExcelFile:Variant ):Variant;
var
   // ExcelFile : Variant;
    WorkBook : Variant;
    Sheet : Variant;

begin
// Open Excel OLE
   ExcelFile :=  CreateOleObject('Excel.Application');
   // Handle WoorkBook
   if not VarIsNull(ExcelFile) then
     begin
       WorkBook := ExcelFile.WorkBooks.Open(fname);
       if not VarIsNull(WorkBook) then
         begin

           // Handle Sheet
           Sheet := WorkBook.WorkSheets.Item[1];  //['yourSheetName'];

           //MaxRow := Sheet.Usedrange.EntireRow.count;
           //MaxCol := sheet.Usedrange.EntireColumn.count;

           //ScanSheet(ssCalls, sheet, maxrow, maxcol);
           Result:=sheet;
         end;

     end;

   //Later when want close excel.
   //ExcelFile.DisplayAlerts := False; // Discard unsaved files....
   //ExcelFile.Quit;
   //ExcelFile:=Unassigned;
end;

function CloseSheet(ExcelFile:Variant):Variant;
begin
  ExcelFile.DisplayAlerts := False; // Discard unsaved files....
  ExcelFile.Quit;
  ExcelFile:=Unassigned;
end;

 {
procedure SearchInDir(s,mask:String; proc:TSrcproc);
var res:Integer; dir,ext:String; r:TSearchRec;
begin

 try
 FillChar(r,sizeof(r),0);

  try
  res:=FindFirst(s+'\'+mask,-1,r);
  while res=0 do
   begin
   if (r.Name<>'.')and(r.Name<>'..' ) then
   if (r.Attr and faDirectory)>0 then
    begin
     // if wanna process subdirectory
    end
   else
    begin
    
     ext:=AnsiLowerCase(ExtractFileExt(r.Name));
     //

    end;
   //if Perform then break; // new directory clicked
   res:=FindNext(r);
   end;

  finally
  FindClose(r);
  end;

 except
 end;

end;
 }

function ScanSheet(ssCalls:TStringList; Sheet : Variant;  dtLastDisconnect:TDateTime ):Integer;          //MaxRow :Integer ;
var row, callIndex:Integer;
    sTime, sEvent, sCallID, sOwnerID, sDialed   :String;

    ci:pCallInfo;
    MaxRow :Integer ;    dtDisconnect:TDateTime;
begin
 Result:=0;

 DestroyCallsInfo(ssCalls);

 MaxRow :=  Sheet.Usedrange.EntireRow.count; ;

 for  row:= 1  to MaxRow do
  begin
   sTime:=  sheet.cells.item[row,colTime].value;
   sEvent:=  TrimAllUnneccessary( sheet.cells.item[row,colEvent].value );
   sCallID:=  TrimAllUnneccessary( sheet.cells.item[row,colCallID].value );
   sOwnerID:= TrimAllUnneccessary( sheet.cells.item[row,colOwnerID].value );
   sDialed:=  TrimAllUnneccessary( sheet.cells.item[row,colDialed].value );
   sEvent:=Lowercase(sEvent);
   while Length(sEvent)>0 do
    begin
    if not (sEvent[1] in ['a'..'z']) then Delete(sEvent,1,1) else break;
    end;
   while Length(sEvent)>0 do
    begin
    if not (sEvent[Length(sEvent)] in ['a'..'z']) then Delete(sEvent,Length(sEvent),1) else break;
    end;
   {
   Время                     Событие      ID звонка                 ID владельца
   2014-02-14 18:20:58.594   Disconnect   26a79ada958311e3a3c7001a4bde0300   105 
   }

   if Lowercase(Trim(sEvent))='disconnect' then
    begin
      dtDisconnect:= ATStimeToDelphi(sTime);
      if dtDisconnect > dtLastDisconnect then
       begin               // hase not yet been written into files coz its Disconnect time later than  dtLastDisconnect, extracted from LastFile
        new(ci);
        ci.dtDisconnect :=  dtDisconnect;
        ci.callID:=sCallID;
        ci.OwnerID:=sOwnerID;
        ci.DialledPhone:= sDialed;
        ssCalls.AddObject(  sCallID, TObject(ci)); // disconnect time into struct
       end;
    end else
   if Lowercase(sEvent)='connect' then
    begin
      callIndex:=ssCalls.indexOf(sCallID);
      if callIndex>-1 then
       begin
         ci:= PCallInfo(ssCalls.Objects[callIndex]);
         ci.dtConnect:= ATStimeToDelphi(sTime);  // connect time into struct

       end;
    end;


   //form1.m.Lines.Add(sTime+' '+sEvent+' '+sCallID+' '+sOwnerID);

  end;


end;
 

function SortDisconnectTimeAscend(ss:TStrings):Integer;
var i:Integer;  ci1,ci2:PCallInfo; iteration:Integer; chg:Boolean;
begin
iteration:=0;
repeat
chg:=false;
for i:=0 to ss.Count-1-1-iteration do
 begin
  ci1:= PCallInfo(  ss.Objects[i] );
  ci2:= PCallInfo(  ss.Objects[i+1] );
  if ci1.dtDisconnect > ci2.dtDisconnect then
   begin
    ss.Exchange(i,i+1);
    chg:=true;
   end;
 end;
iteration:=    iteration+1;
until not chg;
Result:=    iteration;
end;


function SortFileNameAscend(ss:TStrings):Integer;
var i:Integer;  iteration:Integer; chg:Boolean;  iName1,iName2:DWORD;
begin
iteration:=0;
repeat
chg:=false;
for i:=0 to ss.Count-1-1-iteration do
 begin
  iName1:=FileNameToInt( ss[i]);
  iName2:=FileNameToInt( ss[i+1]);
  if iName1 > iName2 then
   begin
    ss.Exchange(i,i+1);
    chg:=true;
   end;
 end;
iteration:=    iteration+1;
until not chg;
Result:=    iteration;
end;


function LastFileDisconnectTime(dir:String):TDateTime;
var ci:TCallInfo;
begin
 LastFileCallInfo(dir, ci);
 Result:=ci.dtDisconnect;
end;

function FileDisconnectTime(sfile:String):TDateTime;
var ci:TCallInfo;
begin
 FileCallInfo(sfile, ci);
 Result:=ci.dtDisconnect;
end;


function DirPlusName(dir,name:String):String;
begin
dir:=trim(dir);
if dir[Length(dir)]<>'\' then dir:=dir+'\';
Result:=dir+name;
end;

function LastFileCallInfo(dir:String; var ci:TCallInfo ):Integer;
var fn:String;
begin
 fN:=LastFileNameAsString(dir);
 fn:=DirPlusName(dir,fn);
 Result:=FileCallInfo(fn, ci);
end;

function FileCallInfo(fname:String ; var ci:TCallInfo):Integer; // 0 -no errors
var ss:TStringlist;
begin
 Result:=0;
 FillChar(ci,sizeof(ci),0);
 if FileExists(fname) then
  begin
  ss:=TStringlist.Create;
  ss.LoadFromFile(fname);
  if ss.Count>0 then
   begin
    LogTextToCallInfo(ss[0], ci);
   end;
  end else Result:=1;    // no file
end; 


function HexToIntNormalOrder(sHex:String):Integer;
var  sHexBin:String;  iFileNum,i,iChar:Integer;
begin
sHexBin:='';  // to use HexToBin less significant byte must be on the left, unlike normal number writing
for i:=0 to 3 do
 begin
  iChar:=Length(sHex) - i*2;
  sHexBin:= sHexBin+ sHex[iChar-1] +  sHex[iChar];  // reverse bytes order  000012FA -> FA120000
 end;
HexToBin(pchar(sHexBin),@iFileNum,4);
Result:= iFileNum;
end;

function IntFileNameToString( iname:DWORD ) :String;
begin
Result:= IntToHex(iname,8)+'.$qi';
end;

function LastFileNameAsString(dir:String):String;
var iname:Integer;
begin
iname:=  LastFileNameAsInt(dir);
Result:= IntFileNameToString( iname);
//IntToHex(iname,8)+'.$qi';
end;



function IntToFileName(iname:Integer):String;
begin
Result:= IntToHex(iname,8)+'.$qi';
end;



function FileNameToInt(name:String):DWORD;
var iName,p:DWORD;
begin
    try
     name:=AnsiUpperCase(ExtractFileName(Name));
     p:=Pos('.',name);
     if p>0 then name:=copy(name,1,p-1);
     iName:=HexToIntNormalOrder( name);
     Result:=iName;
    except
     Result:=0;
    end;
end;

 
function LastFileNameAsInt(dir:String):Integer;
var res,p,iName,inameMax:Integer; name:String; r:TSearchRec;
begin
 Result:=0;
 inameMax:=0;
 try
 FillChar(r,sizeof(r),0);
  try
  dir:=trim(dir);
  if dir[length(dir)]<>'\' then dir:=dir+'\';
  res:=FindFirst(dir+''+'*.$qi',-1,r);
  while res=0 do
   begin
   //if (r.Name<>'.')and(r.Name<>'..' ) then
   if (r.Attr and faDirectory)>0 then
    begin
     // if wanna process subdirectory
    end
   else
    begin

     name:=AnsiUpperCase(ExtractFileName(r.Name));
     p:=Pos('.',name);
     if p>0 then name:=copy(name,1,p-1);
     iName:=HexToIntNormalOrder( name);
     if iName>inameMax then
      begin
        iNameMax:=iName;
      end;
     //

    end;
   //if Perform then break; // new directory clicked
   res:=FindNext(r);
   end;

  finally
  FindClose(r);
  end;
  //sFName:=Inttohex( iNameMax, 8 )+'.$qi';
  Result:=iNameMax ;//sFName;
 except
 end;

end;



function DateToATS(dt:TDateTime):String;
var y,m,d,h,min,sec,ms:word; s:String;
begin
DecodeDateTime( dt,y,m,d,h,min,sec,ms);
s:= IntTostr(y)+'-'+
    FormatFloat('00',m)+'-'+
    FormatFloat('00',d)    ;
Result:=s;
end;

function TimeToATS(dt:TDateTime):String;
var y,m,d,h,min,sec,ms:word; s:String;
begin
DecodeDateTime( dt,y,m,d,h,min,sec,ms);
s:= FormatFloat('00',h)+':'+
    FormatFloat('00',min)+':'+
    FormatFloat('00',sec);
Result:=s;     
end;

function CallsToText(ss:TStrings; LineDelimiter:String):String;
var i:Integer; line:String;
begin
 Result:='';
 for i:=0 to ss.Count-1 do
  begin
   line:=  CallInfoToText(PCallInfo(ss.Objects[i]));
   Result := Result + line+  LineDelimiter;
  end;
end;


function SplitString(s,delim:String ; ss:TStrings):Integer;
var p:Integer;
begin

 ss.Clear;
 repeat
 s:=Trim(s);
 p:= Pos(delim,s);
 if p>0 then
  begin
   //
   ss.Add(trim(copy(s,1,p-1)));
   Delete (s,1,p+Length(delim)-1);

  end else break;
 until false;
 ss.Add(trim(s));
 Result:=ss.Count;

end;

function LogTextToCallInfo(S:String; var ci:TCallInfo):Integer;
var ss:TStringlist;    dtDur:TDateTime;
begin
Result:=0;
fillchar(ci,sizeof(ci),0);
ss:=TStringlist.create;
splitString(s,' ',ss);
if ss.count>=6 then
 begin
 ci.dtConnect:=ATStimeToDelphi( ss[0]+' '+ss[1] );
 dtDur:= StrToTime(ss[2]);
 ci.dtDisconnect:=ci.dtConnect+ dtDur;
 ci.callID  :='unknown'; //here
 ci.DialledPhone:=ss[3];
 ci.OwnerID:=ss[5];
 // 2014-02-14 18:20:31 00:00:27  110                                     O    105 //
 end;

end;


function CallInfoToText(ci:PCallInfo):String;
var s,sTime,sDate,sDuration:String; secBtw,dh,dm,ds:Integer;
begin
 {s:=DateTimetostr(ci.dtConnect)+ ' ' + DateTimetostr(ci.dtDisConnect) + ' ' +ci.OwnerID+' '+ci.callID;}
 sTime:=  TimeToATS(ci.dtConnect); // time of call
 sDate:=  DateToATS(ci.dtConnect);

 secBtw:= SecondsBetween(  ci.dtConnect, ci.dtDisConnect); //duration in seconds
 dh:= secBtw div 3600;
 dm:= (secBtw - dh*3600) div 60;
 ds:= (secBtw - dh*3600) - dm*60;
 sDuration := FormatFloat('00:',dh)+FormatFloat('00:',dm)+FormatFloat('00',ds);

 s:=sDate+' '+sTime+' '+sDuration;    // output string
 while Length(s)<30 do s:=s+' ';
 s:=s+ci.DialledPhone;                // from 31 - dialled phone
 while Length(s)<70 do s:=s+' ';
 s:=s+'O';
 while Length(s)<75 do s:=s+' ';
 s:=s+ci.OwnerID;
 while Length(s)<79 do s:=s+' ';
 s:=s+#13#10;
 Result:=s;
end;

{Название блока	Символы	Формат	Пример
Дата начала соединения	1-10	любой	27042007
27/04/2007
2007-04-27
Время начала соединения	11-20	любой	13:29:56
13/29/56
132956
Продолжительность соединения	21-30	Hh:mm:ss	00:12:42
Набранный номер	31-70		83452288654
Признак исходящего звонка	71-75	символ	O
Внутренний номер телефона, с которого осуществлен звонок	76-79		1812
Символы окончания строки и возврата каретки	80-72	Данное поле не обязательно	0D0A

2014-02-14 18:20:31 00:00:27  110                                     O    105 //
2014-02-14 18:20:31 00:00:27  110                                     O    105
2014-02-14 18:20:14 00:00:04  111                                     O    105
2014-02-14 18:20:14 00:00:04  111                                     O    105 }


function TrimAllUnneccessary(s:String):String;
var bt:char; len:Integer;  NeccessarySymbols : set of char;
begin
NeccessarySymbols:=  ['a'..'z','A'..'Z','0'..'9','-',':','.'];
while Length(s)>0 do
 begin
 bt:=(s[1]);  
 if not (bt in NeccessarySymbols) then Delete(s,1,1) else break;
 end;
while true do
 begin
 len:= Length(s);
 if len=0 then break;
 bt:=(s[len]);
 if not (bt in NeccessarySymbols) then Delete(s,Len,1) else break;
 end;
Result:=s;
end;


function ATStimeToDelphi(s:String):TDAteTime;
var p, y,m,d,h,min,sec,ms:Integer;
    sy,sm,sd,sh,smin,ssec,sms:String;
    dtTime,dtDate:TDateTime;
begin
p:=Pos('-',s);
if p>0 then
 begin
 sy:=TrimAllUnneccessary(Copy(s,1,p-1));
 Delete(s,1,p);
 end;
p:=Pos('-',s);
if p>0 then
 begin
 sm:=TrimAllUnneccessary(Copy(s,1,p-1));
 Delete(s,1,p);
 end;
p:=Pos(' ',s);
if p>0 then
 begin
 sd:=TrimAllUnneccessary(Copy(s,1,p-1));
 Delete(s,1,p);
 end;
p:=Pos(':',s);
if p>0 then
 begin
 sh:=TrimAllUnneccessary(Copy(s,1,p-1));
 Delete(s,1,p);
 end;
p:=Pos(':',s);
if p>0 then
 begin
 smin:=TrimAllUnneccessary(Copy(s,1,p-1));
 Delete(s,1,p);
 end;
p:=Pos('.',s);
if p>0 then
 begin
 ssec:=TrimAllUnneccessary(Copy(s,1,p-1));
 Delete(s,1,p);
 end else
 begin
 ssec:=s;
 s:='0';
 end;
sms:=TrimAllUnneccessary(s);
y:=StrToIntDef(  sy,-1);
m:=StrToIntDef(  sm,-1);
d:=StrToIntDef(  sd,-1);
h:=StrToIntDef(  sh,-1);
min:=StrToIntDef(  smin,-1);
sec:=StrToIntDef(  ssec,-1);
ms:=StrToIntDef(  sms,-1);
dtTime:= EncodeTime(h,min,sec,ms);
dtDate:= EncodeDate(y,m,d);
Result:= Trunc(dtDate) + ( dtTime - trunc(dtTime) ) ;
end;



end.
