#include "directory_widget.hpp"
#include <string>
#include <boost/filesystem/path.hpp>
#include <QVBoxLayout>
#include <QModelIndex>

using std::string;
namespace fs = boost::filesystem;

static fs::path relative_path(fs::path const & p1, fs::path const & p2);


directory_widget::directory_widget()
{
	QVBoxLayout * layout = new QVBoxLayout{this};

	_view.setModel(&_model);
	_view.setAlternatingRowColors(true);

	// view - model connections
	connect(&_view, SIGNAL(item_opened(QModelIndex)), &_model, SLOT(open_item(QModelIndex)));
	connect(&_view, SIGNAL(go_up()), &_model, SLOT(go_up()));
	connect(&_view, SIGNAL(directory_created(QString)), &_model, SLOT(make_directory(QString)));
	connect(&_view, SIGNAL(item_removed(QItemSelectionModel*)), &_model, SLOT(remove_item(QItemSelectionModel*)));

	_path_label.setText("<current-directory-path>");
	connect(&_model, SIGNAL(directory_changed(QString, QString)), this, SLOT(change_directory(QString, QString)));

	layout->addWidget(&_path_label);
	layout->addWidget(&_view);

	_model.path("~");
}

void directory_widget::change_directory(QString path, QString prev_path)
{
	_path_label.setText(path);

	if (path.size() > prev_path.size())  // vosiel som do adresara, koniec
		return;

	fs::path relpath = relative_path(fs::path{path.toStdString()}, fs::path{prev_path.toStdString()});

	QModelIndex idx = _model.find(relpath.begin()->string());
	if (idx.isValid())
		_view.setCurrentIndex(idx);
}

fs::path relative_path(fs::path const & p1, fs::path const & p2)
{
	assert(p1.native().size() <= p2.native().size() && "p1 is not a subpath of p2");

	fs::path::iterator it = p2.begin();

	for (auto elem : p1)
	{
		if (*it == elem)
			++it;
		else
			break;
	}

	fs::path result;
	for (; it != p2.end(); ++it)
		result /= *it;

	return result;
}