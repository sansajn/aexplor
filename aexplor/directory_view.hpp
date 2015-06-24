#pragma once
#include <string>
#include <QWidget>
#include <QListView>
#include <QStringListModel>
#include <boost/filesystem/path.hpp>

namespace fs = boost::filesystem;

class directory_view : public QListView
{
	Q_OBJECT

public:
	directory_view(std::string const & root, std::string const & remote = std::string());

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
	bool directory(fs::path const & p) const;
	std::string ls_file(std::string const & path) const;
	void ls(std::string const & path, QStringList & result) const;

	fs::path _path;
	std::string _root;
	std::string _remote;  // remote command
	QStringListModel _model;
};
