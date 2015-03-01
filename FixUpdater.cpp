//---------------------------------------------------------------------------
// Copyright (C) 2009-2015 Krzysztof Grochocki
//
// This file is part of FixUpdater
//
// FixUpdater is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// FixUpdater is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GNU Radio; see the file COPYING. If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street,
// Boston, MA 02110-1301, USA.
//---------------------------------------------------------------------------

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
//FORWARD-AQQ-HOOKS----------------------------------------------------------
INT_PTR __stdcall OnAddLink(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnAddonInstalled(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnColorChange(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnDeleteLink(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnModulesLoaded(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnThemeChanged(WPARAM wParam, LPARAM lParam);
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

//Sprawdzanie czy wlaczona jest zaawansowana stylizacja okien
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

//Sprawdzanie ustawien animacji AlphaControls
bool ChkThemeAnimateWindows()
{
	TStrings* IniList = new TStringList();
	IniList->SetText((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0));
	TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
	Settings->SetStrings(IniList);
	delete IniList;
	UnicodeString AnimateWindowsEnabled = Settings->ReadString("Theme","ThemeAnimateWindows","1");
	delete Settings;
	return StrToBool(AnimateWindowsEnabled);
}
//---------------------------------------------------------------------------
bool ChkThemeGlowing()
{
	TStrings* IniList = new TStringList();
	IniList->SetText((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0));
	TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
	Settings->SetStrings(IniList);
	delete IniList;
	UnicodeString GlowingEnabled = Settings->ReadString("Theme","ThemeGlowing","1");
	delete Settings;
	return StrToBool(GlowingEnabled);
}
//---------------------------------------------------------------------------

//Pobieranie ustawien koloru AlphaControls
int GetHUE()
{
	return (int)PluginLink.CallService(AQQ_SYSTEM_COLORGETHUE,0,0);
}
//---------------------------------------------------------------------------
int GetSaturation()
{
	return (int)PluginLink.CallService(AQQ_SYSTEM_COLORGETSATURATION,0,0);
}
//---------------------------------------------------------------------------
int GetBrightness()
{
	return (int)PluginLink.CallService(AQQ_SYSTEM_COLORGETBRIGHTNESS,0,0);
}
//---------------------------------------------------------------------------

//Sprawdzanie dostepnosci aktualizacji
void CheckUpdates(int Mode)
{
	if(Mode==0) //Normalne aktualizacje
		PluginLink.CallService(AQQ_FUNCTION_SILENTUPDATECHECK,0,0);
	else if(Mode==1) //Wymuszanie
		PluginLink.CallService(AQQ_FUNCTION_SILENTUPDATECHECK,0,1);
	else if(Mode==2) //Wersje Beta AQQ
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

//Dodawanie lub usuwanie adresow repozytorium
void SetUpdateLink(UnicodeString URL, bool Enabled)
{
	PluginLink.CallService(AQQ_SYSTEM_SETUPDATELINK,!Enabled,(LPARAM)URL.w_str());
}
//---------------------------------------------------------------------------

//Hook na dodawanie repozytorium przez zewnetrzne wtyczki
INT_PTR __stdcall OnAddLink(WPARAM wParam, LPARAM lParam)
{
	//Pobieranie adresu repozytorium
	UnicodeString URL = (wchar_t*)lParam;
	URL = URL.Trim();
	//Jezeli pobrane repozytorium nie jest puste
	if(!URL.IsEmpty())
	{
		//Pobieranie informacji o aktywacji repozytorium
		UnicodeString Enable = (WPARAM)wParam;
		//Odczyt ustawien wtyczki na formie ustawien
		hMainForm->aLoadSettings->Execute();
		//Szukanie wskazanego adresu
		for(int Count=0;Count<hMainForm->UrlListPreview->Items->Count;Count++)
		{
			//Wskazane repozytorium juz istenieje
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
	//Pobrane repozytorium jest puste
	else return 2;
}
//---------------------------------------------------------------------------

//Hook na instalowanie dodatkow
INT_PTR __stdcall OnAddonInstalled(WPARAM wParam, LPARAM lParam)
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

//Hook na zmiane kolorystyki AlphaControls
INT_PTR __stdcall OnColorChange(WPARAM wParam, LPARAM lParam)
{
	//Okno ustawien zostalo juz stworzone
	if(hMainForm)
	{
		//Wlaczona zaawansowana stylizacja okien
		if(ChkSkinEnabled())
		{
			TPluginColorChange ColorChange = *(PPluginColorChange)wParam;
			hMainForm->sSkinManager->HueOffset = ColorChange.Hue;
			hMainForm->sSkinManager->Saturation = ColorChange.Saturation;
			hMainForm->sSkinManager->Brightness = ColorChange.Brightness;
		}
	}

	return 0;
}
//---------------------------------------------------------------------------

//Hook na usuwanie repozytorium przez zewnetrzne wtyczki
INT_PTR __stdcall OnDeleteLink(WPARAM wParam, LPARAM lParam)
{
	//Pobieranie adresu repozytorium
	UnicodeString URL = (wchar_t*)lParam;
	URL = URL.Trim();
	//Jezeli pobrane repozytorium nie jest puste
	if(!URL.IsEmpty())
	{
		//Odczyt ustawien wtyczki na formie ustawien
		hMainForm->aLoadSettings->Execute();
		//Szukanie wskazanego repozytorium na formie ustawien
		for(int Count=0;Count<hMainForm->UrlListPreview->Items->Count;Count++)
		{
			//Porownanie rekordu z przekazanym w notyfikacji adresem repozytorium
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
		//Brak repozytorium
		return 2;
	}
	//Pobrane repozytorium jest puste
	else return 2;
}
//---------------------------------------------------------------------------

//Hook na zaladowanie wszystkich modolow
INT_PTR __stdcall OnModulesLoaded(WPARAM, LPARAM)
{
	//Wlaczenie timera sprawdzania aktualizacji dodatkow
	hMainForm->CheckUpdatesOnStartTimer->Enabled = true;

	return 0;
}
//---------------------------------------------------------------------------

//Hook na zmiane kompozycji
INT_PTR __stdcall OnThemeChanged (WPARAM wParam, LPARAM lParam)
{
	//Okno ustawien zostalo juz stworzone
	if(hMainForm)
	{
		//Wlaczona zaawansowana stylizacja okien
		if(ChkSkinEnabled())
		{
			//Pobieranie sciezki nowej aktywnej kompozycji
			UnicodeString ThemeSkinDir = StringReplace((wchar_t*)lParam, "\\", "\\\\", TReplaceFlags() << rfReplaceAll) + "\\\\Skin";
			//Plik zaawansowanej stylizacji okien istnieje
			if(FileExists(ThemeSkinDir + "\\\\Skin.asz"))
			{
				//Dane pliku zaawansowanej stylizacji okien
				ThemeSkinDir = StringReplace(ThemeSkinDir, "\\\\", "\\", TReplaceFlags() << rfReplaceAll);
				hMainForm->sSkinManager->SkinDirectory = ThemeSkinDir;
				hMainForm->sSkinManager->SkinName = "Skin.asz";
				//Ustawianie animacji AlphaControls
				if(ChkThemeAnimateWindows()) hMainForm->sSkinManager->AnimEffects->FormShow->Time = 200;
				else hMainForm->sSkinManager->AnimEffects->FormShow->Time = 0;
				hMainForm->sSkinManager->Effects->AllowGlowing = ChkThemeGlowing();
				//Zmiana kolorystyki AlphaControls
				hMainForm->sSkinManager->HueOffset = GetHUE();
				hMainForm->sSkinManager->Saturation = GetSaturation();
				hMainForm->sSkinManager->Brightness = GetBrightness();
				//Aktywacja skorkowania AlphaControls
				hMainForm->sSkinManager->Active = true;
			}
			//Brak pliku zaawansowanej stylizacji okien
			else hMainForm->sSkinManager->Active = false;
		}
		//Zaawansowana stylizacja okien wylaczona
		else hMainForm->sSkinManager->Active = false;
	}

	return 0;
}
//---------------------------------------------------------------------------

//Zapisywanie zasobów
void ExtractRes(wchar_t* FileName, wchar_t* ResName, wchar_t* ResType)
{
	TPluginTwoFlagParams PluginTwoFlagParams;
	PluginTwoFlagParams.cbSize = sizeof(TPluginTwoFlagParams);
	PluginTwoFlagParams.Param1 = ResName;
	PluginTwoFlagParams.Param2 = ResType;
	PluginTwoFlagParams.Flag1 = (int)HInstance;
	PluginLink.CallService(AQQ_FUNCTION_SAVERESOURCE,(WPARAM)&PluginTwoFlagParams,(LPARAM)FileName);
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
	else return 0;
}
//---------------------------------------------------------------------------

extern "C" INT_PTR __declspec(dllexport) __stdcall Load(PPluginLink Link)
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
		ExtractRes((PluginUserDir+"\\\\Shared\\\\FixUpdater.dll.png").w_str(),L"SHARED",L"DATA");
	else if(MD5File(PluginUserDir+"\\\\Shared\\\\FixUpdater.dll.png")!="3EA122B23FBF8835FDE23DCD1CC9968B")
		ExtractRes((PluginUserDir+"\\\\Shared\\\\FixUpdater.dll.png").w_str(),L"SHARED",L"DATA");
	//Ustawienia domyœlne wtyczki
	if(!FileExists(PluginUserDir+"\\\\FixUpdater\\\\Settings.ini"))
	{
		//Tworzenie folderu z ustawieniami wtyczki
		if(!DirectoryExists(PluginUserDir + "\\\\FixUpdater"))
		CreateDir(PluginUserDir + "\\\\FixUpdater");
		//Tworzenie pliku ustawien z domyslnym repozytorium
		TIniFile *Ini = new TIniFile(PluginUserDir + "\\\\FixUpdater\\\\Settings.ini");
		Ini->WriteString("Links", "Url1", "http://beherit.pl/aqq_update/stable.xml");
		Ini->WriteBool("Links", "Enable1", true);
		Ini->WriteString("Links", "Url2", "http://beherit.pl/aqq_update/beta.xml");
		Ini->WriteBool("Links", "Enable2", false);
		delete Ini;
	}
	//Odczyt ustawien wtyczki
	TIniFile *Ini = new TIniFile(PluginUserDir + "\\\\FixUpdater\\\\Settings.ini");
	//Repozytorium
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
	//Zakonczenie odczytu ustawien wtyczki
	delete Ini;
	//Hook na dodawanie repozytorium przez zewnetrzne wtyczki
	PluginLink.HookEvent(FIXUPDATER_SYSTEM_ADDLINK,OnAddLink);
	//Hook na instalowanie dodatkow
	PluginLink.HookEvent(AQQ_SYSTEM_ADDONINSTALLED,OnAddonInstalled);
	//Hook na zmiane kolorystyki AlphaControls
	PluginLink.HookEvent(AQQ_SYSTEM_COLORCHANGEV2,OnColorChange);
	//Hook na usuwanie repozytorium przez zewnetrzne wtyczki
	PluginLink.HookEvent(FIXUPDATER_SYSTEM_DELETELINK,OnDeleteLink);
	//Hook na zaladowanie wszystkich modolow
	PluginLink.HookEvent(AQQ_SYSTEM_MODULESLOADED,OnModulesLoaded);
	//Hook na zmiane kompozycji
	PluginLink.HookEvent(AQQ_SYSTEM_THEMECHANGED,OnThemeChanged);
	//Wlaczenie timera sprawdzania aktualizacji dodatkow
	if(PluginLink.CallService(AQQ_SYSTEM_MODULESLOADED,0,0))
		hMainForm->CheckUpdatesOnStartTimer->Enabled = true;

	return 0;
}
//---------------------------------------------------------------------------

extern "C" INT_PTR __declspec(dllexport)__stdcall Settings()
{
	//Pokaznie okna ustawien
	hMainForm->Show();

	return 0;
}
//---------------------------------------------------------------------------

extern "C" INT_PTR __declspec(dllexport) __stdcall Unload()
{
	//Wylaczanie timerow
	if(hMainForm)
	{
		hMainForm->CheckUpdatesTimer->Enabled = false;
		hMainForm->CheckUpdatesOnStartTimer->Enabled = false;
	}
	//Wyladowanie wszystkich hookow
	PluginLink.UnhookEvent(OnAddLink);
	PluginLink.UnhookEvent(OnAddonInstalled);
	PluginLink.UnhookEvent(OnColorChange);
	PluginLink.UnhookEvent(OnDeleteLink);
	PluginLink.UnhookEvent(OnModulesLoaded);
	PluginLink.UnhookEvent(OnThemeChanged);
	//Usuniecie adresow repozytorium z aktualizatora
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
	PluginInfo.Version = PLUGIN_MAKE_VERSION(1,4,6,0);
	PluginInfo.Description = L"Ulepsza system aktualizacji poprzez mo¿liwoœæ dodawania dodatkowych adresów serwerów zawieraj¹cych bazê dodatków oraz ustawienie czêstszego interwa³u sprawdzania aktualizacji.";
	PluginInfo.Author = L"Krzysztof Grochocki";
	PluginInfo.AuthorMail = L"kontakt@beherit.pl";
	PluginInfo.Copyright = L"Krzysztof Grochocki";
	PluginInfo.Homepage = L"http://beherit.pl/";
	PluginInfo.Flag = 0;
	PluginInfo.ReplaceDefaultModule = 0;

	return &PluginInfo;
}
//---------------------------------------------------------------------------
