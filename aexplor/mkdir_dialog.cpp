#include "mkdir_dialog.hpp"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

mkdir_dialog::mkdir_dialog(std::string const & path, QWidget * parent)
	: QDialog(parent)
{
	QVBoxLayout * layout = new QVBoxLayout(this);

	QLabel * label = new QLabel;
	label->setText(QString("Create new folder in:\n%1").arg(path.c_str()));
	layout->addWidget(label);

	_dirname = new QLineEdit;
	_dirname->setClearButtonEnabled(true);
	_dirname->setText("New Folder");
	_dirname->selectAll();
	layout->addWidget(_dirname);

	QHBoxLayout * button_layout = new QHBoxLayout;

	QPushButton * ok = new QPushButton("&Ok");
	ok->setAutoDefault(true);
	connect(ok, SIGNAL(clicked()), this, SLOT(accept()));
	button_layout->addWidget(ok);

	QPushButton * cancel = new QPushButton("&Cancel");
	connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));
	button_layout->addWidget(cancel);

	layout->addLayout(button_layout);

	setWindowTitle("New Folder");
}

QString mkdir_dialog::result() const
{
	return _dirname->text();
}
