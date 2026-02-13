!include "MUI2.nsh"

Name "My Qt App"
OutFile "MyAppInstaller.exe"
InstallDir "$PROGRAMFILES64\MyApp"

RequestExecutionLevel admin

Section "Install"

  SetOutPath "$INSTDIR"

  ; ---- Install VC++ Redistributable ----
  File "deploy\VC_redist.x64.exe"
  ExecWait '"$INSTDIR\VC_redist.x64.exe" /install /quiet /norestart'

  ; ---- Copy application files ----
  File /r "deploy\app\*.*"

  CreateShortcut "$DESKTOP\MyApp.lnk" "$INSTDIR\MyApp.exe"

SectionEnd
