#include "directory_view.hpp"
#include <string>
#include "directory_model.hpp"
#include "mkdir_dialog.hpp"
#include "help_dialog.hpp"

using std::string;

directory_view::directory_view()
{}

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
