#include <string>
#include <cassert>
#include <QApplication>
#include "directory_view.hpp"

using std::string;

string const root = "~";


int main(int argc, char * argv[])
{
	QApplication app(argc, argv);

	directory_view view(root);
	view.show();

	return app.exec();
}
