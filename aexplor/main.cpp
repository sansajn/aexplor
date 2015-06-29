#include <string>
#include <QApplication>
#include "directory_widget.hpp"

using std::string;

string const root = "/";

// TODO:
// * local-search dialog
// * rename <f2>
// * lista prikazou (<f7>, <del>, <f2>)
// * restore position on up-dir
// * su mode
// * remount (/system)
// * permissions, size, date
// * favorites
// * view file <f3>
// * edit file <f4>
// * kopirovanie do pc (drop event dostane cielova aplykacia a ta nevie ako ziskat subory cez adb)
// * d&d copy, move context menu
// * file operations in thread
// * ok - sort
// * ok - ikony (directory, executable, ...)
// * ok - mkdir <f7>
// * ok - delete <del>
// * ok - kopirovanie s pc
// * ok - podpora pre linky (hlavne adresarov)

int main(int argc, char * argv[])
{
	QApplication app(argc, argv);

	directory_widget w(root);
	w.setWindowTitle("Andorid Explorer");
	w.resize(400, 600);
	w.show();

	return app.exec();
}
