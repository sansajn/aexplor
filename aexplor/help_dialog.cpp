#include "help_dialog.hpp"
#include <QVBoxLayout>

help_dialog::help_dialog(QWidget * parent)
	: QDialog{parent}
{
	setWindowTitle("Help");

	QString str =
		"Commands:\n"
		"<del> remove file/directory\n"
		"<f7> create new directory\n"
		"<f3> text quick view\n"
		"<double-click> rename file/directory\n"
		"<backspace> parent directory\n"
		"<drag&drop> copy to device\n";

	_text.setText(str);

	QVBoxLayout * layout = new QVBoxLayout{this};
	layout->addWidget(&_text);
}
