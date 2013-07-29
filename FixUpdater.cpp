//---------------------------------------------------------------------------
#include <vcl.h>
#include <windows.h>
#pragma hdrstop
#pragma argsused
#include "Aqq.h"
#include "MainFrm.h"
#include <inifiles.hpp>
#define FIXUPDATER_SYSTEM_ADDLINK L"FixUpdater/System/AddLink"
#define FIXUPDATER_SYSTEM_DELETELINK L"FixUpdater/System/DeleteLink"
//---------------------------------------------------------------------------

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
  return 1;
}
//---------------------------------------------------------------------------

//tworzenie uchwytu do formy
TMainForm *hMainForm;

//utworzenie obiektow do struktur
TPluginLink PluginLink;
TPluginInfo PluginInfo;
TPluginHook PluginHook;
TSaveSetup SaveSetup;

//Zmienne
UnicodeString Dir;
int UrlCount;
UnicodeString Url;
bool Enabled;
int Count;
int UpdateTime;
int UpdateMode;
//Zewnetrzny hook
UnicodeString PEnable;
UnicodeString PURL;
int PCount;
bool PDoNotAdd;

extern "C" __declspec(dllexport) PPluginInfo __stdcall AQQPluginInfo(DWORD AQQVersion)
{
  //Sprawdzanie wersji AQQ
  if (CompareVersion(AQQVersion,PLUGIN_MAKE_VERSION(2,2,0,45))<0)
  {
	MessageBox(Application->Handle,
	  "Wymagana wesja AQQ przez wtyczkê to minimum 2.2.0.45!\n"
	  "Wtyczka FixUpdater nie bêdzie dzia³aæ poprawnie!",
	  "Nieprawid³owa wersja AQQ",
	  MB_OK | MB_ICONEXCLAMATION);
  }
  PluginInfo.cbSize = sizeof(TPluginInfo);
  PluginInfo.ShortName = (wchar_t*)L"FixUpdater";
  PluginInfo.Version = PLUGIN_MAKE_VERSION(1,0,3,0);
  PluginInfo.Description = (wchar_t*)L"Dodawanie w³asnych serwerów aktualizacji dodatków";
  PluginInfo.Author = (wchar_t*)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.AuthorMail = (wchar_t*)L"email@beherit.pl";
  PluginInfo.Copyright = (wchar_t*)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.Homepage = (wchar_t*)L"http://beherit.pl/";

  return &PluginInfo;
}
//---------------------------------------------------------------------------

//Dodawanie kana³ow przez zewnetrzne wtyczki
int __stdcall AddLink(WPARAM wParam, LPARAM lParam)
{
  PEnable = (WPARAM)wParam;
  PURL = (wchar_t*)lParam;
  PURL = PURL.Trim();

  if(PURL!="")
  {
	if(hMainForm==NULL)
	{
	  Application->Handle = (HWND)MainForm;
	  hMainForm = new TMainForm(Application);
	}
	hMainForm->aReadSettings->Execute();

	PDoNotAdd = false;
	for(PCount=0;PCount<hMainForm->UrlListPreview->Items->Count;PCount++)
	{
	  if(hMainForm->UrlListPreview->Items->Item[PCount]->SubItems->Strings[0]==PURL)
	  {
		PDoNotAdd = true;
		PCount = hMainForm->UrlListPreview->Items->Count;
	  }
	}

	if(PDoNotAdd==false)
	{
	  PCount = hMainForm->UrlListPreview->Items->Count;
	  hMainForm->UrlListPreview->Items->Add();
	  hMainForm->UrlListPreview->Items->Item[PCount]->Checked=StrToBool(PEnable);
	  hMainForm->UrlListPreview->Items->Item[PCount]->SubItems->Add(PURL);
	  hMainForm->aSaveSettings->Execute();
	}
  }

  return 1;
}
//---------------------------------------------------------------------------

