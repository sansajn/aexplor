#include "directory_model.hpp"
#include <algorithm>
#include <vector>
#include <list>
#include <iterator>
#include <string>
#include <cstdio>
#include <cctype>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>

using std::any_of;
using std::find_if;
using std::advance;
using std::distance;
using std::vector;
using std::list;
using std::string;
using boost::trim;

static void ls(fs::path const & path, list<file_info> & result);
static bool ls_file(fs::path const & path, file_info & result);
static void rename(fs::path const & oldname, fs::path const & newname);
static void mkdir(fs::path const & dir);
static void rm(fs::path const & p);
static void cp(fs::path const & src, fs::path const & dst);
static bool file_compare(file_info const & a, file_info const & b);
static bool parse_ls_line(string const & line, file_info & result);
static bool parse_link(string const & name, string & link_name, string & link_to);


directory_model::directory_model(std::string const & path)
{
	change_directory(fs::path{path});
}

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
	bool operator()(file_info const & fi) const {return fi.name == name;}
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

		rm(_path / file_it->name);
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

		cp(src, _path);

		file_info fi;
		ls_file(_path / src.filename(), fi);

		beginInsertRows(QModelIndex{}, _files.size(), _files.size());
		_files.push_back(fi);
		endInsertRows();
	}
}

void directory_model::make_directory(QString local_name)
{
	string dirname = local_name.toStdString();

	mkdir(_path / dirname);

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
	ls(link ? _path / "/" : _path, _files);  // list links as 'ls -l sample/'
	_files.sort(file_compare);
	endResetModel();

	emit directory_changed(QString::fromUtf8(_path.c_str()), QString::fromUtf8(prev_path.c_str()));
}

void directory_model::rename(string const & oldval, string const & newval)
{
	::rename(_path / oldval, _path / newval);
}

/* linux 'ls -l' output format

	drwxr-xr-x  2 ja ja      4096 jún 28  2013 Public
	drwxr-xr-x  4 ja ja      4096 feb 24 09:07 Pictures
	-rw-rw-r--  1 ja ja  29020524 apr 21 07:11 PID Control - A brief introduction.mp4

	(1) file permissions,
	(2) number of links,
	(3) owner name,
	(4) owner group,
	(5) file size,
	(6) time of last modification, and
	(7) file/directory name
*/
bool parse_ls_line(string const & line, file_info & result)
{
	static string type_expr = R"((?'type'.))";
	static string permission_expr = R"((?'permission'(?:[r-][w-][x-]){3}))";
	static string links_expr = R"((?'links'\d+))";
	static string user_expr = R"((?'user'[^\s]+))";
	static string group_expr = R"((?'group'[^\s]+))";
	static string size_expr = R"((?'size'\d+))";
	static string date_expr = R"((?'date'[^\s]+\s+\d{1,2}\s+(?:\d{4}|(?:\d{2}:\d{2}))))";
	static string name_expr = R"((?'name'.+))";
	static string gap = R"(\s+)";

	static string expr = type_expr + permission_expr + gap + links_expr + gap + user_expr + gap +
		group_expr + gap + size_expr + gap + date_expr + R"(\s)" + name_expr;

	boost::regex e{expr};
	boost::regex e_partial{type_expr + permission_expr};

	boost::smatch what;
	if (boost::regex_match(line, what, e))  // full match
	{
		char type = what["type"].str()[0];

		result.name = what["name"];
		result.permission = what["permission"];
		result.size = std::stoul(what["size"]);
		result.directory = type == 'd';
		result.link = type == 'l';
		result.executable = what["permission"].str().find('x') != string::npos;

		if (result.link && result.name.back() == '/')  // directory-link: source -> data/source/
			result.directory = true;

		return true;
	}
	else if (boost::regex_search(line, what, e_partial))  // partial match based on permission
	{
		char type = what["type"].str()[0];

		result.name = line;
		result.permission = what["permission"];
		result.directory = type == 'd';
		result.link = type == 'l';
		result.executable = what["permission"].str().find('x') != string::npos;

		return true;
	}
	else
		return false;  // not a file or directory
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

inline string escaped(fs::path const & p)
{
	string path_str = p.string();

	string result;
	for (auto ch : path_str)
	{
		if (isblank(ch))
			result.push_back('\\');

		result.push_back(ch);
	}

	return result;
}

void mkdir(fs::path const & dir)
{
	string cmd = string{"mkdir -p "} + escaped(dir);
	system(cmd.c_str());
}

void rename(fs::path const & oldname, fs::path const & newname)
{
	string cmd = string{"mv "} + escaped(oldname) + " " + escaped(newname);
	system(cmd.c_str());
}

void ls(fs::path const & path, list<file_info> & result)
{
	string cmd = string{"ls -l "} + escaped(path);
	FILE * pin = popen(cmd.c_str(), "r");
	if (!pin)
		return;

	char buf[1024];
	while (fgets(buf, sizeof buf, pin))
	{
		string line{buf};
		trim(line);  // TODO: trim znehodnoti filename orezanim
		file_info fi;
		if (parse_ls_line(line, fi))
			result.push_back(fi);
	}

	pclose(pin);
}

bool ls_file(fs::path const & path, file_info & result)
{
	string cmd = string{"ls -ld "} + escaped(path);
	FILE * pin = popen(cmd.c_str(), "r");
	if (!pin)
		return false;

	char line[1024];
	char * rv = fgets(line, sizeof line, pin);

	pclose(pin);

	if (!rv)
		return false;

	if (parse_ls_line(string{line, line + strlen(line)-1}, result))
	{
		fs::path p{result.name};  // 'ls -ld' gets absolute file path

		if (result.directory)
		{
			fs::path::iterator it = p.end();
			--it;
			result.name = it->string();
		}
		else
			result.name = p.filename().string();

		return true;
	}
	else
		return false;
}

void cp(fs::path const & src, fs::path const & dst)
{
	string cmd = string{"cp "} + escaped(src) + " " + escaped(dst);
	system(cmd.c_str());
}

void rm(fs::path const & p)
{
	string cmd = string{"rm -rf "} + escaped(p);
	system(cmd.c_str());
}
