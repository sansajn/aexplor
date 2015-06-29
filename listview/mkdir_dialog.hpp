#pragma once
#include <string>
#include <QDialog>
#include <QLineEdit>

/**/
class mkdir_dialog : public QDialog
{
	Q_OBJECT

public:
	mkdir_dialog(std::string const & path, QWidget * parent = nullptr);

	QString result() const;

private:
	QLineEdit * _dirname;
};
