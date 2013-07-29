#include <vcl.h>
#include <windows.h>
#pragma hdrstop
#pragma argsused
#include "MainFrm.h"
#include <PluginAPI.h>
#include <inifiles.hpp>
#include <IdHashMessageDigest.hpp>
#define FIXUPDATER_SYSTEM_ADDLINK L"FixUpdater/System/AddLink"
#define FIXUPDATER_SYSTEM_DELETELINK L"FixUpdater/System/DeleteLink"

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
  return 1;
}
//---------------------------------------------------------------------------

//Uchwyt-do-formy-ustawien---------------------------------------------------
TMainForm *hMainForm;
//Struktury-glowne-----------------------------------------------------------
TPluginLink PluginLink;
TPluginInfo PluginInfo;
//SETTINGS-------------------------------------------------------------------
bool ChangeAddonBrowserChk;
//FORWARD-AQQ-HOOKS----------------------------------------------------------
int __stdcall OnAddLink(WPARAM wParam, LPARAM lParam);
int __stdcall OnAddonBrowser(WPARAM wParam, LPARAM lParam);
int __stdcall OnAddonInstalled(WPARAM wParam, LPARAM lParam);
int __stdcall OnDeleteLink(WPARAM wParam, LPARAM lParam);
int __stdcall OnModulesLoaded(WPARAM wParam, LPARAM lParam);
int __stdcall OnThemeChanged(WPARAM wParam, LPARAM lParam);
//---------------------------------------------------------------------------

//Pobieranie sciezki katalogu prywatnego wtyczek
UnicodeString GetPluginUserDir()
{
  return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,0,0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
}
//---------------------------------------------------------------------------

//Pobieranie sciezki do skorki kompozycji
UnicodeString GetThemeSkinDir()
{
  return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETTHEMEDIR,0,0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll) + "\\\\Skin";
}
//---------------------------------------------------------------------------

//Sprawdzanie czy wlaczona jest obsluga stylow obramowania okien
bool ChkSkinEnabled()
{
  TStrings* IniList = new TStringList();
  IniList->SetText((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0));
  TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
  Settings->SetStrings(IniList);
  delete IniList;
  UnicodeString SkinsEnabled = Settings->ReadString("Settings","UseSkin","1");
  delete Settings;
  return StrToBool(SkinsEnabled);
}
//---------------------------------------------------------------------------

//Sprawdzanie czy wlaczony jest natywny styl Windows
bool ChkNativeEnabled()
{
  TStrings* IniList = new TStringList();
  IniList->SetText((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0));
  TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
  Settings->SetStrings(IniList);
  delete IniList;
  UnicodeString NativeEnabled = Settings->ReadString("Settings","Native","0");
  delete Settings;
  return StrToBool(NativeEnabled);
}
//---------------------------------------------------------------------------

//Zmiana ustawien przegladarki dodatkow
void ChangeAddonBrowser(bool Enabled)
{
  ChangeAddonBrowserChk = Enabled;
}
//---------------------------------------------------------------------------

//Sprawdzanie dostepnosci aktualizacji
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

//Pobieranie daty ostatniego sprawdzania aktualizacji
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

//Dodawanie lub usuwanie linkow kanalow aktualizacji
void SetUpdateLink(UnicodeString URL, bool Enabled)
{
  PluginLink.CallService(AQQ_SYSTEM_SETUPDATELINK,!Enabled,(LPARAM)URL.w_str());
}
//---------------------------------------------------------------------------

//Hook na dodawanie kana³ow przez zewnetrzne wtyczki
int __stdcall OnAddLink(WPARAM wParam, LPARAM lParam)
{
  //Pobieranie adresu kanalu
  UnicodeString URL = (wchar_t*)lParam;
  URL = URL.Trim();
  //Jezeli pobrany kanal nie jest pusty
  if(!URL.IsEmpty())
  {
	//Pobieranie informacji o aktywacji kanalu
	UnicodeString Enable = (WPARAM)wParam;
	//Odczyt ustawien wtyczki na formie ustawien
	hMainForm->aLoadSettings->Execute();
	//Szukanie wskazanego adresu
	for(int Count=0;Count<hMainForm->UrlListPreview->Items->Count;Count++)
	{
	  //Wskazany kanal juz istenieje
	  if(hMainForm->UrlListPreview->Items->Item[Count]->SubItems->Strings[0]==URL)
	   return 2;
	}
	//Dodawanie wskazanego adresu
	int Count = hMainForm->UrlListPreview->Items->Count;
	hMainForm->UrlListPreview->Items->Add();
	hMainForm->UrlListPreview->Items->Item[Count]->Checked=StrToBool(Enable);
	hMainForm->UrlListPreview->Items->Item[Count]->SubItems->Add(URL);
	//Zapisanie ustawien
	hMainForm->aSaveSettings->Execute();
	//Zwrocenie info o pomyslnej operacji
	return 1;
  }
  //Pobrany kanal jest pusty
  else
   return 2;
}
//---------------------------------------------------------------------------

