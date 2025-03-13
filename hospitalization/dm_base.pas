unit dm_base;

interface

uses
  System.SysUtils, System.Classes, Data.DB, Data.Win.ADODB;

type
  TDataModuleBase = class(TDataModule)
    ADOConnection: TADOConnection;
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  DataModuleBase: TDataModuleBase;

implementation

{%CLASSGROUP 'Vcl.Controls.TControl'}

{$R *.dfm}

end.
