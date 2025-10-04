#pragma once
#include <wx/wx.h>


namespace Theme {
	inline wxColour AppBg() { return wxColour(245, 245, 245); } // light gray
	inline wxColour TopBarBg() { return wxColour(46, 59, 78); } // dark blue-gray
	inline wxColour TopBarFg() { return *wxWHITE; }
	inline wxColour SubBarBg() { return wxColour(236, 239, 244); } // soft light
	inline wxColour PanelBg() { return *wxWHITE; }
}