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
#include <inifiles.hpp>
#include <IdHashMessageDigest.hpp>
#include <PluginAPI.h>
#include <LangAPI.hpp>
#pragma hdrstop
#pragma argsused
#include "SettingsFrm.h"
#define FIXUPDATER_SYSTEM_ADDLINK L"FixUpdater/System/AddLink"
#define FIXUPDATER_SYSTEM_DELETELINK L"FixUpdater/System/DeleteLink"

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
	return 1;
}
//---------------------------------------------------------------------------

//Uchwyt-do-formy-ustawien---------------------------------------------------
TSettingsForm *hSettingsForm;
//Struktury-glowne-----------------------------------------------------------
TPluginLink PluginLink;
TPluginInfo PluginInfo;
//Uchwyt-do-okna-timera------------------------------------------------------
HWND hTimerFrm;
//Gdy-zostalo-uruchomione-wyladowanie-wtyczki-wraz-z-zamknieciem-komunikatora
bool ForceUnloadExecuted = false;
//TIMERY---------------------------------------------------------------------
#define TIMER_CHKUPDATES 10
//SETTINGS-------------------------------------------------------------------
int UpdateMode;
int UpdateInterval;
//FORWARD-TIMER--------------------------------------------------------------
LRESULT CALLBACK TimerFrmProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
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
	return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR, 0, 0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
}
//---------------------------------------------------------------------------

//Pobieranie sciezki do skorki kompozycji
UnicodeString GetThemeSkinDir()
{
	return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETTHEMEDIR, 0, 0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll) + "\\\\Skin";
}
//---------------------------------------------------------------------------

//Sprawdzanie czy wlaczona jest zaawansowana stylizacja okien
bool ChkSkinEnabled()
{
	TStrings* IniList = new TStringList();
	IniList->SetText((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP, 0, 0));
	TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
	Settings->SetStrings(IniList);
	delete IniList;
	UnicodeString SkinsEnabled = Settings->ReadString("Settings", "UseSkin", "1");
	delete Settings;
	return StrToBool(SkinsEnabled);
}
//---------------------------------------------------------------------------

//Sprawdzanie ustawien animacji AlphaControls
bool ChkThemeAnimateWindows()
{
	TStrings* IniList = new TStringList();
	IniList->SetText((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP, 0, 0));
	TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
	Settings->SetStrings(IniList);
	delete IniList;
	UnicodeString AnimateWindowsEnabled = Settings->ReadString("Theme", "ThemeAnimateWindows", "1");
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
	UnicodeString GlowingEnabled = Settings->ReadString("Theme", "ThemeGlowing", "1");
	delete Settings;
	return StrToBool(GlowingEnabled);
}
//---------------------------------------------------------------------------

//Pobieranie ustawien koloru AlphaControls
int GetHUE()
{
	return (int)PluginLink.CallService(AQQ_SYSTEM_COLORGETHUE, 0, 0);
}
//---------------------------------------------------------------------------
int GetSaturation()
{
	return (int)PluginLink.CallService(AQQ_SYSTEM_COLORGETSATURATION, 0, 0);
}
//---------------------------------------------------------------------------
int GetBrightness()
{
	return (int)PluginLink.CallService(AQQ_SYSTEM_COLORGETBRIGHTNESS, 0, 0);
}
//---------------------------------------------------------------------------

//Sprawdzanie dostepnosci aktualizacji
void CheckUpdates(int Mode)
{
	//0 = Aktualizacja zalezna od ustawien AQQ
	//1 = Wymuszanie aktualizacji
	//2 = Wymuszanie aktualizacji + sprawdzanie wersji beta
	PluginLink.CallService(AQQ_FUNCTION_SILENTUPDATECHECK, 0, Mode);
}
//---------------------------------------------------------------------------

