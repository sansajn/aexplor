#include "directory_view.hpp"
#include <cstdio>
#include <QMessageBox>
#include <QKeyEvent>

using std::string;

static void ls(string const & path, QStringList & result);

directory_view::directory_view(std::string const & path)
	: QListView(nullptr), _path(path)
{
	setModel(&_model);
	update_view();
}

void directory_view::keyPressEvent(QKeyEvent * event)
{
	if (event->key() == Qt::Key_Return)
		dir_change();
	else
		QListView::keyPressEvent(event);
}

void directory_view::dir_up()
{
	_path = _path.parent_path();
	update_view();
}

string ls_file(string const & path)
{
	string cmd = string("ls -Fd ") + path;
	FILE * pin = popen(cmd.c_str(), "r");
	if (!pin)
		return string();

	char line[1024];
	char * rv = fgets(line, sizeof line, pin);

	pclose(pin);

	if (!rv)
		return string();

	return string(line, line+strlen(line)-1);  // ignore last '\n'
}

static bool directory(fs::path const & p)
{
	string line = ls_file(p.string());
	return line.back() == '/';
}

void directory_view::dir_change()
{
	QString item = model()->data(currentIndex()).toString();

	fs::path p(_path);
	p /= item.toStdString();
	if (directory(p))
	{
		_path = p;
		update_view();
	}
}

void directory_view::update_view()
{
	QStringList files;
	files << "..";
	ls(_path.string(), files);
	_model.setStringList(files);
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
