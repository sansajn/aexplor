#include <string>
#include <cassert>
#include <QApplication>
#include "directory_widget.hpp"

using std::string;

string const root = "~";


int main(int argc, char * argv[])
{
	QApplication app(argc, argv);

	directory_widget w(root);
	w.show();

	return app.exec();
}
