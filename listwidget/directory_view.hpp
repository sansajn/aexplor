#pragma once
#include <string>
#include <boost/filesystem/path.hpp>
#include <QStringListModel>
#include <QListWidgetItem>

namespace fs = boost::filesystem;


struct file_info
{
	std::string name;
	std::string permission;
	size_t size = -1;
	bool link = false;
	bool directory = false;
	bool executable = false;
};

class directory_view : public QListWidget
{
	Q_OBJECT

public:
	using base = QListWidget;

	directory_view(std::string const & root, std::string const & remote = std::string());

signals:
	void directory_changed(QString const & path);

protected:
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
	void ls_long(std::string const & path, std::vector<file_info> & result) const;
	void mkdir(std::string const & path) const;
	QIcon file_icon(std::string const & path, file_info const & file);

	fs::path _path;
	std::string _root;
	std::string _remote;  // remote command
	QPoint _start_drag_pos;
};
