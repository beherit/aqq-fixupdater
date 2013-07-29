//---------------------------------------------------------------------------
#include <vcl.h>
#include <windows.h>
#pragma hdrstop
#pragma argsused
#include "Aqq.h"
#include "MainFrm.h"
#include <inifiles.hpp>
//---------------------------------------------------------------------------

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
  return 1;
}
//---------------------------------------------------------------------------

//tworzenie uchwytu do formy
TMainForm *handle;

//utworzenie obiektow do struktur
TPluginLink PluginLink;
TPluginInfo PluginInfo;

//Zmienne
UnicodeString Dir;
int UrlCount;
UnicodeString Url;
bool Enabled;
int Count;

extern "C"  __declspec(dllexport) PPluginInfo __stdcall AQQPluginInfo(DWORD AQQVersion)
{
  //Sprawdzanie wersji AQQ
  if (CompareVersion(AQQVersion,PLUGIN_MAKE_VERSION(2,1,0,27))<0)
  {
	MessageBox(Application->Handle,
	  "Wymagana wesja AQQ przez wtyczkê to minimum 2.1.0.27!\n"
	  "Wtyczka FixUpdater nie bêdzie dzia³aæ poprawnie!",
	  "Nieprawid³owa wersja AQQ",
	  MB_OK | MB_ICONEXCLAMATION);
  }
  PluginInfo.cbSize = sizeof(TPluginInfo);
  PluginInfo.ShortName = (wchar_t*)L"FixUpdater";
  PluginInfo.Version = PLUGIN_MAKE_VERSION(1,0,0,0);
  PluginInfo.Description = (wchar_t*)L"Dodawanie w³asnych serwerów aktualizacji dodatków";
  PluginInfo.Author = (wchar_t*)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.AuthorMail = (wchar_t*)L"sirbeherit@gmial.com";
  PluginInfo.Copyright = (wchar_t*)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.Homepage = (wchar_t*)L"http://beherit.pl/";

  return &PluginInfo;
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
  delete Ini;

  return 0;
}
//---------------------------------------------------------------------------

extern "C" int __declspec(dllexport)__stdcall Settings()
{
  if(handle==NULL)
  {
	Application->Handle = MainForm;
	handle = new TMainForm(Application);
  }
  handle->Show();

  return 0;
}
//---------------------------------------------------------------------------

extern "C" int __declspec(dllexport) __stdcall Unload()
{
  Dir = GetPluginUserDir();
  TIniFile *Ini = new TIniFile(Dir + "\\\\FixUpdater\\\\Url.ini");
  UrlCount = Ini->ReadInteger("UrlCount", "Count", 2);
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

  return 0;
}
//---------------------------------------------------------------------------

void SetUpdateLink(bool Enabled, UnicodeString URL)
{
  PluginLink.CallService(AQQ_SYSTEM_SETUPDATELINK,Enabled,(LPARAM)URL.w_str());
}
//---------------------------------------------------------------------------
