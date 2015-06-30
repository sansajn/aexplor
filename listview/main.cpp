#include <QApplication>
#include "directory_widget.hpp"

/*
TODO:
* kopirovanie do pc (drop event dostane cielova aplykacia a ta nevie ako ziskat subory cez adb)
* permissions, size, date
* skoc na novo vytvoreny adresar
* su mode
* remount (/system)
* favorites
* kopirovanie s pc
* view file <f3>
* edit file <f4>
* file operations in thread
* d&d copy, move context menu
* local-search dialog
* ok - ikony (directory, executable, ...)
* ok - zoznam prikazou ? (mkdir:<f7>, remove:<del>, rename)
* ok - files with spaces
* ok - navrat po linku
* ok - restore position on up-dir
* ok - podpora pre linky (hlavne adresarov)
* ok - sort
* ok - rename <double-click>
* ok - mkdir <f7>
* ok - delete <del>
*/
int main(int argc, char * argv[])
{
	QApplication app(argc, argv);

	directory_widget w;
	w.show();

	return app.exec();
}
