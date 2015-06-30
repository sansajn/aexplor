#pragma once
#include <QWidget>
#include <QLabel>
#include <QString>
#include "directory_view.hpp"
#include "directory_model.hpp"

class directory_widget : public QWidget
{
	Q_OBJECT

public:
	directory_widget();

private slots:
	void change_directory(QString path, QString prev_path);


private:
	directory_view _view;
	directory_model _model;
	QLabel _path_label;
};
