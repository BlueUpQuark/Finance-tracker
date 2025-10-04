#include "MainFrame.h"
#include "Theme.h"
#include <wx/datetime.h>
#include <wx/statline.h>


wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_BUTTON(ID_Nav_Spending, MainFrame::OnNavSpending)
EVT_BUTTON(ID_Nav_Transactions, MainFrame::OnNavTransactions)
EVT_BUTTON(ID_Nav_Categories, MainFrame::OnNavCategories)
EVT_BUTTON(ID_Nav_Accounts, MainFrame::OnNavAccounts)


EVT_BUTTON(ID_Spending_Settings, MainFrame::OnSpendingSettings)


EVT_BUTTON(ID_Transactions_New, MainFrame::OnTransactionsNew)
EVT_BUTTON(ID_Transactions_Sort, MainFrame::OnTransactionsSort)


EVT_BUTTON(ID_Categories_New, MainFrame::OnCategoriesNew)
EVT_BUTTON(ID_Categories_Sort, MainFrame::OnCategoriesSort)


EVT_BUTTON(ID_Accounts_New, MainFrame::OnAccountsNew)
wxEND_EVENT_TABLE()

MainFrame::MainFrame()
	: wxFrame(nullptr, wxID_ANY, "Finance Tracker", wxDefaultPosition, wxSize(1024, 640))
{
	SetBackgroundColour(Theme::AppBg());
	BuildUi();
	Centre();
}

void MainFrame::BuildUi() {
	auto* root = new wxPanel(this);
	root->SetBackgroundColour(Theme::AppBg());


	auto* vbox = new wxBoxSizer(wxVERTICAL);


	// Top navigation bar
	m_topBar = BuildTopBar(root);
	vbox->Add(m_topBar, 0, wxEXPAND);


	// Content book
	m_book = new wxSimplebook(root);
	m_book->SetBackgroundColour(Theme::AppBg());


	// Pages
	m_pageSpending = BuildSpendingPage(m_book);
	m_pageTransactions = BuildTransactionsPage(m_book);
	m_pageCategories = BuildCategoriesPage(m_book);
	m_pageAccounts = BuildAccountsPage(m_book);


	m_book->AddPage(m_pageSpending, wxEmptyString, true);   // default visible
	m_book->AddPage(m_pageTransactions, wxEmptyString, false);
	m_book->AddPage(m_pageCategories, wxEmptyString, false);
	m_book->AddPage(m_pageAccounts, wxEmptyString, false);


	vbox->Add(m_book, 1, wxEXPAND);


	root->SetSizer(vbox);


	// Default selection & styling
	UpdateActiveNavButton(ID_Nav_Spending);
}

wxPanel* MainFrame::BuildTopBar(wxWindow* parent) {
	auto* p = new wxPanel(parent);
	p->SetBackgroundColour(Theme::TopBarBg());


	m_btnSpend = new wxButton(p, ID_Nav_Spending, "Spending");
	m_btnTxns = new wxButton(p, ID_Nav_Transactions, "Transactions");
	m_btnCats = new wxButton(p, ID_Nav_Categories, "Categories");
	m_btnAccts = new wxButton(p, ID_Nav_Accounts, "Accounts");


	// Light styling: dark bg + white fg
	auto setBtn = [](wxButton* b) { b->SetForegroundColour(Theme::TopBarFg()); b->SetBackgroundColour(Theme::TopBarBg()); b->SetWindowStyleFlag(b->GetWindowStyleFlag() | wxBORDER_NONE); };
	setBtn(m_btnSpend); setBtn(m_btnTxns); setBtn(m_btnCats); setBtn(m_btnAccts);


	auto* h = new wxBoxSizer(wxHORIZONTAL);
	h->AddSpacer(8);
	h->Add(m_btnSpend, 0, wxALL, 6);
	h->Add(m_btnTxns, 0, wxALL, 6);
	h->Add(m_btnCats, 0, wxALL, 6);
	h->Add(m_btnAccts, 0, wxALL, 6);
	h->AddStretchSpacer();


	p->SetSizer(h);
	return p;
}

static wxPanel* MakeSubBar(wxWindow* parent) {
	auto* bar = new wxPanel(parent);
	bar->SetBackgroundColour(Theme::SubBarBg());
	auto* s = new wxBoxSizer(wxHORIZONTAL);
	s->AddSpacer(10);
	bar->SetSizer(s);
	return bar;
}

