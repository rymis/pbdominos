#ifndef POCKET_BOOK_H
#define POCKET_BOOK_H

// Classes and functions for work with inkview with C++
#include <inkview.h>

namespace pocketbook {

class dialog;

//! This function is simmilar to gtk_main from Gtk+. You can call this function as many time you wants
// After you can call ink_main_quit(); When you exit from last main, you can not run main again.
void ink_main(dialog *dlg);

//! Quit from one ink_main
void ink_main_quit();

//! Class dialog is base class for your dialogs
class dialog {
	public:
		//! Full screen dialog
		dialog();

		//! Dialog with coordinates:
		dialog(int x, int y, int w, int h);

		virtual ~dialog()
		{
		}

		//!

};



};

#endif


