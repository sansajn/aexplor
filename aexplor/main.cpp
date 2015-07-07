#include <QApplication>
#include "main_window.hpp"

/*
TODO:
* po vstupu do adresar nastav current na ..
* permissions, size, date
* skoc na novo vytvoreny adresar
* su mode
* favorites
* view file <f3>
* edit file <f4>
* kopirovanie do pc (drop event dostane cielova aplykacia a ta nevie ako ziskat subory cez adb)
* d&d copy, move context menu
* local-search dialog
* local list filter
* file operations in thread
*/
int main(int argc, char * argv[])
{
	QApplication app(argc, argv);

	main_window w;
	w.setWindowTitle("Android explorer");
	w.show();

	return app.exec();
}
