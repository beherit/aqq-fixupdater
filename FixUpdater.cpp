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
bool ChangeAddonBrowserChk;

//Wymagana wersja AQQ
bool ChkAQQVersion = true;

//Pobieranie sciezki do skorki kompozycji
UnicodeString GetThemeSkinDir()
{
  UnicodeString Dir = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETTHEMEDIR,0,0));
  Dir = StringReplace(Dir, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
  Dir = Dir + "\\\\Skin";
  return Dir;
}
//---------------------------------------------------------------------------

//Sprawdzanie czy wlaczona jest obsluga stylow obramowania okien
bool ChkSkinEnabled()
{
  TStrings* IniList = new TStringList();
  IniList->SetText((wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0)));
  TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
  Settings->SetStrings(IniList);
  delete IniList;
  UnicodeString AlphaSkinsEnabled = Settings->ReadString("Settings","UseSkin","1");
  delete Settings;
  return StrToBool(AlphaSkinsEnabled);
}
//---------------------------------------------------------------------------

//Zmiana skorki wtyczki
void ChangePluginSkin()
{
  if(hMainForm)
  {
	UnicodeString ThemeSkinDir = GetThemeSkinDir();
	if(FileExists(ThemeSkinDir + "\\\\Skin.asz"))
	{
	  ThemeSkinDir = StringReplace(ThemeSkinDir, "\\\\", "\\", TReplaceFlags() << rfReplaceAll);
	  hMainForm->sSkinManager->SkinDirectory = ThemeSkinDir;
	  hMainForm->sSkinManager->SkinName = "Skin.asz";
	  hMainForm->sSkinProvider->DrawNonClientArea = ChkSkinEnabled();
	  hMainForm->sSkinManager->Active = true;
	}
	else
	 hMainForm->sSkinManager->Active = false;
  }
}
//---------------------------------------------------------------------------

int __stdcall OnThemeChanged (WPARAM wParam, LPARAM lParam)
{
  ChangePluginSkin();

  return 0;
}
//---------------------------------------------------------------------------

