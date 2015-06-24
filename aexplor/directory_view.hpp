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
	directory_view(std::string const & root);
	std::string path() const {return _path.string();}

signals:
	void directory_changed(QString const & path);

protected:
	void keyPressEvent(QKeyEvent * event);

private:
	void dir_up();
	void dir_change(fs::path const & p);
	void update_view();
	void on_return();
	void on_backspace();

	fs::path _path;
	std::string _root;
	QStringListModel _model;
};
