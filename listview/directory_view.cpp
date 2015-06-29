#include "directory_view.hpp"
#include <string>
#include "mkdir_dialog.hpp"
#include "directory_model.hpp"

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
	if (event->key() == Qt::Key_Backspace)
		emit go_up();
	if (event->key() == Qt::Key_F7)
		mkdir_event();

	base::keyPressEvent(event);
}
