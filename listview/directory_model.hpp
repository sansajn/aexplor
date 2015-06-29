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
	directory_model() {}
	directory_model(std::string const & path);

	std::string path() const;
	void path(std::string const & p);

	Qt::ItemFlags flags(QModelIndex const & index) const override;
	QVariant data(QModelIndex const & index, int role = Qt::DisplayRole) const override;
	int rowCount(QModelIndex const & parent = QModelIndex()) const override;
	bool setData(QModelIndex const & index, QVariant const & value, int role = Qt::EditRole) override;

public slots:
	void go_up();
	void open_item(QModelIndex idx);
	void make_directory(QString local_name);

signals:
	void directory_changed(QString);

private:
	void change_directory(fs::path const & path);
	void rename(QString const & oldval, QString const & newval);

	fs::path _path;
	QStringList _files;
};
