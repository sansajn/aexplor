#include "directory_view.hpp"

directory_view::directory_view()
{}

void directory_view::on_return()
{
	emit item_opened(selectionModel()->currentIndex());
}

void directory_view::keyPressEvent(QKeyEvent * event)
{
	if (event->key() == Qt::Key_Return)
		on_return();

	base::keyPressEvent(event);
}