//Usuwanie kana³ow przez zewnetrzne wtyczki
int __stdcall DeleteLink(WPARAM wParam, LPARAM lParam)
{
  PURL = (wchar_t*)lParam;
  PURL = PURL.Trim();

  if(PURL!="")
  {
	if(hMainForm==NULL)
	{
	  Application->Handle = (HWND)MainForm;
	  hMainForm = new TMainForm(Application);
	}
	hMainForm->aReadSettings->Execute();

	for(PCount=0;PCount<hMainForm->UrlListPreview->Items->Count;PCount++)
	{
	  if(hMainForm->UrlListPreview->Items->Item[PCount]->SubItems->Strings[0]==PURL)
	  {
		hMainForm->UrlListPreview->Items->Delete(PCount);
		hMainForm->aSaveSettings->Execute();
		PCount = hMainForm->UrlListPreview->Items->Count;
	  }
	}
  }

  return 1;
}
//---------------------------------------------------------------------------

int __stdcall OnModulesLoaded(WPARAM, LPARAM)
{
  if(hMainForm==NULL)
  {
	Application->Handle = (HWND)MainForm;
	hMainForm = new TMainForm(Application);
  }
  hMainForm->eUpdateTime = UpdateTime;
  hMainForm->eUpdateMode = UpdateMode;
  hMainForm->CheckUpdatesOnStartTimer->Enabled=true;

  return 0;
}
//---------------------------------------------------------------------------

//Pobieranie sciezki katalogu prywatnego uzytkownika
UnicodeString GetPluginUserDir()
{
  UnicodeString Dir = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(HInstance),0));
  Dir = StringReplace(Dir, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
  return Dir;
}
//---------------------------------------------------------------------------

extern "C" int __declspec(dllexport) __stdcall Load(PPluginLink Link)
{
  PluginLink = *Link;

  Dir = GetPluginUserDir();

  //Hook SDK wtyczki
  PluginLink.HookEvent(FIXUPDATER_SYSTEM_ADDLINK,AddLink);
  PluginLink.HookEvent(FIXUPDATER_SYSTEM_DELETELINK,DeleteLink);

  //Konwersja ustawieñ 1.0.2.0 -> 1.0.3.0
  if(FileExists(Dir + "\\\\FixUpdater\\\\Url.ini"))
  {
	MoveFile((Dir + "\\\\FixUpdater\\\\Url.ini").t_str(),(Dir + "\\\\FixUpdater\\\\Settings.ini").t_str());
	TIniFile *Ini = new TIniFile(Dir + "\\\\FixUpdater\\\\Settings.ini");
	UpdateTime = Ini->ReadInteger("Settings", "UpdateMode", 0);
	Ini->WriteInteger("Settings", "UpdateTime",UpdateTime);
	Ini->WriteInteger("Settings", "UpdateMode",0);
	delete Ini;
  }

  //Ustawienia domyœlne
  if(!FileExists(Dir + "\\\\FixUpdater\\\\Settings.ini"))
  {
	if(!DirectoryExists(Dir + "\\\\FixUpdater"))
	 CreateDir(Dir + "\\\\FixUpdater");

	TIniFile *Ini = new TIniFile(Dir + "\\\\FixUpdater\\\\Settings.ini");
	Ini->WriteInteger("UrlCount", "Count", 3);
	Ini->WriteString("Update1", "Url", "http://beherit.pl/aqq_update/stable.xml");
	Ini->WriteBool("Update1", "Enable", true);
	Ini->WriteString("Update2", "Url", "http://beherit.pl/aqq_update/beta.xml");
	Ini->WriteBool("Update2", "Enable", false);
	Ini->WriteString("Update3", "Url", "http://aqqnews.komunikatory.pl/Pliki/aqq_update.xml");
	Ini->WriteBool("Update3", "Enable", true);
	delete Ini;
  }

  TIniFile *Ini = new TIniFile(Dir + "\\\\FixUpdater\\\\Settings.ini");
  //Kana³y aktualizacji
  UrlCount = Ini->ReadInteger("UrlCount", "Count", 3);
  for(Count=1;Count<=UrlCount;Count++)
  {
	Enabled = Ini->ReadBool("Update" + IntToStr(Count), "Enable", true);
	if(Enabled==true)
	{
	  Url = Ini->ReadString("Update" + IntToStr(Count), "Url", "");
	  if(Url!="") PluginLink.CallService(AQQ_SYSTEM_SETUPDATELINK,0,(LPARAM)Url.w_str());
	}
  }
  //Czêstotliwoœci aktualizacji
  UpdateTime = Ini->ReadInteger("Settings", "UpdateTime", 0);
  //Sposób aktualizacji
  UpdateMode = Ini->ReadInteger("Settings", "UpdateMode", 0);
  //W³¹czenie Timer'a
  PluginLink.HookEvent(AQQ_SYSTEM_MODULESLOADED, OnModulesLoaded);
  if(PluginLink.CallService(AQQ_SYSTEM_MODULESLOADED,0,0)==true)
  {
	if(hMainForm==NULL)
	{
	  Application->Handle = (HWND)MainForm;
	  hMainForm = new TMainForm(Application);
	}
	hMainForm->eUpdateTime = UpdateTime;
	hMainForm->eUpdateMode = UpdateMode;
	hMainForm->CheckUpdatesOnStartTimer->Enabled=true;
  }
  delete Ini;

  return 0;
}
//---------------------------------------------------------------------------

