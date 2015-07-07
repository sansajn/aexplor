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
	void wsys_click_event(bool);

private:
	QPushButton _wsys_btn;
	directory_widget _dirw;
};
