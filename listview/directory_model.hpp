#pragma once
#include <string>
#include <boost/filesystem/path.hpp>
#include <QAbstractListModel>
#include <QStringList>

namespace fs = boost::filesystem;

class directory_model : public QAbstractListModel
{
	Q_OBJECT

public:
	directory_model(std::string const & path);
	Qt::ItemFlags flags(QModelIndex const & index) const;
	QVariant data(QModelIndex const & index, int role = Qt::DisplayRole) const;
	int rowCount(QModelIndex const & parent = QModelIndex()) const;
	bool setData(QModelIndex const & index, QVariant const & value, int role = Qt::EditRole);

public slots:
	void open_item(QModelIndex idx);

private:
	void go_up();
	void change_directory(fs::path const & path);
	void rename(QString const & oldval, QString const & newval);

	fs::path _path;
	QStringList _files;
};
