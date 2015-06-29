#include <QApplication>
#include "directory_view.hpp"
#include "directory_model.hpp"

int main(int argc, char * argv[])
{
	QApplication app(argc, argv);

	directory_view w;
	directory_model model{"~"};
	QObject::connect(&w, SIGNAL(item_opened(QModelIndex)), &model, SLOT(open_item(QModelIndex)));
	w.setModel(&model);
	w.show();

	return app.exec();
}
