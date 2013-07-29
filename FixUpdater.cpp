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
int UpdateMode;
//Zewnetrzny hook
UnicodeString PEnable;
UnicodeString PURL;
int PCount;
bool PDoNotAdd;

extern "C" __declspec(dllexport) PPluginInfo __stdcall AQQPluginInfo(DWORD AQQVersion)
{
  //Sprawdzanie wersji AQQ
  if (CompareVersion(AQQVersion,PLUGIN_MAKE_VERSION(2,1,0,47))<0)
  {
	MessageBox(Application->Handle,
	  "Wymagana wesja AQQ przez wtyczk� to minimum 2.1.0.47!\n"
	  "Wtyczka FixUpdater nie b�dzie dzia�a� poprawnie!",
	  "Nieprawid�owa wersja AQQ",
	  MB_OK | MB_ICONEXCLAMATION);
  }
  PluginInfo.cbSize = sizeof(TPluginInfo);
  PluginInfo.ShortName = (wchar_t*)L"FixUpdater";
  PluginInfo.Version = PLUGIN_MAKE_VERSION(1,0,1,8);
  PluginInfo.Description = (wchar_t*)L"Dodawanie w�asnych serwer�w aktualizacji dodatk�w";
  PluginInfo.Author = (wchar_t*)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.AuthorMail = (wchar_t*)L"sirbeherit@gmail.com";
  PluginInfo.Copyright = (wchar_t*)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.Homepage = (wchar_t*)L"http://beherit.pl/";

  return &PluginInfo;
}
//---------------------------------------------------------------------------

//Dodawanie kana�ow przez zewnetrzne wtyczki
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

//Usuwanie kana�ow przez zewnetrzne wtyczki
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
  hMainForm->eUpdateMode = UpdateMode;
  hMainForm->CheckUpdatesOnStartTimer->Enabled=true;
  PluginLink.UnhookEvent(OnModulesLoaded);

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

  //W��czenie aktualizacji dodatkow w AQQ
  SaveSetup.Section = L"Settings";
  SaveSetup.Ident = L"UpdateAddons";
  SaveSetup.Value = L"1";
  PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
  PluginLink.CallService(AQQ_FUNCTION_REFRESHSETUP,0,0);

  //Hook SDK wtyczki
  PluginLink.HookEvent(FIXUPDATER_SYSTEM_ADDLINK,AddLink);
  PluginLink.HookEvent(FIXUPDATER_SYSTEM_DELETELINK,DeleteLink);

  //Ustawienia domy�lne
  if(!FileExists(Dir + "\\\\FixUpdater\\\\Url.ini"))
  {
	if(!DirectoryExists(Dir + "\\\\FixUpdater"))
	 CreateDir(Dir + "\\\\FixUpdater");

	TIniFile *Ini = new TIniFile(Dir + "\\\\FixUpdater\\\\Url.ini");
	Ini->WriteInteger("UrlCount", "Count", 3);
	Ini->WriteString("Update1", "Url", "http://beherit.pl/aqq_update/stable.xml");
	Ini->WriteBool("Update1", "Enable", true);
	Ini->WriteString("Update2", "Url", "http://beherit.pl/aqq_update/beta.xml");
	Ini->WriteBool("Update2", "Enable", false);
	Ini->WriteString("Update3", "Url", "http://aqqnews.komunikatory.pl/Pliki/aqq_update.xml");
	Ini->WriteBool("Update3", "Enable", true);
	delete Ini;
  }

  TIniFile *Ini = new TIniFile(Dir + "\\\\FixUpdater\\\\Url.ini");

  //Kana�y aktualizacji
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

  //Cz�stotliwo�ci aktualizacji
  UpdateMode = Ini->ReadInteger("Settings", "UpdateMode", 0);
  if(PluginLink.CallService(AQQ_SYSTEM_MODULESLOADED,0,0)==false)
  {
	PluginLink.HookEvent(AQQ_SYSTEM_MODULESLOADED, OnModulesLoaded);
  }
  else
  {
	if(hMainForm==NULL)
	{
	  Application->Handle = (HWND)MainForm;
	  hMainForm = new TMainForm(Application);
	}
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
  TIniFile *Ini = new TIniFile(Dir + "\\\\FixUpdater\\\\Url.ini");
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

  return 0;
}
//---------------------------------------------------------------------------

void SetUpdateLink(bool Enabled, UnicodeString URL)
{
  PluginLink.CallService(AQQ_SYSTEM_SETUPDATELINK,Enabled,(LPARAM)URL.w_str());
}
//---------------------------------------------------------------------------

void CheckUpdates()
{
  PluginLink.CallService(AQQ_FUNCTION_SILENTUPDATECHECK,0,1);
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
