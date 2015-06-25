#include <string>
#include <cassert>
#include <QApplication>
#include "directory_widget.hpp"

using std::string;

string const root = "/";

// TODO:
// * mkdir <f7>
// * kopirovanie do pc
// * rename <f2>
// * su mode
// * remount (/system)
// * ikony
// * permissions, size, date
// * favorites
// * view file <f3>
// * edit file <f4>
// * d&d copy, move context menu
// * file operations in thread
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
