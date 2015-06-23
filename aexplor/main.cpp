#include <string>
#include <cstdio>
#include <cassert>
#include <QApplication>
#include <QListView>
#include <QStringListModel>

using std::string;

string const root = "~";


void ls(string const & path, QStringList & result);


int main(int argc, char * argv[])
{
	QApplication app(argc, argv);

	QStringList strings;
	strings << "..";
	ls(root, strings);

	QStringListModel model;
	model.setStringList(strings);

	QListView view;
	view.setModel(&model);

	view.show();

	return app.exec();
}

void ls(string const & path, QStringList & result)
{
	string cmd = string("ls ") + path;
	FILE * pin = popen(cmd.c_str(), "r");
	if (!pin)
		return;

	char line[1024];
	while (fgets(line, sizeof line, pin))
	{
		int len = strlen(line);
		if (line[len-1] == '\n')
			result << QString::fromUtf8(line, len-1);
		else
			result << line;
	}

	pclose(pin);
}