wxPanel* MainFrame::BuildSpendingPage(wxWindow* parent) {
	auto* page = new wxPanel(parent);
	page->SetBackgroundColour(Theme::AppBg());


	auto* v = new wxBoxSizer(wxVERTICAL);


	// Sub-top bar: month label + settings
	auto* bar = MakeSubBar(page);
	auto* h = static_cast<wxBoxSizer*>(bar->GetSizer());
	m_lblSpendingMonth = new wxStaticText(bar, wxID_ANY, CurrentMonthLabel());
	auto* btnSettings = new wxButton(bar, ID_Spending_Settings, "Settings");


	h->Add(m_lblSpendingMonth, 0, wxALL | wxALIGN_CENTER_VERTICAL, 8);
	h->AddStretchSpacer();
	h->Add(btnSettings, 0, wxALL | wxALIGN_CENTER_VERTICAL, 6);


	// Content placeholder
	auto* body = new wxPanel(page);
	body->SetBackgroundColour(Theme::PanelBg());
	auto* bodySizer = new wxBoxSizer(wxVERTICAL);
	bodySizer->Add(new wxStaticText(body, wxID_ANY, "Spending screen content will go here."), 0, wxALL, 12);
	body->SetSizer(bodySizer);


	v->Add(bar, 0, wxEXPAND);
	v->AddSpacer(4);
	v->Add(body, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);


	page->SetSizer(v);
	return page;
}

wxPanel* MainFrame::BuildTransactionsPage(wxWindow* parent) {
	auto* page = new wxPanel(parent);
	page->SetBackgroundColour(Theme::AppBg());


	auto* v = new wxBoxSizer(wxVERTICAL);


	// Sub-top bar: month label + +New + Sort
	auto* bar = MakeSubBar(page);
	auto* h = static_cast<wxBoxSizer*>(bar->GetSizer());


	m_lblTxnMonth = new wxStaticText(bar, wxID_ANY, CurrentMonthLabel());
	auto* btnNew = new wxButton(bar, ID_Transactions_New, "+");
	auto* btnSort = new wxButton(bar, ID_Transactions_Sort, "Sort");


	h->Add(m_lblTxnMonth, 0, wxALL | wxALIGN_CENTER_VERTICAL, 8);
	h->AddStretchSpacer();
	h->Add(btnNew, 0, wxALL | wxALIGN_CENTER_VERTICAL, 6);
	h->Add(btnSort, 0, wxALL | wxALIGN_CENTER_VERTICAL, 6);


	// Content placeholder
	auto* body = new wxPanel(page);
	body->SetBackgroundColour(Theme::PanelBg());
	auto* bodySizer = new wxBoxSizer(wxVERTICAL);
	bodySizer->Add(new wxStaticText(body, wxID_ANY, "Transactions screen content will go here."), 0, wxALL, 12);
	body->SetSizer(bodySizer);


	v->Add(bar, 0, wxEXPAND);
	v->AddSpacer(4);
	v->Add(body, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);


	page->SetSizer(v);
	return page;
}

wxPanel* MainFrame::BuildCategoriesPage(wxWindow* parent) {
	auto* page = new wxPanel(parent);
	page->SetBackgroundColour(Theme::AppBg());


	auto* v = new wxBoxSizer(wxVERTICAL);


	// Sub-top bar: +New Category, Sort
	auto* bar = MakeSubBar(page);
	auto* h = static_cast<wxBoxSizer*>(bar->GetSizer());


	auto* lbl = new wxStaticText(bar, wxID_ANY, "Categories");
	auto* btnNew = new wxButton(bar, ID_Categories_New, "+");
	auto* btnSort = new wxButton(bar, ID_Categories_Sort, "Sort");


	h->Add(lbl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 8);
	h->AddStretchSpacer();
	h->Add(btnNew, 0, wxALL | wxALIGN_CENTER_VERTICAL, 6);
	h->Add(btnSort, 0, wxALL | wxALIGN_CENTER_VERTICAL, 6);


	// Content placeholder
	auto* body = new wxPanel(page);
	body->SetBackgroundColour(Theme::PanelBg());
	auto* bodySizer = new wxBoxSizer(wxVERTICAL);
	bodySizer->Add(new wxStaticText(body, wxID_ANY, "Categories screen content will go here."), 0, wxALL, 12);
	body->SetSizer(bodySizer);


	v->Add(bar, 0, wxEXPAND);
	v->AddSpacer(4);
	v->Add(body, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);


	page->SetSizer(v);
	return page;
}

