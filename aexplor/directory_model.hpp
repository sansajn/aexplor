#pragma once
#include <map>
#include <list>
#include <string>
#include <QAbstractListModel>
#include <QStringList>
#include <QItemSelection>
#include <QIcon>
#include "system.hpp"

class directory_model : public QAbstractListModel
{
	Q_OBJECT

public:
	directory_model();
	directory_model(std::string const & path);
	~directory_model();

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
	void drop_item(std::vector<std::string> const & files);
	void view_item(QModelIndex idx);
	void make_directory(QString local_name);

signals:
	void directory_changed(QString path, QString prev_path);  // TODO: make &
	void current_index_changed(QModelIndex idx);

private:
	file_info & get_file(int row);
	QIcon get_icon(std::string const & name) const;
	void change_directory(fs::path const & path, bool link = false);
	void rename(std::string const & oldval, std::string const & newval);

	fs::path _path;
	std::list<file_info> _files;
	std::map<fs::path, file_info> _path_history;
	mutable std::map<std::string, QIcon> _icons;
	abstract_system * _sys;
};
