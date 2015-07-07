#include "directory_model.hpp"
#include <algorithm>
#include <vector>
#include <list>
#include <iterator>
#include <string>
#include <cstdio>
#include <cctype>
#include <boost/regex.hpp>

using std::any_of;
using std::find_if;
using std::advance;
using std::distance;
using std::vector;
using std::list;
using std::string;

static bool parse_link(string const & name, string & link_name, string & link_to);

directory_model::directory_model()
{
	_sys = get_system_impl();
}

directory_model::directory_model(std::string const & path)
{
	_sys = get_system_impl();
	change_directory(fs::path{path});
}

directory_model::~directory_model()
{}

std::string directory_model::path() const
{
	return _path.string();
}

void directory_model::path(std::string const & p)
{
	change_directory(fs::path{p});
}

struct find_by_name
{
	find_by_name(string const & name) : name{name} {}

	bool operator()(file_info const & fi) const
	{
		if (fi.link)
			return fi.name.find(name) == 0;
		else
			return fi.name == name;
	}

	string name;
};

QModelIndex directory_model::find(std::string const & file_name) const
{
	list<file_info>::const_iterator it = find_if(_files.begin(), _files.end(), find_by_name{file_name});
	if (it != _files.end())
		return index(distance(_files.begin(), it));
	else
		return QModelIndex{};
}

Qt::ItemFlags directory_model::flags(QModelIndex const & index) const
{
	return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

QVariant directory_model::data(QModelIndex const & index, int role) const
{
	switch (role)
	{
		case Qt::EditRole:
		case Qt::DisplayRole:
		{
			if (index.column() == 0 && index.row() < _files.size())
			{
				list<file_info>::const_iterator it = _files.begin();
				advance(it, index.row());
				return QString::fromUtf8(it->name.c_str());
			}
			break;
		}

		case Qt::DecorationRole:
		{
			if (index.column() == 0 && index.row() < _files.size())
			{
				QVariant result;

				list<file_info>::const_iterator it = _files.begin();
				advance(it, index.row());
				if (it->directory)
					result.setValue(get_icon("folder"));
				else if (it->executable)
					result.setValue(get_icon("application-x-executable"));
				else
					result.setValue(get_icon("document-new"));

				return result;
			}
			break;
		}
	}  // switch

	return QVariant{};
}

int directory_model::rowCount(QModelIndex const & parent) const
{
	return _files.size();
}

bool directory_model::setData(QModelIndex const & index, QVariant const & value, int role)
{
	if (role != Qt::EditRole || index.column() != 0 || index.row() >= _files.size())
		return false;

	string newname = value.toString().toStdString();

	list<file_info>::iterator file_it = _files.begin();
	advance(file_it, index.row());

	if (newname == file_it->name)
		return false;

	rename(file_it->name, newname);
	file_it->name = newname;

	emit dataChanged(index, index);

	return true;
}

void directory_model::go_up()
{
	auto rm_it = _path_history.find(_path);  // odstran ~/temp az ked opustam ~/temp do ~
	if (rm_it != _path_history.end())
		_path_history.erase(rm_it);

	fs::path p = _path.parent_path();
	auto file_it = _path_history.find(p);
	if (file_it != _path_history.end())
	{
		file_info & fi = file_it->second;
		assert(fi.directory && "directory expected");
		change_directory(p, fi.link);
	}
	else
		change_directory(p);
}

void directory_model::open_item(QModelIndex index)
{
	if (index.model() != this)
		return;

	QString text = index.data().toString();
	if (text == "..")
		go_up();

	list<file_info>::iterator file_it = _files.begin();
	advance(file_it, index.row());

	if (file_it->directory)
	{
		if (file_it->link)  // directory link
		{
			string name, to;
			if (parse_link(file_it->name, name, to))
			{
				fs::path p = _path / name;
				_path_history[p] = *file_it;  // save before change
				change_directory(p, true);
			}
		}
		else // directory
			change_directory(_path / text.toStdString());
	}
}

void directory_model::remove_item(QItemSelectionModel * selection)
{
	if (selection->model() != this)
		return;

	QModelIndexList indexes = selection->selection().indexes();

	vector<list<file_info>::const_iterator> remove_iter_list;
	remove_iter_list.reserve(indexes.size());

	// first, remove from fs
	for (QModelIndex & index : selection->selection().indexes())
	{
		int row = index.row();

		list<file_info>::const_iterator file_it = _files.begin();
		advance(file_it, row);
		remove_iter_list.push_back(file_it);

		_sys->rm(_path / file_it->name);
	}

	beginResetModel();
	for (auto it : remove_iter_list)
		_files.erase(it);
	endResetModel();
}

void directory_model::drop_item(std::vector<std::string> const & files)
{
	for (string const & file : files)
	{
		fs::path src{file};

		_sys->cp(src, _path);

		file_info fi;
		_sys->ls_file(_path / src.filename(), fi);

		beginInsertRows(QModelIndex{}, _files.size(), _files.size());
		_files.push_back(fi);
		endInsertRows();
	}
}

void directory_model::make_directory(QString local_name)
{
	string dirname = local_name.toStdString();

	_sys->mkdir(_path / dirname);

	// TODO: vloz adresar na spravnu poziciu, nie na koniec
	beginInsertRows(QModelIndex{}, _files.size(), _files.size());
	_files.emplace_back(dirname, true);  // TODO: ak sa mkdir nepodari, nevkladaj
	endInsertRows();
}

bool file_compare(file_info const & a, file_info const & b)
{
	if (a.directory && !b.directory)
		return true;
	else if (!a.directory && b.directory)
		return false;
	else
		return a.name < b.name;
}

QIcon directory_model::get_icon(std::string const & name) const
{
	auto it = _icons.find(name);

	if (it != _icons.end())
		return it->second;
	else
	{
		auto ires = _icons.insert(make_pair(name, QIcon::fromTheme(name.c_str())));
		assert(ires.second && "insertion failed");
		return ires.first->second;
	}
}

void directory_model::change_directory(fs::path const & path, bool link)
{
	fs::path prev_path{_path};
	_path = path;

	beginResetModel();
	_files.clear();
	_files.emplace_back("..", true);
	_sys->ls(link ? _path / "/" : _path, _files);  // list links as 'ls -l sample/'
	_files.sort(file_compare);
	endResetModel();

	emit directory_changed(QString::fromUtf8(_path.c_str()), QString::fromUtf8(prev_path.c_str()));
}

void directory_model::rename(string const & oldval, string const & newval)
{
	_sys->rename(_path / oldval, _path / newval);
}

bool parse_link(string const & name, string & link_name, string & link_to)
{
	// source -> data/source/
	boost::regex e{R"((?'name'.+)\s->\s(?'to'.+))"};

	boost::smatch what;
	if (boost::regex_match(name, what, e))
	{
		link_name = what["name"];
		link_to = what["to"];
		return true;
	}

	return false;
}