wxPanel* MainFrame::BuildAccountsPage(wxWindow* parent) {
	auto* page = new wxPanel(parent);
	page->SetBackgroundColour(Theme::AppBg());


	auto* v = new wxBoxSizer(wxVERTICAL);


	// Sub-top bar: +New Account
	auto* bar = MakeSubBar(page);
	auto* h = static_cast<wxBoxSizer*>(bar->GetSizer());


	auto* lbl = new wxStaticText(bar, wxID_ANY, "Accounts");
	auto* btnNew = new wxButton(bar, ID_Accounts_New, "+");


	h->Add(lbl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 8);
	h->AddStretchSpacer();
	h->Add(btnNew, 0, wxALL | wxALIGN_CENTER_VERTICAL, 6);


	// Content placeholder
	auto* body = new wxPanel(page);
	body->SetBackgroundColour(Theme::PanelBg());
	auto* bodySizer = new wxBoxSizer(wxVERTICAL);
	bodySizer->Add(new wxStaticText(body, wxID_ANY, "Accounts screen content will go here."), 0, wxALL, 12);
	body->SetSizer(bodySizer);


	v->Add(bar, 0, wxEXPAND);
	v->AddSpacer(4);
	v->Add(body, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);


	page->SetSizer(v);
	return page;
}

void MainFrame::UpdateActiveNavButton(int whichId) {
	auto setActive = [](wxButton* b, bool on) {
		if (!b) return;

		// color
		b->SetForegroundColour(on ? *wxYELLOW : Theme::TopBarFg());

		// font weight
		wxFont f = b->GetFont();
		if (on) {
			f.MakeBold();                           // highlight active
		}
		else {
			f.SetWeight(wxFONTWEIGHT_NORMAL);           // reset to normal
		}
		b->SetFont(f);
		b->Refresh();
		};

	setActive(m_btnSpend, whichId == ID_Nav_Spending);
	setActive(m_btnTxns, whichId == ID_Nav_Transactions);
	setActive(m_btnCats, whichId == ID_Nav_Categories);
	setActive(m_btnAccts, whichId == ID_Nav_Accounts);
}


wxString MainFrame::CurrentMonthLabel() const {
	wxDateTime now = wxDateTime::Now();
	return now.Format("%B %Y");
}

// ===== Nav handlers =====
void MainFrame::OnNavSpending(wxCommandEvent&) { m_book->SetSelection(0); UpdateActiveNavButton(ID_Nav_Spending); }
void MainFrame::OnNavTransactions(wxCommandEvent&) { m_book->SetSelection(1); UpdateActiveNavButton(ID_Nav_Transactions); }
void MainFrame::OnNavCategories(wxCommandEvent&) { m_book->SetSelection(2); UpdateActiveNavButton(ID_Nav_Categories); }
void MainFrame::OnNavAccounts(wxCommandEvent&) { m_book->SetSelection(3); UpdateActiveNavButton(ID_Nav_Accounts); }


// ===== Toolbar actions (placeholders for now) =====
void MainFrame::OnSpendingSettings(wxCommandEvent&) {
	wxMessageBox("Settings screen will be implemented.", "Settings");
}


void MainFrame::OnTransactionsNew(wxCommandEvent&) {
	wxMessageBox("Open 'New Transaction' dialog (to be implemented).", "+ New Transaction");
}


void MainFrame::OnTransactionsSort(wxCommandEvent&) {
	wxMessageBox("Show sort options for current month (to be implemented).", "Sort Transactions");
}


void MainFrame::OnCategoriesNew(wxCommandEvent&) {
	wxMessageBox("Open 'New Category' dialog (to be implemented).", "+ New Category");
}


void MainFrame::OnCategoriesSort(wxCommandEvent&) {
	wxMessageBox("Show category sort options (to be implemented).", "Sort Categories");
}


void MainFrame::OnAccountsNew(wxCommandEvent&) {
	wxMessageBox("Open 'New Account' dialog (to be implemented).", "+ New Account");
}