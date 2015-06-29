#include "directory_model.hpp"
#include <vector>
#include <string>
#include <cstdio>
#include <boost/algorithm/string/trim.hpp>

using std::vector;
using std::string;
using boost::trim;

static void ls(fs::path const & path, QStringList & result);
static void rename(fs::path const & oldname, fs::path const & newname);
static bool directory(fs::path const & p);

directory_model::directory_model(std::string const & path)
{
	change_directory(fs::path{path});
}

Qt::ItemFlags directory_model::flags(QModelIndex const & index) const
{
	return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

QVariant directory_model::data(QModelIndex const & index, int role) const
{
	if (role != Qt::DisplayRole && role != Qt::EditRole)
		return QVariant{};

	if (index.column() == 0 && index.row() < _files.count())
		return _files.at(index.row());
	else
		return QVariant{};
}

int directory_model::rowCount(QModelIndex const & parent) const
{
	return _files.count();
}

bool directory_model::setData(QModelIndex const & index, QVariant const & value, int role)
{
	if (role != Qt::EditRole || index.column() != 0 || index.row() >= _files.count())
		return false;

	if (value.toString() == _files.at(index.row()))
		return false;

	rename(_files[index.row()], value.toString());

	_files[index.row()] = value.toString();

	emit dataChanged(index, index);

	return true;
}

void directory_model::open_item(QModelIndex idx)
{
	if (idx.model() != this)
		return;

	QString text = idx.data().toString();
	if (text == "..")
		go_up();

	fs::path p{_path};
	p /= idx.data().toString().toStdString();

	if (directory(p))
		change_directory(p);
}

void directory_model::go_up()
{
	change_directory(_path.parent_path());
}

void directory_model::change_directory(fs::path const & path)
{
	beginResetModel();
	_path = path;
	_files.clear();
	_files << "..";
	ls(_path, _files);
	endResetModel();
}

void directory_model::rename(QString const & oldval, QString const & newval)
{
	fs::path oldpath{_path};
	oldpath /= oldval.toStdString();
	fs::path newpath{_path};
	newpath /= newval.toStdString();
	::rename(oldpath, newpath);
}

void rename(fs::path const & oldname, fs::path const & newname)
{
	string cmd = string{"mv "} + oldname.string() + " " + newname.string();
	system(cmd.c_str());
}

void ls(fs::path const & path, QStringList & result)
{
	string cmd = string{"ls "} + path.string();
	FILE * pin = popen(cmd.c_str(), "r");
	if (!pin)
		return;

	char buf[1024];
	while (fgets(buf, sizeof buf, pin))
	{
		string line(buf);
		trim(line);
		result << QString::fromUtf8(line.c_str());
	}

	pclose(pin);
}

bool directory(fs::path const & p)
{
	return (p.string().find('.') == string::npos);
}
