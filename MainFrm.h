//---------------------------------------------------------------------------
#ifndef MainFrmH
#define MainFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ActnList.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
	TActionList *ActionList;
	TAction *aReadSettings;
	TAction *aSaveSettings;
	TAction *aResetSettings;
	TButton *AddButton;
	TButton *DeleteButton;
	TListView *UrlListPreview;
	TButton *ResetButton;
	TButton *SaveButton;
	TButton *EditButton;
	TAction *aExit;
	TPageControl *PageControl;
	TTabSheet *LinksTabSheet;
	TTabSheet *SettingsTabSheet;
	TComboBox *UpdateTimeComboBox;
	TLabel *UpdateTimeLabel;
	TLabel *LastUpdateInfoLabel;
	TLabel *LastUpdateLabel;
	TTimer *CheckUpdatesOnStartTimer;
	TTimer *CheckUpdatesTimer;
	TBevel *Bevel;
	TComboBox *UpdateModeComboBox;
	TLabel *UpdateModeLabel;
	TBevel *Bevel2;
	TAction *aSettingsChanged;
	TButton *CancelButton;
	TButton *OkButton;
	void __fastcall UrlListPreviewSelectItem(TObject *Sender, TListItem *Item, bool Selected);
	void __fastcall DeleteButtonClick(TObject *Sender);
	void __fastcall SaveButtonClick(TObject *Sender);
	void __fastcall AddButtonClick(TObject *Sender);
	void __fastcall aReadSettingsExecute(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall aSaveSettingsExecute(TObject *Sender);
	void __fastcall aResetSettingsExecute(TObject *Sender);
	void __fastcall ResetButtonClick(TObject *Sender);
	void __fastcall UrlListPreviewClick(TObject *Sender);
	void __fastcall EditButtonClick(TObject *Sender);
	void __fastcall aExitExecute(TObject *Sender);
	void __fastcall CheckUpdatesOnStartTimerTimer(TObject *Sender);
	void __fastcall CheckUpdatesTimerTimer(TObject *Sender);
	void __fastcall aSettingsChangedExecute(TObject *Sender);
	void __fastcall UrlListPreviewItemChecked(TObject *Sender, TListItem *Item);
	void __fastcall OkButtonClick(TObject *Sender);
	void __fastcall CancelButtonClick(TObject *Sender);



private:	// User declarations
public:		// User declarations
	int UpdateTime;
    int UpdateMode;
	__fastcall TMainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