//Pobieranie daty ostatniego sprawdzania aktualizacji
UnicodeString GetLastUpdate()
{
	TStrings* IniList = new TStringList();
	IniList->SetText((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP, 0, 0));
	TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
	Settings->SetStrings(IniList);
	delete IniList;
	UnicodeString LastUpdate = Settings->ReadString("User", "LastUpdateDate", "0");
	delete Settings;
	return LastUpdate;
}
//---------------------------------------------------------------------------

//Dodawanie lub usuwanie adresow repozytorium
void SetUpdateLink(UnicodeString URL, bool Enabled)
{
	PluginLink.CallService(AQQ_SYSTEM_SETUPDATELINK, !Enabled, (LPARAM)URL.w_str());
}
//---------------------------------------------------------------------------

//Wylaczenie timera sprawdzania aktualizacji
void KillTimerEx()
{
	KillTimer(hTimerFrm, TIMER_CHKUPDATES);
}
//---------------------------------------------------------------------------

//Wlaczenie timera sprawdzania aktualizacji
void SetTimerEx(int Interval)
{
	SetTimer(hTimerFrm, TIMER_CHKUPDATES, Interval, (TIMERPROC)TimerFrmProc);
}
//---------------------------------------------------------------------------

//Procka okna timera
LRESULT CALLBACK TimerFrmProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg==WM_TIMER)
	{
		//Sprawdzanie aktualizacji
		if(wParam==TIMER_CHKUPDATES)
		{
			//Wylaczenie timera
			KillTimer(hTimerFrm, TIMER_CHKUPDATES);
			//Sprawdzanie dostepnosci aktualizacji
			CheckUpdates(UpdateMode);
			//Ponowne wlaczenie timera sprawdzania aktualizacji
			if(UpdateInterval) SetTimer(hTimerFrm, TIMER_CHKUPDATES, 3600000 * UpdateInterval, (TIMERPROC)TimerFrmProc);
		}

		return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
//---------------------------------------------------------------------------

//Hook na dodawanie repozytorium przez zewnetrzne wtyczki
INT_PTR __stdcall OnAddLink(WPARAM wParam, LPARAM lParam)
{
	//Pobieranie adresu repozytorium
	UnicodeString URL = (wchar_t*)lParam;
	URL = URL.Trim();
	//Pobrany adres repozytorium nie jest pusty
	if(!URL.IsEmpty())
	{
		//Pobieranie informacji o aktywacji repozytorium
		UnicodeString Enable = (WPARAM)wParam;
		//Otwarcie pliku ustawien
		TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\FixUpdater\\\\Settings.ini");
		//Pobieranie ilosci repozytoriow
		TStringList *Links = new TStringList;
		Ini->ReadSection("Links", Links);
		int LinksCount = Links->Count/2;
		delete Links;
		//Szukanie wskazanego adresu na liscie dodanych repozytoriow
		for(int Count=2; Count<LinksCount; Count++)
		{
			//Wskazane repozytorium zostalo juz dodane
			if(URL==Ini->ReadString("Links", "Url" + IntToStr(Count+1), ""))
			{
				//Zamkniecie pliku ustawien
				delete Ini;
				//Zwrocenie info o istnieniu repozytorium
				return 2;
			}
		}
		//Dodawanie wskazanego adresu repozytorium
		Ini->WriteString("Links", "Url" +	IntToStr(LinksCount), URL);
		Ini->WriteBool("Links", "Enable" + IntToStr(LinksCount), StrToBool(Enable));
		//Zamkniecie pliku ustawien
		delete Ini;
		//Dodawanie adresu repozytorium do aktualizatora
		if(StrToBool(Enable)) SetUpdateLink(URL, true);
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
			//Wylaczanie timera sprawdzania aktualizacji
			KillTimer(hTimerFrm, TIMER_CHKUPDATES);
			//Wlaczenie timera sprawdzania aktualizacji
			SetTimer(hTimerFrm, TIMER_CHKUPDATES, 2000, (TIMERPROC)TimerFrmProc);
		}
	}

	return 0;
}
//---------------------------------------------------------------------------