//Dodawanie kana³ow przez zewnetrzne wtyczki
int __stdcall AddLink(WPARAM wParam, LPARAM lParam)
{
  UnicodeString Enable = (WPARAM)wParam;
  UnicodeString URL = (wchar_t*)lParam;
  URL = URL.Trim();
  int Count;

  if(!URL.IsEmpty())
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

  if(!URL.IsEmpty())
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

int __stdcall OnAddonBrowser(WPARAM wParam, LPARAM lParam)
{
   if(ChangeAddonBrowserChk) return (LPARAM)L"http://addons.aqqnews.pl/";
   else return 0;
}
//---------------------------------------------------------------------------

int __stdcall OnModulesLoaded(WPARAM, LPARAM)
{
  hMainForm->CheckUpdatesOnStartTimer->Enabled = true;

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
  TStrings* IniList = new TStringList();
  IniList->SetText((wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0)));
  TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
  Settings->SetStrings(IniList);
  delete IniList;
  UnicodeString LastUpdate = Settings->ReadString("User","LastUpdateDate","0");
  delete Settings;
  return LastUpdate;
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

void ChangeAddonBrowser(bool Enabled)
{
  ChangeAddonBrowserChk = Enabled;
}
//---------------------------------------------------------------------------

int __stdcall OnAddonInstalled(WPARAM wParam, LPARAM lParam)
{
  UnicodeString AddonFile = (wchar_t*)lParam ;
  if(ExtractFileName(AddonFile)!="FixUpdater.dll")
  {
	PPluginTriple PluginTriple = (PPluginTriple)wParam;
	if(PluginTriple->Param1==tatPlugin)
	{
	   hMainForm->CheckUpdatesTimer->Enabled = false;
	   hMainForm->CheckUpdatesOnStartTimer->Enabled = false;
	   hMainForm->CheckUpdatesOnStartTimer->Interval = 2000;
	   hMainForm->CheckUpdatesOnStartTimer->Enabled = true;
	}
  }
}
//---------------------------------------------------------------------------

extern "C" int __declspec(dllexport) __stdcall Load(PPluginLink Link)
{
  PluginLink = *Link;
  //Tworzenie uchwytu do okna glownego
  if(!hMainForm)
  {
	Application->Handle = (HWND)MainForm;
	hMainForm = new TMainForm(Application);
  }
  //Pobieranie sciezki
  UnicodeString Path = GetPluginUserDir();
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
	Ini->WriteString("Links", "Url4", "http://files.aqqnews.pl/fixupdater-beta.php");
	Ini->WriteBool("Links", "Enable4", false);
	delete Ini;
  }
  //Sprawdzenie czy sa domyslne linki
  else
  {
	TIniFile *Ini = new TIniFile(Path + "\\\\FixUpdater\\\\Settings.ini");
	UnicodeString Version = Ini->ReadString("Settings","Default","");
	//Wersja domyslnych linkow jest inna
	if(Version!="1.3")
	{
	  //Ustawiane nowej wersji
	  Ini->WriteString("Settings","Default","1.3");
	  //Aktualizacja linkow
	  TStringList *Links = new TStringList;
	  TStringList *UserLinks = new TStringList;
	  TStringList *DefLinks = new TStringList;
	  Ini->ReadSection("Links",Links);
	  for(int Count=0;Count<Links->Count/2;Count++)
	  {
		UserLinks->Add(Ini->ReadString("Links","Url"+IntToStr(Count+1),""));
		UserLinks->Add(Ini->ReadString("Links","Enable"+IntToStr(Count+1),""));
	  }
	  //Usuwanie starych domyslnych linkow
	  int Count = UserLinks->IndexOf("http://beherit.pl/aqq_update.xml");
	  if(Count!=-1)
	  {
		UserLinks->Delete(Count);
		UserLinks->Delete(Count);
	  }
	  Count = UserLinks->IndexOf("http://beherit.pl/aqq_update_beta.xml");
      if(Count!=-1)
	  {
		UserLinks->Delete(Count);
		UserLinks->Delete(Count);
	  }
	  //Usuwanie domyslnych linkow
	  Count = UserLinks->IndexOf("http://beherit.pl/aqq_update/stable.xml");
	  if(Count!=-1)
	  {
		DefLinks->Add(UserLinks->Strings[Count]);
		DefLinks->Add(UserLinks->Strings[Count+1]);
		UserLinks->Delete(Count);
		UserLinks->Delete(Count);
	  }
	  Count = UserLinks->IndexOf("http://beherit.pl/aqq_update/beta.xml");
	  if(Count!=-1)
	  {
		DefLinks->Add(UserLinks->Strings[Count]);
		DefLinks->Add(UserLinks->Strings[Count+1]);
		UserLinks->Delete(Count);
		UserLinks->Delete(Count);
	  }
	  Count = UserLinks->IndexOf("http://files.aqqnews.pl/fixupdater.php");
	  if(Count!=-1)
	  {
		DefLinks->Add(UserLinks->Strings[Count]);
		DefLinks->Add(UserLinks->Strings[Count+1]);
		UserLinks->Delete(Count);
		UserLinks->Delete(Count);
	  }
	  Count = UserLinks->IndexOf("http://files.aqqnews.pl/fixupdater-beta.php");
	  if(Count!=-1)
	  {
		DefLinks->Add(UserLinks->Strings[Count]);
		DefLinks->Add(UserLinks->Strings[Count+1]);
		UserLinks->Delete(Count);
		UserLinks->Delete(Count);
	  }
	  //Dodawanie domyslnych linkow
	  Count = DefLinks->IndexOf("http://files.aqqnews.pl/fixupdater-beta.php");
	  if(Count!=-1)
	  {
		UserLinks->Insert(0,DefLinks->Strings[Count+1]);
		UserLinks->Insert(0,DefLinks->Strings[Count]);
	  }
	  else
	  {
		UserLinks->Insert(0,"1");
		UserLinks->Insert(0,"http://files.aqqnews.pl/fixupdater-beta.php");
	  }
	  Count = DefLinks->IndexOf("http://files.aqqnews.pl/fixupdater.php");
	  if(Count!=-1)
	  {
		UserLinks->Insert(0,DefLinks->Strings[Count+1]);
		UserLinks->Insert(0,DefLinks->Strings[Count]);
	  }
	  else
	  {
		UserLinks->Insert(0,"1");
		UserLinks->Insert(0,"http://files.aqqnews.pl/fixupdater.php");
	  }
	  Count = DefLinks->IndexOf("http://beherit.pl/aqq_update/beta.xml");
	  if(Count!=-1)
	  {
		UserLinks->Insert(0,DefLinks->Strings[Count+1]);
		UserLinks->Insert(0,DefLinks->Strings[Count]);
	  }
	  else
	  {
		UserLinks->Insert(0,"0");
		UserLinks->Insert(0,"http://beherit.pl/aqq_update/beta.xml");
	  }
	  Count = DefLinks->IndexOf("http://beherit.pl/aqq_update/stable.xml");
	  if(Count!=-1)
	  {
		UserLinks->Insert(0,DefLinks->Strings[Count+1]);
		UserLinks->Insert(0,DefLinks->Strings[Count]);
	  }
	  else
	  {
		UserLinks->Insert(0,"1");
		UserLinks->Insert(0,"http://beherit.pl/aqq_update/stable.xml");
	  }
	  //Zapisywanie nowej struktury
	  Ini->EraseSection("Links");
	  for(int Count=0;Count<UserLinks->Count/2;Count++)
	  {
		Ini->WriteString("Links","Url"+IntToStr(Count+1),UserLinks->Strings[Count+Count]);
		Ini->WriteString("Links","Enable"+IntToStr(Count+1),UserLinks->Strings[Count+Count+1]);
	  }
	  delete DefLinks;
	  delete UserLinks;
	  delete Links;
	}
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
  //Menedzer dodatkow
  ChangeAddonBrowserChk = Ini->ReadBool("Settings", "ChangeAddonBrowser", true);
  delete Ini;
  //Hook SDK wtyczki
  PluginLink.HookEvent(FIXUPDATER_SYSTEM_ADDLINK,AddLink);
  PluginLink.HookEvent(FIXUPDATER_SYSTEM_DELETELINK,DeleteLink);
  //Hook na pokaztwanie browsera dodatkow
  PluginLink.HookEvent(AQQ_SYSTEM_ADDONBROWSER_URL,OnAddonBrowser);
  //Hook na instalowanie dodatkow
  PluginLink.HookEvent(AQQ_SYSTEM_ADDONINSTALLED,OnAddonInstalled);
  //Hook na zmianê kompozycji
  PluginLink.HookEvent(AQQ_SYSTEM_THEMECHANGED,OnThemeChanged);
  //W³¹czenie Timer'a automatycznej aktualizacji
  PluginLink.HookEvent(AQQ_SYSTEM_MODULESLOADED, OnModulesLoaded);
  if(PluginLink.CallService(AQQ_SYSTEM_MODULESLOADED,0,0)==true)
   hMainForm->CheckUpdatesOnStartTimer->Enabled=true;

  return 0;
}
//---------------------------------------------------------------------------

extern "C" int __declspec(dllexport)__stdcall Settings()
{
  hMainForm->Show();
  hMainForm->ChangeAddonBrowserCheckBox->Enabled = ChkAQQVersion;

  return 0;
}
//---------------------------------------------------------------------------

extern "C" int __declspec(dllexport) __stdcall Unload()
{
  //Wylaczanie timetow
  if(hMainForm)
  {
	hMainForm->CheckUpdatesTimer->Enabled = false;
	hMainForm->CheckUpdatesOnStartTimer->Enabled = false;
  }
  //Unhook eventow
  PluginLink.UnhookEvent(AddLink);
  PluginLink.UnhookEvent(DeleteLink);
  PluginLink.UnhookEvent(OnAddonBrowser);
  PluginLink.UnhookEvent(OnAddonInstalled);
  PluginLink.UnhookEvent(OnThemeChanged);
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
	  if(!Url.IsEmpty()) SetUpdateLink(false,Url);
	}
  }
  delete Links;
  delete Ini;

  return 0;
}
//---------------------------------------------------------------------------

extern "C" __declspec(dllexport) PPluginInfo __stdcall AQQPluginInfo(DWORD AQQVersion)
{
  //Sprawdzanie wersji AQQ
  if(PLUGIN_COMPARE_VERSION(AQQVersion,PLUGIN_MAKE_VERSION(2,3,0,15))<0)
   ChkAQQVersion = false;
  PluginInfo.cbSize = sizeof(TPluginInfo);
  PluginInfo.ShortName = L"FixUpdater";
  PluginInfo.Version = PLUGIN_MAKE_VERSION(1,1,0,0);
  PluginInfo.Description = L"Dodawanie w³asnych serwerów aktualizacji dodatków";
  PluginInfo.Author = L"Krzysztof Grochocki (Beherit)";
  PluginInfo.AuthorMail = L"kontakt@beherit.pl";
  PluginInfo.Copyright = L"Krzysztof Grochocki (Beherit)";
  PluginInfo.Homepage = L"http://beherit.pl/";

  return &PluginInfo;
}
//---------------------------------------------------------------------------
