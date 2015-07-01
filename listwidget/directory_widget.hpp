#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include "directory_view.hpp"

class directory_widget : public QWidget
{
	Q_OBJECT

public:
	directory_widget(std::string const & root, QWidget * parent = nullptr);

private:
	directory_view * _view;
	QLabel * _path;
	QPushButton * _mkdir_btn;
	QPushButton * _del_btn;
	QPushButton * _rename_btn;
	QPushButton * _view_btn;
};
