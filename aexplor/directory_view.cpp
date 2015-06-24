#include "directory_view.hpp"
#include <cstdio>
#include <QMessageBox>
#include <QKeyEvent>
#include <QVBoxLayout>

using std::string;

static void ls(string const & path, QStringList & result);
static string ls_file(string const & path);
static bool directory(fs::path const & p);

directory_view::directory_view(std::string const & root)
	: QListView(nullptr), _root(root)
{
	setModel(&_model);
	dir_change(_root);
}

void directory_view::keyPressEvent(QKeyEvent * event)
{
	if (event->key() == Qt::Key_Return)
		on_return();
	else if (event->key() == Qt::Key_Backspace)
		on_backspace();
	else
		QListView::keyPressEvent(event);
}

void directory_view::on_return()
{
	QString item = model()->data(currentIndex()).toString();

	if (item == "..")
		dir_up();
	else
	{
		fs::path p(_path);
		p /= item.toStdString();  // TODO: c_str()

		if (directory(p))
			dir_change(p);
	}
}

void directory_view::on_backspace()
{
	dir_up();
}

void directory_view::dir_up()
{
	if (_path.compare(_root) != 0)  // nie som v roote
		dir_change(_path.parent_path());
}

void directory_view::dir_change(fs::path const & p)
{
	_path = p;
	update_view();
	emit directory_changed(QString::fromUtf8(_path.c_str()));
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
	string cmd = string("ls --group-directories-first ") + path;
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

bool directory(fs::path const & p)
{
	string line = ls_file(p.string());
	return line.back() == '/';
}