//Hook na pokazywanie browsera dodatkow
int __stdcall OnAddonBrowser(WPARAM wParam, LPARAM lParam)
{
  //Adres przegladarki dodatkow ma byc zmieniany
  if(ChangeAddonBrowserChk) return (LPARAM)L"http://addons.aqqnews.pl/";
  //Adres przegladarki dodatkow nie ma byc zmieniany
  else return 0;
}
//---------------------------------------------------------------------------

//Hook na instalowanie dodatkow
int __stdcall OnAddonInstalled(WPARAM wParam, LPARAM lParam)
{
  //Pobranie typu zainstalowanego dodatku
  PPluginTriple PluginTriple = (PPluginTriple)wParam;
  if(PluginTriple->Param1==tatPlugin)
  {
    //Pobranie sciezki do dodatku
	UnicodeString AddonFile = (wchar_t*)lParam ;
	if(ExtractFileName(AddonFile)!="FixUpdater.dll")
	{
	  //Wylaczenie timerow sprawdzania aktualizacji dodatkow
	  hMainForm->CheckUpdatesTimer->Enabled = false;
	  hMainForm->CheckUpdatesOnStartTimer->Enabled = false;
	  //Ustawienie interwalu timera
	  hMainForm->CheckUpdatesOnStartTimer->Interval = 2000;
	  //Wlaczenie timera sprawdzania aktualizacji dodatkow
	  hMainForm->CheckUpdatesOnStartTimer->Enabled = true;
    }
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na usuwanie kana³ow przez zewnetrzne wtyczki
int __stdcall OnDeleteLink(WPARAM wParam, LPARAM lParam)
{
  //Pobieranie adresu kanalu
  UnicodeString URL = (wchar_t*)lParam;
  URL = URL.Trim();
  //Jezeli pobrany kanal nie jest pusty
  if(!URL.IsEmpty())
  {
	//Odczyt ustawien wtyczki na formie ustawien
	hMainForm->aLoadSettings->Execute();
	//Szukanie wskazanego kanalu na formie ustawien
	for(int Count=0;Count<hMainForm->UrlListPreview->Items->Count;Count++)
	{
      //Porownanie rekordu z przekazanym w notyfikacji adresem kanalu
	  if(hMainForm->UrlListPreview->Items->Item[Count]->SubItems->Strings[0]==URL)
	  {
		//Usuniecie wskazanej pozycji
		hMainForm->UrlListPreview->Items->Delete(Count);
		//Zapisanie ustawien
		hMainForm->aSaveSettings->Execute();
		//Zwrocenie info o pomyslnej operacji
		return 1;
	  }
	}
	//Brak kanalu
	return 2;
  }
  //Pobrany kanal jest pusty
  else
   return 2;
}
//---------------------------------------------------------------------------

//Hook na zaladowanie wszystkich modolow
int __stdcall OnModulesLoaded(WPARAM, LPARAM)
{
  //Wlaczenie timera sprawdzania aktualizacji dodatkow
  hMainForm->CheckUpdatesOnStartTimer->Enabled = true;

  return 0;
}
//---------------------------------------------------------------------------

//Hook na zmiane kompozycji
int __stdcall OnThemeChanged (WPARAM wParam, LPARAM lParam)
{
  //Pobieranie sciezki nowej aktywnej kompozycji
  UnicodeString ThemeDir = StringReplace((wchar_t*)lParam, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
  //Zmiana skorki wtyczki
  if(hMainForm)
  {
	//Wlaczenie skorkowania
	if((FileExists(ThemeDir+"\\\\Skin\\\\Skin.asz"))&&(!ChkNativeEnabled()))
	{
	  UnicodeString ThemeSkinDir = ThemeDir+"\\\\Skin";
	  ThemeSkinDir = StringReplace(ThemeSkinDir, "\\\\", "\\", TReplaceFlags() << rfReplaceAll);
	  hMainForm->sSkinManager->SkinDirectory = ThemeSkinDir;
	  hMainForm->sSkinManager->SkinName = "Skin.asz";
	  hMainForm->sSkinProvider->DrawNonClientArea = ChkSkinEnabled();
	  hMainForm->sSkinManager->Active = true;
	}
	//Wylaczenie skorkowania
	else
	 hMainForm->sSkinManager->Active = false;
  }

  return 0;
}
//---------------------------------------------------------------------------

//Zapisywanie zasobów
bool SaveResourceToFile(wchar_t* FileName, wchar_t* Res)
{
  HRSRC hrsrc = FindResource(HInstance, Res, RT_RCDATA);
  if(!hrsrc) return false;
  DWORD size = SizeofResource(HInstance, hrsrc);
  HGLOBAL hglob = LoadResource(HInstance, hrsrc);
  LPVOID rdata = LockResource(hglob);
  HANDLE hFile = CreateFile(FileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  DWORD writ;
  WriteFile(hFile, rdata, size, &writ, NULL);
  CloseHandle(hFile);
  return true;
}
//---------------------------------------------------------------------------

//Obliczanie sumy kontrolnej pliku
UnicodeString MD5File(UnicodeString FileName)
{
  if(FileExists(FileName))
  {
	UnicodeString Result;
    TFileStream *fs;

	fs = new TFileStream(FileName, fmOpenRead | fmShareDenyWrite);
	try
	{
	  TIdHashMessageDigest5 *idmd5= new TIdHashMessageDigest5();
	  try
	  {
	    Result = idmd5->HashStreamAsHex(fs);
	  }
	  __finally
	  {
	    delete idmd5;
	  }
    }
	__finally
    {
	  delete fs;
    }

    return Result;
  }
  else
   return 0;
}
//---------------------------------------------------------------------------

extern "C" int __declspec(dllexport) __stdcall Load(PPluginLink Link)
{
  //Linkowanie wtyczki z komunikatorem
  PluginLink = *Link;
  //Przypisanie uchwytu do formy ustawien
  if(!hMainForm)
  {
	Application->Handle = (HWND)MainForm;
	hMainForm = new TMainForm(Application);
  }
  //Sciezka folderu prywatnego wtyczek
  UnicodeString PluginUserDir = GetPluginUserDir();
  //Wypakiwanie ikonki FixUpdater.dll.png
  //3EA122B23FBF8835FDE23DCD1CC9968B
  if(!DirectoryExists(PluginUserDir+"\\\\Shared"))
   CreateDir(PluginUserDir+"\\\\Shared");
  if(!FileExists(PluginUserDir+"\\\\Shared\\\\FixUpdater.dll.png"))
   SaveResourceToFile((PluginUserDir+"\\\\Shared\\\\FixUpdater.dll.png").w_str(),L"PLUGIN_RES");
  else if(MD5File(PluginUserDir+"\\\\Shared\\\\FixUpdater.dll.png")!="3EA122B23FBF8835FDE23DCD1CC9968B")
   SaveResourceToFile((PluginUserDir+"\\\\Shared\\\\FixUpdater.dll.png").w_str(),L"PLUGIN_RES");
  //Ustawienia domyœlne wtyczki
  if(!FileExists(PluginUserDir+"\\\\FixUpdater\\\\Settings.ini"))
  {
	//Tworzenie folderu z ustawieniami wtyczki
	if(!DirectoryExists(PluginUserDir + "\\\\FixUpdater"))
	 CreateDir(PluginUserDir + "\\\\FixUpdater");
    //Tworzenie pliku ustawien z domyslnymi kanalami
	TIniFile *Ini = new TIniFile(PluginUserDir + "\\\\FixUpdater\\\\Settings.ini");
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
  //Odczyt ustawien wtyczki
  TIniFile *Ini = new TIniFile(PluginUserDir + "\\\\FixUpdater\\\\Settings.ini");
  //Kanaly aktualizacji
  TStringList *Links = new TStringList;
  Ini->ReadSection("Links",Links);
  for(int Count=0;Count<Links->Count/2;Count++)
  {
	if(Ini->ReadBool("Links", "Enable" + IntToStr(Count+1), true))
	{
	  UnicodeString Url = Ini->ReadString("Links", "Url" + IntToStr(Count+1), "");
	  if(!Url.IsEmpty()) SetUpdateLink(Url,true);
	}
  }
  delete Links;
  //Sposob aktualizacji
  hMainForm->UpdateMode = Ini->ReadInteger("Settings", "UpdateMode", 0);
  //Czestotliwosci aktualizacji
  hMainForm->UpdateTime = Ini->ReadInteger("Settings", "UpdateTime", 0);
  //Menedzer dodatkow
  ChangeAddonBrowserChk = Ini->ReadBool("Settings", "ChangeAddonBrowser", true);
  //Zakonczenie odczytu ustawien wtyczki
  delete Ini;
  //Hook na dodawanie kana³ow przez zewnetrzne wtyczki
  PluginLink.HookEvent(FIXUPDATER_SYSTEM_ADDLINK,OnAddLink);
  //Hook na pokazywanie browsera dodatkow
  PluginLink.HookEvent(AQQ_SYSTEM_ADDONBROWSER_URL,OnAddonBrowser);
  //Hook na instalowanie dodatkow
  PluginLink.HookEvent(AQQ_SYSTEM_ADDONINSTALLED,OnAddonInstalled);
  //Hook na usuwanie kana³ow przez zewnetrzne wtyczki
  PluginLink.HookEvent(FIXUPDATER_SYSTEM_DELETELINK,OnDeleteLink);
  //Hook na zaladowanie wszystkich modolow
  PluginLink.HookEvent(AQQ_SYSTEM_MODULESLOADED, OnModulesLoaded);
  //Hook na zmiane kompozycji
  PluginLink.HookEvent(AQQ_SYSTEM_THEMECHANGED,OnThemeChanged);
  //Wlaczenie timera sprawdzania aktualizacji dodatkow
  if(PluginLink.CallService(AQQ_SYSTEM_MODULESLOADED,0,0))
   hMainForm->CheckUpdatesOnStartTimer->Enabled = true;

  return 0;
}
//---------------------------------------------------------------------------

extern "C" int __declspec(dllexport)__stdcall Settings()
{
  //Pokaznie okna ustawien
  hMainForm->Show();

  return 0;
}
//---------------------------------------------------------------------------

extern "C" int __declspec(dllexport) __stdcall Unload()
{
  //Wylaczanie timerow
  if(hMainForm)
  {
	hMainForm->CheckUpdatesTimer->Enabled = false;
	hMainForm->CheckUpdatesOnStartTimer->Enabled = false;
  }
  //Wyladowanie wszystkich hookow
  PluginLink.UnhookEvent(OnAddLink);
  PluginLink.UnhookEvent(OnAddonBrowser);
  PluginLink.UnhookEvent(OnAddonInstalled);
  PluginLink.UnhookEvent(OnDeleteLink);
  PluginLink.UnhookEvent(OnModulesLoaded);
  PluginLink.UnhookEvent(OnThemeChanged);
  //Usuniecie adresow kanalow aktualizacji z aktualizatora
  TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\FixUpdater\\\\Settings.ini");
  TStringList *Links = new TStringList;
  Ini->ReadSection("Links",Links);
  for(int Count=0;Count<Links->Count/2;Count++)
  {
	if(Ini->ReadBool("Links", "Enable" + IntToStr(Count+1), true))
	{
	  UnicodeString Url = Ini->ReadString("Links", "Url" + IntToStr(Count+1), "");
	  if(!Url.IsEmpty()) SetUpdateLink(Url,false);
	}
  }
  delete Links;
  delete Ini;

  return 0;
}
//---------------------------------------------------------------------------

//Informacje o wtyczce
extern "C" __declspec(dllexport) PPluginInfo __stdcall AQQPluginInfo(DWORD AQQVersion)
{
  PluginInfo.cbSize = sizeof(TPluginInfo);
  PluginInfo.ShortName = L"FixUpdater";
  PluginInfo.Version = PLUGIN_MAKE_VERSION(1,2,1,0);
  PluginInfo.Description = L"Wtyczka ulepszaj¹ca system aktualizacji w AQQ poprzez mo¿liwoœæ dodawania w³asnych adresów do serwerów zawieraj¹cych aktualizacje.";
  PluginInfo.Author = L"Krzysztof Grochocki (Beherit)";
  PluginInfo.AuthorMail = L"kontakt@beherit.pl";
  PluginInfo.Copyright = L"Krzysztof Grochocki (Beherit)";
  PluginInfo.Homepage = L"http://beherit.pl/";

  return &PluginInfo;
}
//---------------------------------------------------------------------------
