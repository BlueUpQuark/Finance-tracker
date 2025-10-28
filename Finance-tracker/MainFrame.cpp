#include "MainFrame.h"
#include "Theme.h"
#include <wx/datetime.h>
#include <wx/statline.h>
#include <wx/dataview.h>
#include <vector>
#include <cmath>
#include <wx/sizer.h>
#include <wx/choice.h>
#include <wx/radiobox.h>
#include <algorithm>

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
EVT_BUTTON(ID_Spending_PrevMonth, MainFrame::OnSpendingPrevMonth)
EVT_BUTTON(ID_Spending_NextMonth, MainFrame::OnSpendingNextMonth)
EVT_BUTTON(ID_Spending_AddTxn, MainFrame::OnSpendingAddTxn)
EVT_BUTTON(ID_Transactions_PrevMonth, MainFrame::OnTransactionsPrevMonth)
EVT_BUTTON(ID_Transactions_NextMonth, MainFrame::OnTransactionsNextMonth)
EVT_BUTTON(ID_Transactions_Sort, MainFrame::OnTransactionsSort)
EVT_BUTTON(ID_Transactions_Sort, MainFrame::OnTransactionsSort)
EVT_BUTTON(ID_Transactions_PrevMonth, MainFrame::OnTransactionsPrevMonth)
EVT_BUTTON(ID_Transactions_NextMonth, MainFrame::OnTransactionsNextMonth)

wxEND_EVENT_TABLE()

struct DemoTxn { wxDateTime date; wxString category; wxString desc; double amount; };
static std::vector<DemoTxn> g_demoTxns;

wxArrayString MainFrame::BuildTxnCategoryChoices() const {
	wxSortedArrayString uniq;
	wxArrayString out;
	out.Add("All");
	for (const auto& t : g_demoTxns) uniq.Add(t.category);
	for (unsigned i = 0; i < uniq.size(); ++i) out.Add(uniq[i]);
	return out;
}

std::vector<DemoTxn> MainFrame::GetFilteredTxnsForCurrentMonth() const {
	wxDateTime base = wxDateTime::Now();
	base.SetDay(1);
	base.Add(wxDateSpan::Months(m_txnMonthOffset));
	wxDateTime start = base;
	wxDateTime end = base; end.Add(wxDateSpan::Months(1));

	std::vector<DemoTxn> out;
	out.reserve(g_demoTxns.size());
	for (const auto& t : g_demoTxns) {
		if (!(t.date.IsBetween(start, end) || t.date.IsSameDate(start))) continue;
		if (m_txnCategoryFilter != "All" && t.category != m_txnCategoryFilter) continue;
		out.push_back(t);
	}
	return out;
}

