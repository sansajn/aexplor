#include "directory_view.hpp"
#include <string>
#include <vector>
#include <QKeyEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QMimeData>
#include <QApplication>
#include <QDrag>
#include "directory_model.hpp"
#include "mkdir_dialog.hpp"
#include "help_dialog.hpp"

using std::string;
using std::vector;

directory_view::directory_view()
{
	setAcceptDrops(true);  // drag&drop support
	setSelectionMode(ExtendedSelection);
}

string directory_view::current_directory() const
{
	directory_model * m = qobject_cast<directory_model *>(model());
	assert(m && "model is not instance of directory_model");
	return m ? m->path() : string{};
}

void directory_view::mkdir_event()
{
	mkdir_dialog d(current_directory(), this);
	if (d.exec() == QDialog::Accepted)
		emit directory_created(d.result());
}

void directory_view::keyPressEvent(QKeyEvent * event)
{
	if (event->key() == Qt::Key_Return)
		emit item_opened(selectionModel()->currentIndex());
	else if (event->key() == Qt::Key_Backspace)
		emit go_up();
	else if (event->key() == Qt::Key_F7)
		mkdir_event();
	else if (event->key() == Qt::Key_Delete)
		emit item_removed(selectionModel());
	else if (event->key() == Qt::Key_Question)
	{
		help_dialog d{this};
		d.exec();
	}

	base::keyPressEvent(event);
}

void directory_view::dragEnterEvent(QDragEnterEvent * event)
{
	if (event->source() != this)
	{
		event->setDropAction(Qt::CopyAction);
		event->accept();
	}
}

void directory_view::dragMoveEvent(QDragMoveEvent * event)
{}

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
			emit item_dropped(files);
	}

	assert(mime && "unknown mime data");
}
