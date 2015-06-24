#pragma once
#include <QWidget>
#include <QLabel>
#include "directory_view.hpp"

class directory_widget : public QWidget
{
	Q_OBJECT

public:
	directory_widget(std::string const & root, QWidget * parent = nullptr);

private:
	directory_view * _view;
	QLabel * _path;
};
