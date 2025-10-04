#pragma once
#include <wx/wx.h>
#include <wx/simplebook.h>
#include <wx/stattext.h>


// Navigation IDs
enum : int {
	ID_Nav_Spending = wxID_HIGHEST + 1,
	ID_Nav_Transactions,
	ID_Nav_Categories,
	ID_Nav_Accounts,


	// Per-screen toolbar actions
	ID_Spending_Settings,


	ID_Transactions_New,
	ID_Transactions_Sort,


	ID_Categories_New,
	ID_Categories_Sort,


	ID_Accounts_New,
};


class MainFrame : public wxFrame {
public:
	MainFrame();


private:
	// Top navigation bar
	wxPanel* m_topBar = nullptr;
	wxButton* m_btnSpend = nullptr;
	wxButton* m_btnTxns = nullptr;
	wxButton* m_btnCats = nullptr;
	wxButton* m_btnAccts = nullptr;


	// Main content book (one page per screen)
	wxSimplebook* m_book = nullptr;


	// Pages
	wxPanel* m_pageSpending = nullptr;
	wxPanel* m_pageTransactions = nullptr;
	wxPanel* m_pageCategories = nullptr;
	wxPanel* m_pageAccounts = nullptr;


	// Page toolbars (per screen) — labels/buttons differ per page
	wxStaticText* m_lblSpendingMonth = nullptr;


	wxStaticText* m_lblTxnMonth = nullptr;


	// Helpers
	void BuildUi();
	wxPanel* BuildTopBar(wxWindow* parent);
	wxPanel* BuildSpendingPage(wxWindow* parent);
	wxPanel* BuildTransactionsPage(wxWindow* parent);
	wxPanel* BuildCategoriesPage(wxWindow* parent);
	wxPanel* BuildAccountsPage(wxWindow* parent);


	void UpdateActiveNavButton(int whichId);
	wxString CurrentMonthLabel() const; // e.g. "October 2025"


	// Nav handlers
	void OnNavSpending(wxCommandEvent&);
	void OnNavTransactions(wxCommandEvent&);
	void OnNavCategories(wxCommandEvent&);
	void OnNavAccounts(wxCommandEvent&);


	// Toolbar handlers (placeholders for now)
	void OnSpendingSettings(wxCommandEvent&);


	void OnTransactionsNew(wxCommandEvent&);
	void OnTransactionsSort(wxCommandEvent&);


	void OnCategoriesNew(wxCommandEvent&);
	void OnCategoriesSort(wxCommandEvent&);


	void OnAccountsNew(wxCommandEvent&);


	wxDECLARE_EVENT_TABLE();
};
