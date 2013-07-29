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
int UpdateTime;
int UpdateMode;

extern "C" __declspec(dllexport) PPluginInfo __stdcall AQQPluginInfo(DWORD AQQVersion)
{
  PluginInfo.cbSize = sizeof(TPluginInfo);
  PluginInfo.ShortName = (wchar_t*)L"FixUpdater";
  PluginInfo.Version = PLUGIN_MAKE_VERSION(1,0,4,2);
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
  UnicodeString Enable = (WPARAM)wParam;
  UnicodeString URL = (wchar_t*)lParam;
  URL = URL.Trim();
  int Count;

  if(URL!="")
  {
	hMainForm->aReadSettings->Execute();
	//Szukanie URL
	for(Count=0;Count<hMainForm->UrlListPreview->Items->Count;Count++)
	{
	  //URL juz dodany
	  if(hMainForm->UrlListPreview->Items->Item[Count]->SubItems->Strings[0]==URL)
	   return 2;
	}
    //Dodawanie URL
	Count = hMainForm->UrlListPreview->Items->Count;
	hMainForm->UrlListPreview->Items->Add();
	hMainForm->UrlListPreview->Items->Item[Count]->Checked=StrToBool(Enable);
	hMainForm->UrlListPreview->Items->Item[Count]->SubItems->Add(URL);
	hMainForm->aSaveSettings->Execute();
	return 1;
  }
  else
   return 2;
}
//---------------------------------------------------------------------------

//Usuwanie kana³ow przez zewnetrzne wtyczki
int __stdcall DeleteLink(WPARAM wParam, LPARAM lParam)
{
  UnicodeString URL = (wchar_t*)lParam;
  URL = URL.Trim();

  if(URL!="")
  {
	hMainForm->aReadSettings->Execute();

	for(int Count=0;Count<hMainForm->UrlListPreview->Items->Count;Count++)
	{
	  if(hMainForm->UrlListPreview->Items->Item[Count]->SubItems->Strings[0]==URL)
	  {
		hMainForm->UrlListPreview->Items->Delete(Count);
		hMainForm->aSaveSettings->Execute();
		return 1;
	  }
	}
	return 2;
  }
  else
   return 2;
}
//---------------------------------------------------------------------------

