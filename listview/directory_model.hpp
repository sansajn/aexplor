#pragma once
#include <map>
#include <list>
#include <string>
#include <boost/filesystem/path.hpp>
#include <QAbstractListModel>
#include <QStringList>
#include <QItemSelection>
#include <QIcon>

namespace fs = boost::filesystem;

struct file_info
{
	std::string name;
	std::string permission;
	size_t size = -1;
	bool link = false;
	bool directory = false;
	bool executable = false;

	file_info() {}
	file_info(std::string const & name, bool directory = true) : name{name}, directory{directory} {}
};

class directory_model : public QAbstractListModel
{
	Q_OBJECT

public:
	directory_model() {}
	directory_model(std::string const & path);

	std::string path() const;
	void path(std::string const & p);
	QModelIndex find(std::string const & file_name) const;

	Qt::ItemFlags flags(QModelIndex const & index) const override;
	QVariant data(QModelIndex const & index, int role = Qt::DisplayRole) const override;
	int rowCount(QModelIndex const & parent = QModelIndex()) const override;
	bool setData(QModelIndex const & index, QVariant const & value, int role = Qt::EditRole) override;

public slots:
	void go_up();
	void open_item(QModelIndex index);
	void remove_item(QItemSelectionModel * selection);
	void make_directory(QString local_name);

signals:
	void directory_changed(QString path, QString prev_path);  // TODO: make &

private:
	QIcon get_icon(std::string const & name) const;
	void change_directory(fs::path const & path, bool link = false);
	void rename(std::string const & oldval, std::string const & newval);

	fs::path _path;
	std::list<file_info> _files;
	std::map<fs::path, file_info> _path_history;
	mutable std::map<std::string, QIcon> _icons;
};
