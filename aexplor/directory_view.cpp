#include "directory_view.hpp"
#include <cstdio>
#include <boost/algorithm/string/trim.hpp>
#include <QMessageBox>
#include <QKeyEvent>
#include <QVBoxLayout>

using std::string;
using boost::trim;

directory_view::directory_view(std::string const & root, std::string const & remote)
	: QListView(nullptr), _root(root), _remote(remote)
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

void directory_view::ls(string const & path, QStringList & result) const
{
	string cmd = _remote + string(" ls ") + path;
	FILE * pin = popen(cmd.c_str(), "r");
	if (!pin)
		return;

	char buf[1024];
	while (fgets(buf, sizeof buf, pin))
	{
		string line(buf);
		trim(line);
		result << QString::fromUtf8(line.c_str());
	}

	pclose(pin);
}

string directory_view::ls_file(string const & path) const
{
	string cmd = _remote + string(" ls -Fd ") + path;
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

bool directory_view::directory(fs::path const & p) const
{
	string line = ls_file(p.string());
	return line[0] == 'd';
}
