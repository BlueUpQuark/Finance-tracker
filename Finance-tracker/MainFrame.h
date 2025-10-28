#pragma once
#include <wx/wx.h>
#include <wx/simplebook.h>
#include <wx/stattext.h>
#include <wx/dataview.h>


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

	ID_Spending_PrevMonth,
	ID_Spending_NextMonth,
	ID_Spending_AddTxn,
	ID_Transactions_PrevMonth,
	ID_Transactions_NextMonth,


};

struct DemoTxn;


class MainFrame : public wxFrame {
public:
	MainFrame();


private:
	enum class TxnSortField { Date, Amount, Category, Description };

	TxnSortField m_txnSortField = TxnSortField::Date;
	bool m_txnSortAsc = true;                // ascending by default

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
	wxStaticText* m_lblIncomeVal = nullptr;
	wxStaticText* m_lblExpenseVal = nullptr;
	wxStaticText* m_lblBalanceVal = nullptr;
	wxStaticText* m_lblTxnIncome = nullptr;
	wxStaticText* m_lblTxnExpense = nullptr;
	wxDataViewListCtrl* m_txnList = nullptr;


	// Helpers
	void BuildUi();
	wxPanel* BuildTopBar(wxWindow* parent);
	wxPanel* BuildSpendingPage(wxWindow* parent);
	wxPanel* BuildTransactionsPage(wxWindow* parent);
	wxPanel* BuildCategoriesPage(wxWindow* parent);
	wxPanel* BuildAccountsPage(wxWindow* parent);
	wxString MonthLabelFromOffset(int offset) const;
	void RefreshSpendingView();  // updates month label, summary numbers, and category list

	void UpdateActiveNavButton(int whichId);
	wxString CurrentMonthLabel() const; // e.g. "October 2025"

	wxArrayString BuildTxnCategoryChoices() const;
	std::vector<DemoTxn> GetFilteredTxnsForCurrentMonth() const;

	// Nav handlers
	void OnNavSpending(wxCommandEvent&);
	void OnNavTransactions(wxCommandEvent&);
	void OnNavCategories(wxCommandEvent&);
	void OnNavAccounts(wxCommandEvent&);
	void OnSpendingPrevMonth(wxCommandEvent&);
	void OnSpendingNextMonth(wxCommandEvent&);
	void OnSpendingAddTxn(wxCommandEvent&);


	// Toolbar handlers (placeholders for now)
	void OnSpendingSettings(wxCommandEvent&);

	void RefreshTransactionsView();
	void OnTransactionsNew(wxCommandEvent&);
	void OnTransactionsSort(wxCommandEvent&);
	void OnTransactionsPrevMonth(wxCommandEvent&);
	void OnTransactionsNextMonth(wxCommandEvent&);

	void OnCategoriesNew(wxCommandEvent&);
	void OnCategoriesSort(wxCommandEvent&);


	void OnAccountsNew(wxCommandEvent&);

	int m_spendMonthOffset = 0;                 // 0 = current month, -1 = prev, +1 = next
	int m_txnMonthOffset = 0;
	
	class wxDataViewListCtrl* m_catList = nullptr;

	wxString m_txnCategoryFilter = "All";

	wxDECLARE_EVENT_TABLE();
};