//Hook na wylaczenie komunikatora poprzez usera
INT_PTR __stdcall OnBeforeUnload(WPARAM wParam, LPARAM lParam)
{
	//Info o rozpoczeciu procedury zamykania komunikatora
	ForceUnloadExecuted = true;

	return 0;
}
//---------------------------------------------------------------------------

//Hook na zmiane kolorystyki AlphaControls
INT_PTR __stdcall OnColorChange(WPARAM wParam, LPARAM lParam)
{
	//Okno ustawien zostalo juz stworzone
	if(hSettingsForm)
	{
		//Wlaczona zaawansowana stylizacja okien
		if(ChkSkinEnabled())
		{
			TPluginColorChange ColorChange = *(PPluginColorChange)wParam;
			hSettingsForm->sSkinManager->HueOffset = ColorChange.Hue;
			hSettingsForm->sSkinManager->Saturation = ColorChange.Saturation;
			hSettingsForm->sSkinManager->Brightness = ColorChange.Brightness;
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
	//Pobrany adres repozytorium nie jest pusty
	if(!URL.IsEmpty())
	{
    //Otwarcie pliku ustawien
		TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\FixUpdater\\\\Settings.ini");
		//Pobieranie ilosci repozytoriow
		TStringList *Links = new TStringList;
		Ini->ReadSection("Links", Links);
		int LinksCount = Links->Count/2;
		delete Links;
		//Szukanie wskazanego repozytorium na formie ustawien
		for(int Count=2; Count<LinksCount; Count++)
		{
			//Wskazane repozytorium zostalo dodane
			if(URL==Ini->ReadString("Links", "Url" + IntToStr(Count+1), ""))
			{
				//Przesuniecie ostatniego repozytorium
				Ini->WriteString("Links", "Url" + IntToStr(Count+1), Ini->ReadString("Links", "Url" + IntToStr(LinksCount), ""));
				Ini->WriteBool("Links", "Enable" + IntToStr(Count+1), Ini->ReadBool("Links", "Enable" + IntToStr(LinksCount), ""));
				Ini->DeleteKey("Links", "Url" + IntToStr(LinksCount));
				Ini->DeleteKey("Links", "Enable" + IntToStr(LinksCount));
				//Usuwanie adresu repozytorium z aktualizatora
				SetUpdateLink(URL, false);
				//Zamkniecie pliku ustawien
				delete Ini;
				//Zwrocenie info o pomyslnej operacji
				return 1;
			}
		}
		//Zamkniecie pliku ustawien
		delete Ini;
		//Brak repozytorium
		return 2;
	}
	//Pobrane repozytorium jest puste
	else return 2;
}
//---------------------------------------------------------------------------

//Hook na zmiane lokalizacji
INT_PTR __stdcall OnLangCodeChanged(WPARAM wParam, LPARAM lParam)
{
	//Czyszczenie cache lokalizacji
	ClearLngCache();
	//Pobranie sciezki do katalogu prywatnego uzytkownika
	UnicodeString PluginUserDir = GetPluginUserDir();
	//Ustawienie sciezki lokalizacji wtyczki
	UnicodeString LangCode = (wchar_t*)lParam;
	LangPath = PluginUserDir + "\\\\Languages\\\\FixUpdater\\\\" + LangCode + "\\\\";
	if(!DirectoryExists(LangPath))
	{
		LangCode = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETDEFLANGCODE,0,0);
		LangPath = PluginUserDir + "\\\\Languages\\\\FixUpdater\\\\" + LangCode + "\\\\";
	}
	//Aktualizacja lokalizacji form wtyczki
	for(int i=0;i<Screen->FormCount;i++)
		LangForm(Screen->Forms[i]);
	if(hSettingsForm) hSettingsForm->aLoadSettings->Execute();

	return 0;
}
//---------------------------------------------------------------------------

//Hook na zaladowanie wszystkich modolow
INT_PTR __stdcall OnModulesLoaded(WPARAM, LPARAM)
{
	//Wlaczenie timera sprawdzania aktualizacji
	SetTimer(hTimerFrm, TIMER_CHKUPDATES, 300000, (TIMERPROC)TimerFrmProc);

	return 0;
}
//---------------------------------------------------------------------------

//Hook na zmiane kompozycji
INT_PTR __stdcall OnThemeChanged (WPARAM wParam, LPARAM lParam)
{
	//Okno ustawien zostalo juz stworzone
	if(hSettingsForm)
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
				hSettingsForm->sSkinManager->SkinDirectory = ThemeSkinDir;
				hSettingsForm->sSkinManager->SkinName = "Skin.asz";
				//Ustawianie animacji AlphaControls
				if(ChkThemeAnimateWindows()) hSettingsForm->sSkinManager->AnimEffects->FormShow->Time = 200;
				else hSettingsForm->sSkinManager->AnimEffects->FormShow->Time = 0;
				hSettingsForm->sSkinManager->Effects->AllowGlowing = ChkThemeGlowing();
				//Zmiana kolorystyki AlphaControls
				hSettingsForm->sSkinManager->HueOffset = GetHUE();
				hSettingsForm->sSkinManager->Saturation = GetSaturation();
				hSettingsForm->sSkinManager->Brightness = GetBrightness();
				//Aktywacja skorkowania AlphaControls
				hSettingsForm->sSkinManager->Active = true;
			}
			//Brak pliku zaawansowanej stylizacji okien
			else hSettingsForm->sSkinManager->Active = false;
		}
		//Zaawansowana stylizacja okien wylaczona
		else hSettingsForm->sSkinManager->Active = false;
	}

	return 0;
}
//---------------------------------------------------------------------------

