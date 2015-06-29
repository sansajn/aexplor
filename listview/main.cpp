#include <QApplication>
#include <QVBoxLayout>
#include <QLabel>
#include "directory_view.hpp"
#include "directory_model.hpp"

int main(int argc, char * argv[])
{
	QApplication app(argc, argv);

	QWidget w;
	QVBoxLayout layout{&w};

	directory_model model;

	directory_view view;
	view.setModel(&model);

	// view - model connections
	QObject::connect(&view, SIGNAL(item_opened(QModelIndex)), &model, SLOT(open_item(QModelIndex)));
	QObject::connect(&view, SIGNAL(go_up()), &model, SLOT(go_up()));
	QObject::connect(&view, SIGNAL(directory_created(QString)), &model, SLOT(make_directory(QString)));

	QLabel path_lbl{"<current-directory-path>"};
	QObject::connect(&model, SIGNAL(directory_changed(QString)), &path_lbl, SLOT(setText(QString)));

	layout.addWidget(&path_lbl);
	layout.addWidget(&view);

	model.path("~");


	w.show();

	return app.exec();
}
