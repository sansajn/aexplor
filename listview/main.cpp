#include <QApplication>
#include <QVBoxLayout>
#include <QLabel>
#include "directory_view.hpp"
#include "directory_model.hpp"

/*
TODO:
* navrat po linku
* restore position on up-dir
* ikony (directory, executable, ...)
* permissions, size, date
* zoznam prikazou ? (mkdir:<f7>, remove:<del>, rename)
* su mode
* remount (/system)
* favorites
* kopirovanie s pc
* kopirovanie do pc (drop event dostane cielova aplykacia a ta nevie ako ziskat subory cez adb)
* local-search dialog
* view file <f3>
* edit file <f4>
* file operations in thread
* d&d copy, move context menu
* ok - podpora pre linky (hlavne adresarov)
* ok - sort
* ok - rename <double-click>
* ok - mkdir <f7>
* ok - delete <del>
*/

int main(int argc, char * argv[])
{
	QApplication app(argc, argv);

	QWidget w;
	QVBoxLayout layout{&w};

	directory_model model;

	directory_view view;
	view.setModel(&model);
	view.setAlternatingRowColors(true);

	// view - model connections
	QObject::connect(&view, SIGNAL(item_opened(QModelIndex)), &model, SLOT(open_item(QModelIndex)));
	QObject::connect(&view, SIGNAL(go_up()), &model, SLOT(go_up()));
	QObject::connect(&view, SIGNAL(directory_created(QString)), &model, SLOT(make_directory(QString)));
	QObject::connect(&view, SIGNAL(item_removed(QItemSelectionModel*)), &model, SLOT(remove_item(QItemSelectionModel*)));

	QLabel path_lbl{"<current-directory-path>"};
	QObject::connect(&model, SIGNAL(directory_changed(QString)), &path_lbl, SLOT(setText(QString)));

	layout.addWidget(&path_lbl);
	layout.addWidget(&view);

	model.path("~");

	w.show();

	return app.exec();
}