static void SeedDemoTransactionsOnce() {
	if (!g_demoTxns.empty()) return;
	wxDateTime now = wxDateTime::Now();
	int y = now.GetYear();
	int m = now.GetMonth();

	auto mk = [&](int year, int month, int day, const char* cat, const char* desc, double amt) {
		wxDateTime d(day, static_cast<wxDateTime::Month>(month), year);
		g_demoTxns.push_back({ d, wxString::FromUTF8(cat), wxString::FromUTF8(desc), amt });
		};

	// Current month (income positive, expenses negative)
	mk(y, m, 1, "Paycheck", "UPS paycheck", 1600.00);
	mk(y, m, 15, "Paycheck", "Pizza Hut pay", 800.00);
	mk(y, m, 5, "Rent", "Monthly rent", -700.00);
	mk(y, m, 8, "Groceries", "Walmart", -220.45);
	mk(y, m, 12, "Gas", "Shell", -60.00);
	mk(y, m, 20, "Fun", "Anime bar", -55.20);

	// Previous month
	int pm = (m == 0 ? 11 : m - 1), py = (m == 0 ? y - 1 : y);
	mk(py, pm, 1, "Paycheck", "UPS paycheck", 1600.00);
	mk(py, pm, 5, "Rent", "Rent", -700.00);
	mk(py, pm, 10, "Groceries", "Costco", -180.00);

	// Next month
	int nm = (m == 11 ? 0 : m + 1), ny = (m == 11 ? y + 1 : y);
	mk(ny, nm, 1, "Paycheck", "UPS paycheck", 1600.00);
}

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

	// ----- Sub-top bar: [<]  Month  [>]  .....................  [Add Transaction] [Settings]
	auto* bar = new wxPanel(page);
	bar->SetBackgroundColour(Theme::SubBarBg());
	auto* h = new wxBoxSizer(wxHORIZONTAL);

	auto* btnPrev = new wxButton(bar, ID_Spending_PrevMonth, "<");
	m_lblSpendingMonth = new wxStaticText(bar, wxID_ANY, MonthLabelFromOffset(0));
	auto* btnNext = new wxButton(bar, ID_Spending_NextMonth, ">");

	auto* btnAddTxn = new wxButton(bar, ID_Spending_AddTxn, "Add Transaction");
	auto* btnSettings = new wxButton(bar, ID_Spending_Settings, "Settings");

	h->AddSpacer(8);
	h->Add(btnPrev, 0, wxALL | wxALIGN_CENTER_VERTICAL, 6);
	h->Add(m_lblSpendingMonth, 0, wxALL | wxALIGN_CENTER_VERTICAL, 8);
	h->Add(btnNext, 0, wxALL | wxALIGN_CENTER_VERTICAL, 6);
	h->AddStretchSpacer();
	h->Add(btnAddTxn, 0, wxALL | wxALIGN_CENTER_VERTICAL, 6);
	h->Add(btnSettings, 0, wxALL | wxALIGN_CENTER_VERTICAL, 6);
	h->AddSpacer(8);

	bar->SetSizer(h);
	v->Add(bar, 0, wxEXPAND);

	v->AddSpacer(6);

	// ----- Summary row: Income (green)   Expenses (red)
	auto* summary = new wxPanel(page);
	summary->SetBackgroundColour(Theme::AppBg());
	auto* hs = new wxBoxSizer(wxHORIZONTAL);

	auto* incomeBox = new wxPanel(summary);
	auto* incomeSz = new wxBoxSizer(wxVERTICAL);
	auto* lblIncome = new wxStaticText(incomeBox, wxID_ANY, "Income");
	m_lblIncomeVal = new wxStaticText(incomeBox, wxID_ANY, "$0.00");
	m_lblIncomeVal->SetForegroundColour(wxColour(0, 128, 0)); // green
	m_lblIncomeVal->SetFont(m_lblIncomeVal->GetFont().Bold());
	incomeSz->Add(lblIncome, 0, wxBOTTOM, 4);
	incomeSz->Add(m_lblIncomeVal, 0);
	incomeBox->SetSizer(incomeSz);

	auto* expenseBox = new wxPanel(summary);
	auto* expenseSz = new wxBoxSizer(wxVERTICAL);
	auto* lblExpense = new wxStaticText(expenseBox, wxID_ANY, "Expenses");
	m_lblExpenseVal = new wxStaticText(expenseBox, wxID_ANY, "$0.00");
	m_lblExpenseVal->SetForegroundColour(wxColour(178, 34, 34)); // firebrick/red
	m_lblExpenseVal->SetFont(m_lblExpenseVal->GetFont().Bold());
	expenseSz->Add(lblExpense, 0, wxBOTTOM, 4);
	expenseSz->Add(m_lblExpenseVal, 0);
	expenseBox->SetSizer(expenseSz);

	hs->AddSpacer(10);
	hs->Add(incomeBox, 0, wxALL, 8);
	hs->AddSpacer(30);
	hs->Add(expenseBox, 0, wxALL, 8);
	hs->AddStretchSpacer();
	hs->AddSpacer(10);

	summary->SetSizer(hs);
	v->Add(summary, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);

	v->AddSpacer(6);

	// ----- Categories list (Category | Amount)
	auto* listPanel = new wxPanel(page);
	listPanel->SetBackgroundColour(Theme::PanelBg());
	auto* listSz = new wxBoxSizer(wxVERTICAL);

	m_catList = new wxDataViewListCtrl(listPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxDV_ROW_LINES | wxDV_VERT_RULES | wxDV_HORIZ_RULES);

	m_catList->AppendTextColumn("Category", wxDATAVIEW_CELL_INERT, 250, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	m_catList->AppendTextColumn("Amount", wxDATAVIEW_CELL_INERT, 120, wxALIGN_RIGHT, wxDATAVIEW_COL_RESIZABLE);

	listSz->Add(m_catList, 1, wxEXPAND | wxALL, 10);
	listPanel->SetSizer(listSz);

	v->Add(listPanel, 1, wxEXPAND | wxLEFT | wxRIGHT, 10);

	v->AddSpacer(6);

	// ----- Balance row
	auto* balance = new wxPanel(page);
	balance->SetBackgroundColour(Theme::AppBg());
	auto* hb = new wxBoxSizer(wxHORIZONTAL);
	auto* lblBal = new wxStaticText(balance, wxID_ANY, "Balance");
	m_lblBalanceVal = new wxStaticText(balance, wxID_ANY, "$0.00");
	m_lblBalanceVal->SetFont(m_lblBalanceVal->GetFont().Bold());
	hb->AddSpacer(10);
	hb->Add(lblBal, 0, wxALL | wxALIGN_CENTER_VERTICAL, 6);
	hb->AddStretchSpacer();
	hb->Add(m_lblBalanceVal, 0, wxALL | wxALIGN_CENTER_VERTICAL, 6);
	hb->AddSpacer(10);
	balance->SetSizer(hb);

	v->Add(balance, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

	page->SetSizer(v);

	// Fill with initial (placeholder) data for current month
	RefreshSpendingView();
	return page;
}

void MainFrame::RefreshTransactionsView() {
	if (m_lblTxnMonth)
		m_lblTxnMonth->SetLabel(MonthLabelFromOffset(m_txnMonthOffset));

	// Filter by month (+ category if set)
	auto items = GetFilteredTxnsForCurrentMonth();

	// Sort according to current settings
	auto cmp = [&](const DemoTxn& a, const DemoTxn& b) {
		switch (m_txnSortField) {
		case TxnSortField::Date:
			return m_txnSortAsc ? (a.date < b.date) : (b.date < a.date);
		case TxnSortField::Amount:
			return m_txnSortAsc ? (a.amount < b.amount) : (b.amount < a.amount);
		case TxnSortField::Category:
			return m_txnSortAsc ? (a.category.CmpNoCase(b.category) < 0)
				: (b.category.CmpNoCase(a.category) < 0);
		case TxnSortField::Description:
			return m_txnSortAsc ? (a.desc.CmpNoCase(b.desc) < 0)
				: (b.desc.CmpNoCase(a.desc) < 0);
		}
		return false;
		};
	std::sort(items.begin(), items.end(), cmp);

	// Rebuild list + totals
	double inc = 0.0, exp = 0.0;
	if (m_txnList) m_txnList->DeleteAllItems();

	for (const auto& t : items) {
		if (t.amount >= 0) inc += t.amount; else exp += -t.amount;

		if (m_txnList) {
			wxVector<wxVariant> row;
			row.push_back(wxVariant(t.date.Format("%Y-%m-%d")));
			row.push_back(wxVariant(t.category));
			row.push_back(wxVariant(t.desc));
			row.push_back(wxVariant(wxString::Format("%s%.2f",
				t.amount >= 0 ? "+$" : "-$", std::fabs(t.amount))));
			m_txnList->AppendItem(row);
		}
	}

	if (m_lblTxnIncome)  m_lblTxnIncome->SetLabel(wxString::Format("$%.2f", inc));
	if (m_lblTxnExpense) m_lblTxnExpense->SetLabel(wxString::Format("$%.2f", exp));
}

wxPanel* MainFrame::BuildTransactionsPage(wxWindow* parent) {
	SeedDemoTransactionsOnce();

	auto* page = new wxPanel(parent);
	page->SetBackgroundColour(Theme::AppBg());

	auto* v = new wxBoxSizer(wxVERTICAL);

	// Sub-top bar: [<] Month [>] | Income/Expenses totals | (+) [Sort]
	auto* bar = new wxPanel(page);
	bar->SetBackgroundColour(Theme::SubBarBg());
	auto* h = new wxBoxSizer(wxHORIZONTAL);

	auto* btnPrev = new wxButton(bar, ID_Transactions_PrevMonth, "<");
	m_lblTxnMonth = new wxStaticText(bar, wxID_ANY, MonthLabelFromOffset(0));
	auto* btnNext = new wxButton(bar, ID_Transactions_NextMonth, ">");

	auto* totals = new wxPanel(bar);
	auto* th = new wxBoxSizer(wxHORIZONTAL);
	auto* lblInc = new wxStaticText(totals, wxID_ANY, "Income:");
	m_lblTxnIncome = new wxStaticText(totals, wxID_ANY, "$0.00");
	m_lblTxnIncome->SetForegroundColour(wxColour(0, 128, 0));
	m_lblTxnIncome->SetFont(m_lblTxnIncome->GetFont().Bold());
	auto* lblExp = new wxStaticText(totals, wxID_ANY, "   Expenses:");
	m_lblTxnExpense = new wxStaticText(totals, wxID_ANY, "$0.00");
	m_lblTxnExpense->SetForegroundColour(wxColour(178, 34, 34));
	m_lblTxnExpense->SetFont(m_lblTxnExpense->GetFont().Bold());
	th->Add(lblInc, 0, wxALIGN_CENTER_VERTICAL);
	th->Add(m_lblTxnIncome, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 6);
	th->Add(lblExp, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 12);
	th->Add(m_lblTxnExpense, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 6);
	totals->SetSizer(th);

	auto* btnNew = new wxButton(bar, ID_Transactions_New, "+");
	auto* btnSort = new wxButton(bar, ID_Transactions_Sort, "Sort");

	h->AddSpacer(8);
	h->Add(btnPrev, 0, wxALL | wxALIGN_CENTER_VERTICAL, 6);
	h->Add(m_lblTxnMonth, 0, wxALL | wxALIGN_CENTER_VERTICAL, 8);
	h->Add(btnNext, 0, wxALL | wxALIGN_CENTER_VERTICAL, 6);
	h->AddStretchSpacer();
	h->Add(totals, 0, wxALL | wxALIGN_CENTER_VERTICAL, 4);
	h->AddStretchSpacer();
	h->Add(btnNew, 0, wxALL | wxALIGN_CENTER_VERTICAL, 6);
	h->Add(btnSort, 0, wxALL | wxALIGN_CENTER_VERTICAL, 6);
	h->AddSpacer(8);

	bar->SetSizer(h);
	v->Add(bar, 0, wxEXPAND);

	// Transactions list
	auto* listPanel = new wxPanel(page);
	listPanel->SetBackgroundColour(Theme::PanelBg());
	auto* listSz = new wxBoxSizer(wxVERTICAL);

	m_txnList = new wxDataViewListCtrl(listPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxDV_ROW_LINES | wxDV_VERT_RULES | wxDV_HORIZ_RULES);
	m_txnList->AppendTextColumn("Date", wxDATAVIEW_CELL_INERT, 100, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	m_txnList->AppendTextColumn("Category", wxDATAVIEW_CELL_INERT, 150, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	m_txnList->AppendTextColumn("Description", wxDATAVIEW_CELL_INERT, 300, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	m_txnList->AppendTextColumn("Amount", wxDATAVIEW_CELL_INERT, 100, wxALIGN_RIGHT, wxDATAVIEW_COL_RESIZABLE);

	listSz->Add(m_txnList, 1, wxEXPAND | wxALL, 10);
	listPanel->SetSizer(listSz);

	// explicit cast so the compiler picks the wxWindow* overload
	v->Add(static_cast<wxWindow*>(listPanel), 1,
		wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

	page->SetSizer(v);

	RefreshTransactionsView();
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

wxString MainFrame::MonthLabelFromOffset(int offset) const {
	wxDateTime now = wxDateTime::Now();
	now.SetDay(1);                 // month label is the month itself
	now.Add(wxDateSpan::Months(offset));
	return now.Format("%B %Y");    // e.g., "October 2025"
}

void MainFrame::RefreshSpendingView() {
	// 1) Update month label
	if (m_lblSpendingMonth) m_lblSpendingMonth->SetLabel(MonthLabelFromOffset(m_spendMonthOffset));

	// 2) Will Pull real data for (income, expenses, categories) for the selected month.
	// For now, placeholder numbers & categories so the UI shows correctly:
	double income = 3200.00;
	double expense = 1847.52;

	if (m_lblIncomeVal)  m_lblIncomeVal->SetLabel(wxString::Format("$%.2f", income));
	if (m_lblExpenseVal) m_lblExpenseVal->SetLabel(wxString::Format("$%.2f", expense));

	double balance = income - expense;
	if (m_lblBalanceVal) {
		m_lblBalanceVal->SetLabel(wxString::Format("$%.2f", balance));
		m_lblBalanceVal->SetForegroundColour(balance >= 0 ? wxColour(0, 128, 0) : wxColour(178, 34, 34));
	}

	// 3) Categories list (clear + repopulate)
	if (m_catList) {
		m_catList->DeleteAllItems();

		// placeholder rows
		struct Row { const char* name; double amt; } rows[] = {
			{"Rent",        700.00},
			{"Groceries",   425.31},
			{"Gas",         230.00},
			{"Phone",        97.08},
			{"Insurance",   240.00},
			{"Entertainment",155.13},
		};
		for (const auto& r : rows) {
			wxVector<wxVariant> cols;
			cols.push_back(wxVariant(wxString::FromUTF8(r.name)));
			cols.push_back(wxVariant(wxString::Format("$%.2f", r.amt)));
			m_catList->AppendItem(cols);
		}
	}
}

void MainFrame::OnSpendingPrevMonth(wxCommandEvent&) {
	m_spendMonthOffset -= 1;
	RefreshSpendingView();
}

void MainFrame::OnSpendingNextMonth(wxCommandEvent&) {
	m_spendMonthOffset += 1;
	RefreshSpendingView();
}

void MainFrame::OnTransactionsPrevMonth(wxCommandEvent& evt) {
	m_txnMonthOffset -= 1;
	RefreshTransactionsView();
}

void MainFrame::OnTransactionsNextMonth(wxCommandEvent& evt) {
	m_txnMonthOffset += 1;
	RefreshTransactionsView();
}

void MainFrame::OnSpendingAddTxn(wxCommandEvent&) {
	// Will work on later
	wxMessageBox("Open 'Add Transaction' dialog (to be implemented).", "Add Transaction");
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
	wxDialog dlg(this, wxID_ANY, "Sort / Filter", wxDefaultPosition, wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	auto* v = new wxBoxSizer(wxVERTICAL);

	// Sort field
	wxArrayString fields; fields.Add("Date"); fields.Add("Amount"); fields.Add("Category"); fields.Add("Description");
	auto* rbField = new wxRadioBox(&dlg, wxID_ANY, "Sort by", wxDefaultPosition, wxDefaultSize,
		fields, 1, wxRA_SPECIFY_ROWS);
	rbField->SetSelection(static_cast<int>(m_txnSortField));

	// Order
	wxArrayString orders; orders.Add("Ascending"); orders.Add("Descending");
	auto* rbOrder = new wxRadioBox(&dlg, wxID_ANY, "Order", wxDefaultPosition, wxDefaultSize,
		orders, 1, wxRA_SPECIFY_ROWS);
	rbOrder->SetSelection(m_txnSortAsc ? 0 : 1);

	// Category filter (includes income categories)
	wxArrayString cats = BuildTxnCategoryChoices();
	auto* chCat = new wxChoice(&dlg, wxID_ANY, wxDefaultPosition, wxDefaultSize, cats);
	int sel = cats.Index(m_txnCategoryFilter);
	chCat->SetSelection(sel == wxNOT_FOUND ? 0 : sel);

	v->Add(rbField, 0, wxALL | wxEXPAND, 8);
	v->Add(rbOrder, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, 8);
	v->Add(new wxStaticText(&dlg, wxID_ANY, "Category filter"), 0, wxLEFT | wxRIGHT, 8);
	v->Add(chCat, 0, wxALL | wxEXPAND, 8);
	v->Add(dlg.CreateSeparatedButtonSizer(wxOK | wxCANCEL), 0, wxALL | wxALIGN_RIGHT, 8);

	dlg.SetSizerAndFit(v);

	if (dlg.ShowModal() == wxID_OK) {
		m_txnSortField = static_cast<TxnSortField>(rbField->GetSelection());
		m_txnSortAsc = (rbOrder->GetSelection() == 0);
		m_txnCategoryFilter = chCat->GetStringSelection();
		RefreshTransactionsView();
	}
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