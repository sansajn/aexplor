#pragma once
#include <QDialog>
#include <QLabel>

class help_dialog : public QDialog
{
	Q_OBJECT

public:
	help_dialog(QWidget * parent = nullptr);

private:
	QLabel _text;
};
