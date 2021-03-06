; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "RPUDesigner"
#define MySubAppName "rpusim"
#define MyAppWindowName "RPU Designer"
#define MyAppVersion "0.3.0"
#define MyAppPublisher "清华微纳实验室"
#define MyAppURL "http://www.tsinghua-wx.org/"
#define MyAppExeName "rpudesigner.exe"
#define MyExePath "..\"
#define MyOutputDir ".\release"
#define MySetupIconFile "..\image\install.ico"
#define MyDateTimeString GetDateTimeString('yyyymmddhhnnss', '', ''); 

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
;AppId={{5FCED8F9-1597-45F3-800D-D7EA8B8DE5FE}AppId={#MyAppName}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
OutputDir={#MyOutputDir}
OutputBaseFilename={#MyAppName}_win_x86_v{#MyAppVersion}_{#MyDateTimeString}
SetupIconFile={#MySetupIconFile}
Compression=lzma
SolidCompression=yes
PrivilegesRequired=admin;Password=Tsinghua123
LicenseFile={#MyExePath}\releasescript\release\RPUDesigner\bin\LICENSE

[LangOptions]
TitleFontSize=9
DialogFontSize=9
CopyrightFontSize=9

[Languages]
Name: "chinesesimplified"; MessagesFile: "compiler:Languages\ChineseSimplified.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "french"; MessagesFile: "compiler:Languages\French.isl"
Name: "german"; MessagesFile: "compiler:Languages\German.isl"
Name: "japanese"; MessagesFile: "compiler:Languages\Japanese.isl"

[CustomMessages]
chinesesimplified.MyCheckInstallTip = 您已经安装过本软件，即将卸载，卸载完毕后请继续安装！

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}";Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}";Flags:checkedonce;



[Files]
Source: "{#MyExePath}\releasescript\release\RPUDesigner\bin\{#MyAppExeName}"; DestDir: "{app}/bin"; Flags: ignoreversion
Source: "{#MyExePath}\releasescript\release\RPUDesigner\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#MyExePath}\version.txt"; DestDir: "{app}"; Flags: ignoreversion
;Source: "{#MyExePath}\bin\*"; DestDir: "{app}/bin"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#MyExePath}\etc\*"; DestDir: "{app}/etc"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#MyExePath}\doc\*"; DestDir: "{app}/doc"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#MyExePath}\image\*"; DestDir: "{app}/image"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#MyExePath}\script\*"; DestDir: "{app}/script"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#MyExePath}\plugins\*"; DestDir: "{app}/plugins"; Flags: ignoreversion recursesubdirs createallsubdirs;Source: "{#MyExePath}\lib\*"; DestDir: "{app}/lib"; Flags: ignoreversion recursesubdirs createallsubdirs 
;Source: "{#MyExePath}\log"; DestDir: "{app}/log"; Flags: ignoreversion 
;Source: "{#MyExePath}\out\*"; DestDir: "{app}/out"; Flags: ignoreversion recursesubdirs createallsubdirs
;Source: "{#MyExePath}\readme\*"; DestDir: "{app}/readme"; Flags: ignoreversion recursesubdirs createallsubdirs
;Source: "{#MyExePath}\tmp"; DestDir: "{app}/tmp"; Flags: ignoreversion 
; NOTE: Don't use "Flags: ignoreversion" on any shared system files


[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\bin\{#MyAppExeName}"
;Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{group}\{#MySubAppName}"; Filename: "{app}\bin\{#MySubAppName}.exe";
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\bin\{#MyAppExeName}"; Tasks: desktopicon;IconFilename: "{app}\image\chip.ico"
;Name: "{commondesktop}\{#MySubAppName}"; Filename: "{app}\bin\{#MySubAppName}.exe"; Tasks: desktopicon;IconFilename: "{app}\image\cmd.ico"


[Registry]
Root: HKLM; Subkey: Software\{#MyAppName}; Flags: uninsdeletekey dontcreatekey; ValueType: none; 
Root: HKLM ;SubKey: Software\{#MyAppName}; ValueType:string;ValueName:InstallFlag;ValueData:ForAllUser ;

Root: HKCR ;SubKey: .rpusln; ValueType:string;ValueName:;ValueData:{#MyAppName}.rpusln;Flags: uninsdeletekey;
Root: HKCR ;SubKey: {#MyAppName}.rpusln; Flags: uninsdeletekey dontcreatekey; 
Root: HKCR ;SubKey: {#MyAppName}.rpusln\DefaultIcon; ValueType:string;ValueName:DefaultIcon;ValueData:{app}\image\chip.ico; 
Root: HKCR ;SubKey: {#MyAppName}.rpusln\shell\open\command; ValueType:string;ValueName:;ValueData:"{app}\bin\{#MyAppName}.exe ""%1""";

[Run]
Filename: "{app}\bin\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[Code]
function InitializeSetup(): boolean;
var 
  appWnd: HWND; 
  isSetup: boolean;         
  ResultCode: Integer;
  isQuit: boolean;  uicmd: String;  path: String;
begin       
  isSetup := true;    
  appWnd := FindWindowByWindowName('{#MyAppWindowName}');
  if (appWnd <> 0) then
  begin         
    isQuit := MsgBox('RPU Designer正在运行，是否继续安装?', mbConfirmation, MB_YESNO) = idYes;
    isSetup:= isQuit;
		if isQuit = true then
		begin
			  PostMessage(appWnd, 18, 0, 0);
		end;
 end;
 Result:= isSetup;end;



function InitializeUninstall(): Boolean; 
var   
  appWnd: HWND;      
  ResultCode: boolean;
  isQuit: boolean;
  isUninstall: boolean;
  uicmd: String;
  path: String;
   
begin 
  ResultCode := false;	
  isUninstall := true;
// 检查**进程是否在运行，是则关闭进程
  appWnd := FindWindowByWindowName('{#MyAppWindowName}');
  if (appWnd <> 0) then
  begin
		isQuit := MsgBox('RPU Designer正在运行，是否继续卸载?', mbConfirmation, MB_YESNO) = idYes;
    isUninstall:= isQuit;
		if isQuit = true then
		begin
			  PostMessage(appWnd, 18, 0, 0);       // quit
        ResultCode := MsgBox(('是否保留配置信息及产品许可证？'), mbInformation, MB_YESNO) = idyes;  
        if ResultCode = false then
        begin
        RegDeleteKeyIncludingSubkeys(HKEY_CURRENT_USER, 'Software\Tsinghua Software Inc.\rpudesigner');
        end;
		end;
	 end else begin
        ResultCode := MsgBox(('是否保留配置信息及产品许可证？'), mbInformation, MB_YESNO) = idyes;  
        if ResultCode = false then
        begin
        RegDeleteKeyIncludingSubkeys(HKEY_CURRENT_USER, 'Software\Tsinghua Software Inc.\rpudesigner');
        end;
  end;  
  Result:= isUninstall;
end;

procedure CurStepChanged(CurStep: TSetupStep);
var 
  appWnd: HWND;
  //isQuit: boolean;
begin
  if CurStep = ssInstall then
  begin
    // 检查××进程是否在运行，是则关闭进程
    appWnd := FindWindowByWindowName('{#MyAppWindowName}');
    if (appWnd <> 0) then
    begin
      //isQuit := MsgBox('RPU Designer正在运行,确定退出吗?', mbConfirmation, MB_YESNO) = idYes;
      //if isQuit = true then
      //begin
        PostMessage(appWnd, 18, 0, 0);       // quit
      //end;
    end;
  end;
end;
