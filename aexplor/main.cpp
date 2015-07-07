#include <QApplication>
#include "main_window.hpp"

/*
TODO:
* permissions, size, date
* su mode
* favorites
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