int __stdcall OnModulesLoaded(WPARAM, LPARAM)
{
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

void SetUpdateLink(bool Enabled, UnicodeString URL)
{
  PluginLink.CallService(AQQ_SYSTEM_SETUPDATELINK,!Enabled,(LPARAM)URL.w_str());
}
//---------------------------------------------------------------------------

extern "C" int __declspec(dllexport) __stdcall Load(PPluginLink Link)
{
  PluginLink = *Link;

  if(hMainForm==NULL)
  {
	Application->Handle = (HWND)MainForm;
	hMainForm = new TMainForm(Application);
  }

  //Hook SDK wtyczki
  PluginLink.HookEvent(FIXUPDATER_SYSTEM_ADDLINK,AddLink);
  PluginLink.HookEvent(FIXUPDATER_SYSTEM_DELETELINK,DeleteLink);

  UnicodeString Path = GetPluginUserDir();

  //Konwersja ustawieñ 1.0.3.0 -> 1.0.4.0
  if(FileExists(Path + "\\\\FixUpdater\\\\Settings.ini"))
  {
	TIniFile *Ini = new TIniFile(Path + "\\\\FixUpdater\\\\Settings.ini");
	if(Ini->SectionExists("UrlCount"))
	{
	  int UrlCount = Ini->ReadInteger("UrlCount", "Count", 3);
	  for(int Count=1;Count<=UrlCount;Count++)
	  {
		if(Ini->ReadString("Update" + IntToStr(Count), "Url", "")=="http://aqqnews.komunikatory.pl/Pliki/aqq_update.xml")
		 Ini->WriteString("Update" + IntToStr(Count), "Url", "http://files.aqqnews.pl/fixupdater.php");

		Ini->WriteString("Links", "Url" +  IntToStr(Count), Ini->ReadString("Update" + IntToStr(Count), "Url", ""));
		Ini->WriteBool("Links", "Enable" + IntToStr(Count), Ini->ReadBool("Update" + IntToStr(Count), "Enable", true));
		Ini->EraseSection("Update" + IntToStr(Count));
	  }
	  Ini->EraseSection("UrlCount");
	}
	delete Ini;
  }

  //Ustawienia domyœlne
  if(!FileExists(Path + "\\\\FixUpdater\\\\Settings.ini"))
  {
	if(!DirectoryExists(Path + "\\\\FixUpdater"))
	 CreateDir(Path + "\\\\FixUpdater");

	TIniFile *Ini = new TIniFile(Path + "\\\\FixUpdater\\\\Settings.ini");
	Ini->WriteString("Links", "Url1", "http://beherit.pl/aqq_update/stable.xml");
	Ini->WriteBool("Links", "Enable1", true);
	Ini->WriteString("Links", "Url2", "http://beherit.pl/aqq_update/beta.xml");
	Ini->WriteBool("Links", "Enable2", false);
	Ini->WriteString("Links", "Url3", "http://files.aqqnews.pl/fixupdater.php");
	Ini->WriteBool("Links", "Enable3", true);
	delete Ini;
  }

  //Kana³y aktualizacji
  TIniFile *Ini = new TIniFile(Path + "\\\\FixUpdater\\\\Settings.ini");
  TStringList *Links = new TStringList;
  Ini->ReadSection("Links",Links);
  for(int Count=0;Count<Links->Count/2;Count++)
  {
	if(Ini->ReadBool("Links", "Enable" + IntToStr(Count+1), true)==true)
	{
	  UnicodeString Url = Ini->ReadString("Links", "Url" + IntToStr(Count+1), "");
	  if(Url!="") SetUpdateLink(true,Url);
	}
  }
  delete Links;
  //Sposób aktualizacji
  hMainForm->UpdateMode = Ini->ReadInteger("Settings", "UpdateMode", 0);
  //Czêstotliwoœci aktualizacji
  hMainForm->UpdateTime = Ini->ReadInteger("Settings", "UpdateTime", 0);
  delete Ini;
  //W³¹czenie Timer'a
  PluginLink.HookEvent(AQQ_SYSTEM_MODULESLOADED, OnModulesLoaded);
  if(PluginLink.CallService(AQQ_SYSTEM_MODULESLOADED,0,0)==true)
   hMainForm->CheckUpdatesOnStartTimer->Enabled=true;

  return 0;
}
//---------------------------------------------------------------------------

extern "C" int __declspec(dllexport)__stdcall Settings()
{
  hMainForm->Show();

  return 0;
}
//---------------------------------------------------------------------------

extern "C" int __declspec(dllexport) __stdcall Unload()
{
  //Wylaczanie timetow
  if(hMainForm!=NULL)
  {
	hMainForm->CheckUpdatesTimer->Enabled = false;
	hMainForm->CheckUpdatesOnStartTimer->Enabled = false;
  }
  //Unhook eventow
  PluginLink.UnhookEvent(AddLink);
  PluginLink.UnhookEvent(DeleteLink);
  PluginLink.UnhookEvent(OnModulesLoaded);
  //Wywalanie linkow z aktualizatora
  TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\FixUpdater\\\\Settings.ini");
  TStringList *Links = new TStringList;
  Ini->ReadSection("Links",Links);
  for(int Count=0;Count<Links->Count/2;Count++)
  {
	if(Ini->ReadBool("Links", "Enable" + IntToStr(Count+1), true)==true)
	{
	  UnicodeString Url = Ini->ReadString("Links", "Url" + IntToStr(Count+1), "");
	  if(Url!="") SetUpdateLink(false,Url);
	}
  }
  delete Links;
  delete Ini;

  return 0;
}
//---------------------------------------------------------------------------

void CheckUpdates(int Mode)
{
  if(Mode==0)//Normalne aktualizacje
   PluginLink.CallService(AQQ_FUNCTION_SILENTUPDATECHECK,0,0);
  else if(Mode==1)//Wymuszanie
   PluginLink.CallService(AQQ_FUNCTION_SILENTUPDATECHECK,0,1);
  else if(Mode==2)//Wersje Beta AQQ
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
