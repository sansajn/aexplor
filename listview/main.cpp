#include <QApplication>
#include "directory_widget.hpp"

/*
TODO:
* navrat po linku
* ikony (directory, executable, ...)
* permissions, size, date
* zoznam prikazou ? (mkdir:<f7>, remove:<del>, rename)
* su mode
* remount (/system)
* favorites
* kopirovanie s pc
* kopirovanie do pc (drop event dostane cielova aplykacia a ta nevie ako ziskat subory cez adb)
* view file <f3>
* edit file <f4>
* file operations in thread
* d&d copy, move context menu
* local-search dialog
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
