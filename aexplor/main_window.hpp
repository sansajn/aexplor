#pragma once
#include <QMainWindow>
#include <QPushButton>
#include "directory_widget.hpp"

class main_window : public QMainWindow
{
	Q_OBJECT

public:
	using base = QMainWindow;

	main_window();

private slots:
	void wsys_event(bool);

private:
	void mount_system_as_rw();

	QPushButton _wsys_btn;
	directory_widget _dirw;
};
