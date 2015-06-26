#pragma once
#include <string>
#include <QListView>
#include <QStringListModel>
#include <QListWidgetItem>
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
	using base = QListView;

	void dropEvent(QDropEvent * event) override;
	void dragEnterEvent(QDragEnterEvent * event) override;
	void dragMoveEvent(QDragMoveEvent * event) override;
	void mousePressEvent(QMouseEvent * event) override;
	void mouseMoveEvent(QMouseEvent * event) override;
	void keyPressEvent(QKeyEvent * event) override;

private:
	std::vector<QString> selected_items();
	void performDrag();
	void dir_up();
	void dir_enter();
	void dir_delete();
	void dir_make();
	void dir_rename();
	void dir_change(fs::path const & p);
	void update_view();
	bool directory(fs::path const & p) const;
	bool directory_link(fs::path const & p) const;
	std::string ls_file(std::string const & path) const;
	void ls(std::string const & path, QStringList & result) const;
	void mkdir(std::string const & path) const;

	fs::path _path;
	std::string _root;
	std::string _remote;  // remote command
	QStringListModel _model;
	QPoint _start_drag_pos;
};
