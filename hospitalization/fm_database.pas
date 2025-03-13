unit fm_database;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.StdCtrls, inifiles, dm_base,
  Vcl.Buttons, Data.DB, Data.Win.ADODB;

type
  TfmDatabase = class(TForm)
    Label1: TLabel;
    edConn: TComboBox;
    btConnect: TButton;
    btDisconnect: TButton;
    lbStatus: TLabel;
    btExit: TButton;
    btBuild: TSpeedButton;
    Label2: TLabel;
    btRecreateTables: TSpeedButton;
    qCreate: TADOQuery;
    qCreateID: TAutoIncField;
    qCreateNumber: TWideStringField;
    qCreatename: TWideStringField;
    qCreateSumm: TBCDField;
    qCreateCreatedDate: TDateTimeField;
    qCreateUser_ID: TAutoIncField;
    Label3: TLabel;
    procedure btConnectClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure btDisconnectClick(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure btUpdateControlsClick(Sender: TObject);
    procedure btExitClick(Sender: TObject);
    procedure btBuildClick(Sender: TObject);
    procedure btRecreateTablesClick(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
    editAllowed:Boolean;
  end;

var
  fmDatabase: TfmDatabase;
  ini:TInifile;


implementation

uses UItypes;

{$R *.dfm}

procedure TfmDatabase.btConnectClick(Sender: TObject);
var s,sIniStr:String;   i,iEmp:Integer;  found:Boolean;
begin
 ini.WriteString('database','connection_string',edConn.Text);    //
 ini.WriteString('connection_versions','1',edConn.Text);         //

 dm_base.  DataModuleBase.ADOConnection.Connected:=false;
 dm_base.  DataModuleBase.ADOConnection.ConnectionString:=  edConn.Text;

 s:=ini.ReadString('database','connection_string',edConn.Text);
 if s<>edConn.Text then
  begin

    found:=false;
    iEmp:=255;
    for I := 0 to 255 do
     begin
       sIniStr:=    ini.ReadString('connection_versions',inttostr(i),'');
       if sIniStr=edConn.Text then
        begin
           found:=true;
           ini.WriteString('connection_versions',inttostr(i),edConn.Text); //
           break;
        end;
       if sIniStr='' then
        if iEmp=-1 then iEmp:=i;
     end;
    if not found then  ini.WriteString('connection_versions',inttostr(iEmp),edConn.Text);
  end;


 try
  dm_base.  DataModuleBase.ADOConnection.Connected:=true;
  ini.writeString('database','connection_string',edConn.Text);
 except
  on e:Exception do
   MessageDlg('Error occured while trying to connect to database: '+e.Message,mtError,[mbOk],0);

 end;
 {if dm_base.  DataModule2.ADOConnection1.Connected then
  begin
    //
    btConnect.Enabled:=False;
    edConn.Enabled:=false;
    lbStatus.Caption:='Connected';
  end;}
 btUpdateControlsClick(nil);


end;

procedure TfmDatabase.btDisconnectClick(Sender: TObject);
begin
  dm_base.  DataModuleBase.ADOConnection.Connected:=false;
  btUpdateControlsClick(nil);
end;

procedure TfmDatabase.btExitClick(Sender: TObject);
begin
 close;
end;

procedure TfmDatabase.btRecreateTablesClick(Sender: TObject);
var ss,ssExec:TStringlist; i:Integer;smsg:String; encoounteredProblem:Boolean;
begin
 //
 encoounteredProblem:=false;
 if MessageDlg('Attantion! All information will be lost. Continue?',mtConfirmation,[mbYes,mbNo],0)<>mrYes then
  begin
    exit;
  end;

 ssExec:=TStringlist.Create;
 ss:=TStringlist.Create;
 ss.LoadFromFile(Extractfilepath(application.ExeName)+'recreate.sql');

 for I := 0 to ss.Count-1 do
   begin
    if lowercase(trim(ss[i]))<>'go' then
     begin
       ssExec.Add(ss[i]);
     end
    else
     begin
       //
       qCreate.SQL.Text:=ssExec.Text;
       ssExec.Clear;
       try
       qCreate.ExecSQL;
       except

       on e:Exception do
        begin
        encoounteredProblem:=true;
        smsg:=copy(  qCreate.SQL.Text,1,200);
        MessageDlg('not executed: Line: '+Inttostr(i)+': '+e.Message+#13#10'executing:'#13#10+sMsg,mterror,[mbok],0);
        break;
        end;
       end;
     end;


   end;


  if encoounteredProblem then
   begin
     MessageDlg('Attention! Tables have not been created',mtConfirmation,[mbok],0);
   end
  else
   begin
     MessageDlg('Tables have been created',mtConfirmation,[mbok],0);
   end;
end;

procedure TfmDatabase.btUpdateControlsClick(Sender: TObject);
begin
 //

 if dm_base.  DataModuleBase.ADOConnection.Connected then
  begin
    //
    btConnect.Enabled:=False;
    edConn.Enabled:=false;
    btDisconnect.enabled:=true;
    btBuild.Enabled:=false;

    lbStatus.Caption:='Connected';//'Connected';
  end
 else
  begin
    btConnect.Enabled:=true;
    edConn.Enabled:=true;
    btBuild.Enabled:=true;
    lbStatus.Caption:='Disconnected';//'Disconnected';
  end;

 {if editAllowed=false then
  begin
    //
    btDisconnect.enabled:=false;
    btBuild  .enabled:=false;
  end;}

   btDisconnect.enabled:=true;
   btConnect.Enabled:=true;
   edConn.Enabled:=true;
   btBuild.Enabled:=true;


end;

procedure TfmDatabase.FormCreate(Sender: TObject);
var i:Integer;   s:String;
begin
 edConn.Items.Clear;
 edConn.Text:=ini.readString('database','connection_string','');
 for I := 0 to 255 do
   begin
     s:=trim(ini.ReadString('connection_versions',inttostr(i),''));
     if s>'' then edConn.Items.Add(s);
   end;
end;

procedure TfmDatabase.FormShow(Sender: TObject);
begin
 {if dm_base.  DataModule2.ADOConnection1.Connected then
  begin
    btConnect.Enabled:=false;
    edConn.Enabled:=false;
    lbStatus.Caption:='Connected';
  end;}
  btUpdateControlsClick(nil);
end;

procedure TfmDatabase.btBuildClick(Sender: TObject);
var  scon :String;
begin
 sCon:=PromptDataSource(handle,'');
 if trim(sCon)>'' then
  begin
   edConn.Text:=sCon;
  end;
end;

initialization
ini:=TInifile.Create( extractFilePath(application.ExeName)+'survey_info.ini' );
finalization
ini.Free;
end.