//Zapisywanie plikow z zasobow
void ExtractRes(wchar_t* FileName, wchar_t* ResName, wchar_t* ResType)
{
	TPluginTwoFlagParams PluginTwoFlagParams;
	PluginTwoFlagParams.cbSize = sizeof(TPluginTwoFlagParams);
	PluginTwoFlagParams.Param1 = ResName;
	PluginTwoFlagParams.Param2 = ResType;
	PluginTwoFlagParams.Flag1 = (int)HInstance;
	PluginLink.CallService(AQQ_FUNCTION_SAVERESOURCE, (WPARAM)&PluginTwoFlagParams, (LPARAM)FileName);
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

//Odczyt ustawien
void LoadSettings()
{
	//Otwarcie pliku ustawien
	TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\FixUpdater\\\\Settings.ini");
	//Nadpisanie domyslnych adresow repozytoriow
	Ini->WriteString("Links", "Url1", "http://beherit.pl/aqq_update/plugins.xml");
	Ini->WriteString("Links", "Url2", "http://beherit.pl/aqq_update/themes.xml");
	//Odczyt repozytoriow
	TStringList *Links = new TStringList;
	Ini->ReadSection("Links", Links);
	int LinksCount = Links->Count/2;
	delete Links;
	for(int Count=0; Count<LinksCount; Count++)
	{
		//Repozytorium wlaczone
		if(Ini->ReadBool("Links", "Enable" + IntToStr(Count+1), true))
			//Dodanie adresu repozytoria do aktualizatora
			SetUpdateLink(Ini->ReadString("Links", "Url" + IntToStr(Count+1), ""), true);
	}
	//Sposob aktualizacji
	UpdateMode = Ini->ReadInteger("Settings", "UpdateMode", 0);
	//Czestotliwosci aktualizacji
	UpdateInterval = Ini->ReadInteger("Settings", "UpdateTime", 0);
	//Zamkniecie pliku ustawien
	delete Ini;
}
//---------------------------------------------------------------------------

extern "C" INT_PTR __declspec(dllexport) __stdcall Load(PPluginLink Link)
{
	//Linkowanie wtyczki z komunikatorem
	PluginLink = *Link;
	//Sciezka folderu prywatnego wtyczek
	UnicodeString PluginUserDir = GetPluginUserDir();
  //Tworzenie katalogow lokalizacji
	if(!DirectoryExists(PluginUserDir + "\\\\Languages"))
		CreateDir(PluginUserDir + "\\\\Languages");
	if(!DirectoryExists(PluginUserDir + "\\\\Languages\\\\FixUpdater"))
		CreateDir(PluginUserDir + "\\\\Languages\\\\FixUpdater");
	if(!DirectoryExists(PluginUserDir + "\\\\Languages\\\\FixUpdater\\\\EN"))
		CreateDir(PluginUserDir + "\\\\Languages\\\\FixUpdater\\\\EN");
	if(!DirectoryExists(PluginUserDir + "\\\\Languages\\\\FixUpdater\\\\PL"))
		CreateDir(PluginUserDir + "\\\\Languages\\\\FixUpdater\\\\PL");
  //Wypakowanie plikow lokalizacji
	//ED2ACB3CF57B23E66C1A5BBFF94D893C
	if(!FileExists(PluginUserDir + "\\\\Languages\\\\FixUpdater\\\\EN\\\\Const.lng"))
		ExtractRes((PluginUserDir + "\\\\Languages\\\\FixUpdater\\\\EN\\\\Const.lng").w_str(), L"EN_CONST", L"DATA");
	else if(MD5File(PluginUserDir + "\\\\Languages\\\\FixUpdater\\\\EN\\\\Const.lng")!="ED2ACB3CF57B23E66C1A5BBFF94D893C")
		ExtractRes((PluginUserDir + "\\\\Languages\\\\FixUpdater\\\\EN\\\\Const.lng").w_str(), L"EN_CONST", L"DATA");
	//5E59ADA9E88CB1A66E9FD19EB5DA8281
	if(!FileExists(PluginUserDir + "\\\\Languages\\\\FixUpdater\\\\EN\\\\TSettingsForm.lng"))
		ExtractRes((PluginUserDir + "\\\\Languages\\\\FixUpdater\\\\EN\\\\TSettingsForm.lng").w_str(), L"EN_SETTINGSFRM", L"DATA");
	else if(MD5File(PluginUserDir + "\\\\Languages\\\\FixUpdater\\\\EN\\\\TSettingsForm.lng")!="5E59ADA9E88CB1A66E9FD19EB5DA8281")
		ExtractRes((PluginUserDir + "\\\\Languages\\\\FixUpdater\\\\EN\\\\TSettingsForm.lng").w_str(), L"EN_SETTINGSFRM", L"DATA");
	//FB0DD8A32BA25D66A3E6109B8C3739B3
	if(!FileExists(PluginUserDir + "\\\\Languages\\\\FixUpdater\\\\PL\\\\Const.lng"))
		ExtractRes((PluginUserDir + "\\\\Languages\\\\FixUpdater\\\\PL\\\\Const.lng").w_str(), L"PL_CONST", L"DATA");
	else if(MD5File(PluginUserDir + "\\\\Languages\\\\FixUpdater\\\\PL\\\\Const.lng")!="FB0DD8A32BA25D66A3E6109B8C3739B3")
		ExtractRes((PluginUserDir + "\\\\Languages\\\\FixUpdater\\\\PL\\\\Const.lng").w_str(), L"PL_CONST", L"DATA");
	//440BFE2CAC5623E04EDBE7BFA429D146
	if(!FileExists(PluginUserDir + "\\\\Languages\\\\FixUpdater\\\\PL\\\\TSettingsForm.lng"))
		ExtractRes((PluginUserDir + "\\\\Languages\\\\FixUpdater\\\\PL\\\\TSettingsForm.lng").w_str(), L"PL_SETTINGSFRM", L"DATA");
	else if(MD5File(PluginUserDir + "\\\\Languages\\\\FixUpdater\\\\PL\\\\TSettingsForm.lng")!="440BFE2CAC5623E04EDBE7BFA429D146")
		ExtractRes((PluginUserDir + "\\\\Languages\\\\FixUpdater\\\\PL\\\\TSettingsForm.lng").w_str(), L"PL_SETTINGSFRM", L"DATA");
  //Ustawienie sciezki lokalizacji wtyczki
	UnicodeString LangCode = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETLANGCODE, 0,0);
	LangPath = PluginUserDir + "\\\\Languages\\\\FixUpdater\\\\" + LangCode + "\\\\";
	if(!DirectoryExists(LangPath))
	{
		LangCode = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETDEFLANGCODE, 0, 0);
		LangPath = PluginUserDir + "\\\\Languages\\\\FixUpdater\\\\" + LangCode + "\\\\";
	}
	//Wypakiwanie ikonki FixUpdater.dll.png
	//3EA122B23FBF8835FDE23DCD1CC9968B
	if(!DirectoryExists(PluginUserDir + "\\\\Shared"))
		CreateDir(PluginUserDir + "\\\\Shared");
	if(!FileExists(PluginUserDir + "\\\\Shared\\\\FixUpdater.dll.png"))
		ExtractRes((PluginUserDir + "\\\\Shared\\\\FixUpdater.dll.png").w_str(), L"SHARED", L"DATA");
	else if(MD5File(PluginUserDir + "\\\\Shared\\\\FixUpdater.dll.png")!="3EA122B23FBF8835FDE23DCD1CC9968B")
		ExtractRes((PluginUserDir + "\\\\Shared\\\\FixUpdater.dll.png").w_str(), L"SHARED", L"DATA");
	//Tworzeniu katalogu z ustawieniami wtyczki
	if(!DirectoryExists(PluginUserDir + "\\\\Blabler"))
	 CreateDir(PluginUserDir + "\\\\Blabler");
	//Odczyt ustawien
	LoadSettings();
	//Hook na dodawanie repozytorium przez zewnetrzne wtyczki
	PluginLink.HookEvent(FIXUPDATER_SYSTEM_ADDLINK, OnAddLink);
	//Hook na instalowanie dodatkow
	PluginLink.HookEvent(AQQ_SYSTEM_ADDONINSTALLED, OnAddonInstalled);
	//Hook na wylaczenie komunikatora poprzez usera
	PluginLink.HookEvent(AQQ_SYSTEM_BEFOREUNLOAD, OnBeforeUnload);
	//Hook na zmiane kolorystyki AlphaControls
	PluginLink.HookEvent(AQQ_SYSTEM_COLORCHANGEV2, OnColorChange);
	//Hook na usuwanie repozytorium przez zewnetrzne wtyczki
	PluginLink.HookEvent(FIXUPDATER_SYSTEM_DELETELINK, OnDeleteLink);
	//Hook na zmiane lokalizacji
	PluginLink.HookEvent(AQQ_SYSTEM_LANGCODE_CHANGED,OnLangCodeChanged);
	//Hook na zaladowanie wszystkich modolow
	PluginLink.HookEvent(AQQ_SYSTEM_MODULESLOADED, OnModulesLoaded);
	//Hook na zmiane kompozycji
	PluginLink.HookEvent(AQQ_SYSTEM_THEMECHANGED, OnThemeChanged);
	//Rejestowanie klasy okna timera
	WNDCLASSEX wincl;
	wincl.cbSize = sizeof (WNDCLASSEX);
	wincl.style = 0;
	wincl.lpfnWndProc = TimerFrmProc;
	wincl.cbClsExtra = 0;
	wincl.cbWndExtra = 0;
	wincl.hInstance = HInstance;
	wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wincl.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
	wincl.lpszMenuName = NULL;
	wincl.lpszClassName = L"TFixUpdaterTimer";
	wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&wincl);
	//Tworzenie okna timera
	hTimerFrm = CreateWindowEx(0, L"TFixUpdaterTimer", L"",	0, 0, 0, 0, 0, NULL, NULL, HInstance, NULL);
	//Wszystkie moduly zostaly zaladowane
	if(PluginLink.CallService(AQQ_SYSTEM_MODULESLOADED,0,0))
		//Wlaczenie timera sprawdzania aktualizacji
		SetTimer(hTimerFrm,TIMER_CHKUPDATES, 300000, (TIMERPROC)TimerFrmProc);

	return 0;
}
//---------------------------------------------------------------------------

