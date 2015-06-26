#include "directory_view.hpp"
#include <vector>
#include <cstdio>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <QMessageBox>
#include <QKeyEvent>
#include <QUrl>
#include <QMimeData>
#include <QListWidgetItem>
#include <QDrag>
#include <QApplication>
#include "mkdir_dialog.hpp"

using std::vector;
using std::string;
using boost::trim;
using boost::starts_with;

static void copy_to_device(vector<string> const & files, string const & dst);

directory_view::directory_view(std::string const & root, std::string const & remote)
	: QListView(nullptr), _root(root), _remote(remote)
{
	setModel(&_model);
	dir_change(_root);
	setAcceptDrops(true);  // drag&drop support
	setDragEnabled(true);
	setSelectionMode(ExtendedSelection);
	setAlternatingRowColors(true);
}

void directory_view::dragEnterEvent(QDragEnterEvent * event)
{
	if (event->source() != this)
	{
		event->setDropAction(Qt::CopyAction);
		event->accept();
	}
//	event->acceptProposedAction();
}

void directory_view::dragMoveEvent(QDragMoveEvent * event)
{
//	event->acceptProposedAction();
}

void directory_view::dropEvent(QDropEvent * event)
{
	if (event->source() == this)
		return;

	event->setDropAction(Qt::CopyAction);
	event->accept();

	QMimeData const * mime = event->mimeData();

	if (mime && mime->hasUrls())
	{
		vector<string> files;
		for (QUrl & url : mime->urls())
			files.push_back(url.toLocalFile().toStdString());

		if (!files.empty())
		{
			copy_to_device(files, _path.c_str());
			update_view();
		}
	}

	assert(mime && "unknown mime data");
}

void directory_view::performDrag()
{
//	QList<QListWidgetItem *> items = selectedItems();
	vector<QString> items = selected_items();

	if (items.empty())
		return;

	QList<QUrl> urls;
	for (QString & item : items)
		urls.push_back(QUrl::fromLocalFile(item));

	QMimeData * mime = new QMimeData;
	mime->setUrls(urls);

	QDrag * drag = new QDrag(this);
	drag->setMimeData(mime);
//	drag->setPixmap(QPixmap(":/images/person.png"));
	if (drag->exec(Qt::CopyAction) == Qt::CopyAction)  // set to Qt::MoveAction to move
		;

	// TODO: kto uprace drag ??
}

void directory_view::mousePressEvent(QMouseEvent * event)
{
	if (event->button() == Qt::LeftButton)
		_start_drag_pos = event->pos();
	base::mousePressEvent(event);
}

void directory_view::mouseMoveEvent(QMouseEvent * event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		int distance = (event->pos() - _start_drag_pos).manhattanLength();
		if (distance >= QApplication::startDragDistance())
			performDrag();
	}
	base::mouseMoveEvent(event);
}

void directory_view::keyPressEvent(QKeyEvent * event)
{
	if (event->key() == Qt::Key_Return)
		dir_enter();
	else if (event->key() == Qt::Key_Backspace)
		dir_up();
	else if (event->key() == Qt::Key_Delete)
		dir_delete();
	else if (event->key() == Qt::Key_F7)
		dir_make();
	else
		QListView::keyPressEvent(event);
}

void directory_view::dir_enter()
{
	QString item = model()->data(currentIndex()).toString();

	if (item == "..")
		dir_up();
	else
	{
		fs::path p(_path);
		p /= item.toStdString();  // TODO: c_str()

		if (directory(p) || directory_link(p))
			dir_change(p);
	}
}

void directory_view::dir_delete()
{
	QString item = model()->data(currentIndex()).toString();

	if (item == "..")
		return;

	fs::path p(_path);
	p /= item.toStdString();

	string cmd;

	if (!directory(p))
		cmd = string("/home/ja/opt/android-sdk-linux/platform-tools/adb shell rm ") + p.string();
	else
		cmd = string("/home/ja/opt/android-sdk-linux/platform-tools/adb shell rm -rf ") + p.string();

	system(cmd.c_str());  // TODO: delete in thread
	update_view();
}

void directory_view::dir_make()
{
	mkdir_dialog d(_path.string(), this);
	if (d.exec() == QDialog::Accepted)
	{
		QString dir = d.result();
		fs::path p = _path;
		p /= dir.toStdString();
		mkdir(p.string());
		update_view();
	}
}

std::vector<QString> directory_view::selected_items()
{
	vector<QString> items;
	for (QModelIndex idx : selectionModel()->selection().indexes())
		items.push_back(idx.data(Qt::DisplayRole).toString());
	return items;
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

void directory_view::mkdir(std::string const & path) const
{
	string cmd = string("/home/ja/opt/android-sdk-linux/platform-tools/adb shell mkdir ") + path;
	system(cmd.c_str());
}

bool directory_view::directory(fs::path const & p) const
{
	string line = ls_file(p.string());
	return line[0] == 'd';
}

bool directory_view::directory_link(fs::path const & p) const
{
	return starts_with(ls_file(p.string()), "ld");
}

void copy_to_device(vector<string> const & files, string const & dst)
{
	// TODO: resolve adb path
	// TODO: use thread to push
	// TODO: vysualize using -p flag
	for (string const & file : files)
	{
		string cmd = string("/home/ja/opt/android-sdk-linux/platform-tools/adb push ") + file + string(" ") + dst;
		system(cmd.c_str());
	}
}
