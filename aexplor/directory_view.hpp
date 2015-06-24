#pragma once
#include <string>
#include <QWidget>
#include <QListView>
#include <QStringListModel>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

class directory_view : public QListView
{
	Q_OBJECT

public:
	directory_view(std::string const & path);

protected:
	void keyPressEvent(QKeyEvent * event);

private:
	void dir_up();
	void dir_change();
	void update_view();

	fs::path _path;
	QStringListModel _model;
};