extern "C" INT_PTR __declspec(dllexport)__stdcall Settings()
{
	//Przypisanie uchwytu do formy ustawien
	if(!hSettingsForm)
	{
		Application->Handle = (HWND)SettingsForm;
		hSettingsForm = new TSettingsForm(Application);
	}
	//Pokaznie okna ustawien
	hSettingsForm->Show();

	return 0;
}
//---------------------------------------------------------------------------

extern "C" INT_PTR __declspec(dllexport) __stdcall Unload()
{
	//Zatrzymanie timera
	KillTimer(hTimerFrm, TIMER_CHKUPDATES);
	//Usuwanie okna timera
	DestroyWindow(hTimerFrm);
	//Wyrejestowanie klasy okna timera
	UnregisterClass(L"TFixUpdaterTimer", HInstance);
	//Wyladowanie wszystkich hookow
	PluginLink.UnhookEvent(OnAddLink);
	PluginLink.UnhookEvent(OnAddonInstalled);
	PluginLink.UnhookEvent(OnBeforeUnload);
	PluginLink.UnhookEvent(OnColorChange);
	PluginLink.UnhookEvent(OnDeleteLink);
	PluginLink.UnhookEvent(OnLangCodeChanged);
	PluginLink.UnhookEvent(OnModulesLoaded);
	PluginLink.UnhookEvent(OnThemeChanged);
	//Usuwanie adresow repozytoriow z aktualizatora
	if(!ForceUnloadExecuted)
	{
		//Odczyt repozytoriow
		TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\FixUpdater\\\\Settings.ini");
		TStringList *Links = new TStringList;
		Ini->ReadSection("Links", Links);
		int LinksCount = Links->Count/2;
		delete Links;
		for(int Count=0; Count<LinksCount; Count++)
		{
			//Repozytorium wlaczone
			if(Ini->ReadBool("Links", "Enable" + IntToStr(Count+1), true))
				//Usuniecie adresu repozytoria z aktualizatora
				SetUpdateLink(Ini->ReadString("Links", "Url" + IntToStr(Count+1), ""), false);
		}
		delete Ini;
	}

	return 0;
}
//---------------------------------------------------------------------------

//Informacje o wtyczce
extern "C" __declspec(dllexport) PPluginInfo __stdcall AQQPluginInfo(DWORD AQQVersion)
{
	PluginInfo.cbSize = sizeof(TPluginInfo);
	PluginInfo.ShortName = L"FixUpdater";
	PluginInfo.Version = PLUGIN_MAKE_VERSION(1,5,0,0);
	PluginInfo.Description = L"Ulepsza system aktualizacji poprzez mo¿liwoœæ dodawania dodatkowych adresów serwerów zawieraj¹cych bazê dodatków oraz ustawienie czêstszego interwa³u sprawdzania aktualizacji.";
	PluginInfo.Author = L"Krzysztof Grochocki";
	PluginInfo.AuthorMail = L"kontakt@beherit.pl";
	PluginInfo.Copyright = L"Krzysztof Grochocki";
	PluginInfo.Homepage = L"http://beherit.pl";
	PluginInfo.Flag = 0;
	PluginInfo.ReplaceDefaultModule = 0;

	return &PluginInfo;
}
//---------------------------------------------------------------------------
