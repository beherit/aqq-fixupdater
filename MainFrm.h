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
	TTabControl *TabControl;
	TButton *ResetButton;
	TButton *SaveButton;
	TLabel *Label;
	TButton *EditButton;
	TAction *aExit;
	void __fastcall UrlListPreviewSelectItem(TObject *Sender, TListItem *Item, bool Selected);
	void __fastcall DeleteButtonClick(TObject *Sender);
	void __fastcall SaveButtonClick(TObject *Sender);
	void __fastcall AddButtonClick(TObject *Sender);
	void __fastcall aReadSettingsExecute(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall aSaveSettingsExecute(TObject *Sender);
	void __fastcall aResetSettingsExecute(TObject *Sender);
	void __fastcall ResetButtonClick(TObject *Sender);
	void __fastcall UrlListPreviewChange(TObject *Sender, TListItem *Item, TItemChange Change);
	void __fastcall UrlListPreviewClick(TObject *Sender);
	void __fastcall EditButtonClick(TObject *Sender);
	void __fastcall aExitExecute(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TMainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