extern "C" int __declspec(dllexport)__stdcall Settings()
{
  if(hMainForm==NULL)
  {
	Application->Handle = (HWND)MainForm;
	hMainForm = new TMainForm(Application);
  }
  hMainForm->Show();

  return 0;
}
//---------------------------------------------------------------------------

extern "C" int __declspec(dllexport) __stdcall Unload()
{
  Dir = GetPluginUserDir();
  TIniFile *Ini = new TIniFile(Dir + "\\\\FixUpdater\\\\Settings.ini");
  UrlCount = Ini->ReadInteger("UrlCount", "Count", 3);
  for(Count=1;Count<=UrlCount;Count++)
  {
	Enabled = Ini->ReadBool("Update" + IntToStr(Count), "Enable", true);
	if(Enabled==true)
	{
	  Url = Ini->ReadString("Update" + IntToStr(Count), "Url", "");
	  if(Url!="") PluginLink.CallService(AQQ_SYSTEM_SETUPDATELINK,1,(LPARAM)Url.w_str());
	}
  }
  delete Ini;
  PluginLink.UnhookEvent(AddLink);
  PluginLink.UnhookEvent(DeleteLink);
  PluginLink.UnhookEvent(OnModulesLoaded);

  return 0;
}
//---------------------------------------------------------------------------

void SetUpdateLink(bool Enabled, UnicodeString URL)
{
  PluginLink.CallService(AQQ_SYSTEM_SETUPDATELINK,Enabled,(LPARAM)URL.w_str());
}
//---------------------------------------------------------------------------

void CheckUpdates(int Mode)
{
  if(Mode==0)//Normalne aktualizacje
   PluginLink.CallService(AQQ_FUNCTION_SILENTUPDATECHECK,0,0);
  if(Mode==1)//Wymuszanie
   PluginLink.CallService(AQQ_FUNCTION_SILENTUPDATECHECK,0,1);
  if(Mode==2)//Wersje Beta AQQ
   PluginLink.CallService(AQQ_FUNCTION_SILENTUPDATECHECK,0,2);
}
//---------------------------------------------------------------------------

UnicodeString GetLastUpdate()
{
  UnicodeString LastUpdate = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0));
  int Pos = AnsiPos("LastUpdateDate=",LastUpdate);
  LastUpdate.Delete(1,Pos+14);
  Pos = AnsiPos("\n",LastUpdate);
  LastUpdate.Delete(Pos,LastUpdate.Length());
  return LastUpdate;
}
//---------------------------------------------------------------------------
