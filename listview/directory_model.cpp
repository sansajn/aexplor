#include "directory_model.hpp"
#include <vector>
#include <list>
#include <iterator>
#include <string>
#include <cstdio>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>

using std::advance;
using std::vector;
using std::list;
using std::string;
using boost::trim;

static void ls(fs::path const & path, list<file_info> & result);
static void rename(fs::path const & oldname, fs::path const & newname);
static void mkdir(fs::path const & dir);
static void rm(fs::path const & p);
static bool file_compare(file_info const & a, file_info const & b);
static bool parse_ls_line(string const & line, file_info & result);


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

Qt::ItemFlags directory_model::flags(QModelIndex const & index) const
{
	return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

QVariant directory_model::data(QModelIndex const & index, int role) const
{
	if (role != Qt::DisplayRole && role != Qt::EditRole)
		return QVariant{};

	if (index.column() == 0 && index.row() < _files.size())
	{
		list<file_info>::const_iterator it = _files.begin();
		advance(it, index.row());
		return QString::fromUtf8(it->name.c_str());
	}
	else
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
	change_directory(_path.parent_path());
}

bool parse_link(string const & name, string & link_name, string & link_to)
{
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
		fs::path p{_path};
		p /= text.toStdString();
		change_directory(p);
	}
	else if (file_it->link)
	{
		string name, to;
		parse_link(file_it->name, name, to);
		bool directory = (to.back() == '/');
		if (directory)
		{
			fs::path p{to};
			change_directory(p);
		}
	}
}

void directory_model::remove_item(QItemSelectionModel * selection)
{
	if (selection->model() != this)
		return;

	for (QModelIndex & index : selection->selection().indexes())
	{
		int row = index.row();

		list<file_info>::const_iterator file_it = _files.begin();
		advance(file_it, row);

		fs::path p{_path};
		p /= file_it->name;
		rm(p);

		beginRemoveRows(QModelIndex{}, row, row);

		_files.erase(file_it);

		endRemoveRows();
	}
}

void directory_model::make_directory(QString local_name)
{
	fs::path p{_path};
	p /= local_name.toStdString();
	mkdir(p);

	beginInsertRows(QModelIndex{}, _files.size(), _files.size());
	_files.emplace_back(local_name.toStdString(), true);  // TODO: ak sa mkdir nepodari, nevkladaj
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

void directory_model::change_directory(fs::path const & path)
{
	_path = path;

	beginResetModel();
	_files.clear();
	_files.emplace_back("..", true);
	ls(_path, _files);
	_files.sort(file_compare);
	endResetModel();

	emit directory_changed(QString::fromUtf8(_path.c_str()));
}

void directory_model::rename(string const & oldval, string const & newval)
{
	fs::path oldpath{_path};
	oldpath /= oldval;
	fs::path newpath{_path};
	newpath /= newval;
	::rename(oldpath, newpath);
}

void mkdir(fs::path const & dir)
{
	string cmd = string{"mkdir -p "} + dir.string();
	system(cmd.c_str());
}

void rename(fs::path const & oldname, fs::path const & newname)
{
	string cmd = string{"mv "} + oldname.string() + " " + newname.string();
	system(cmd.c_str());
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
		result.directory = type == 'd';
		result.size = std::stoul(what["size"]);
		result.link = type == 'l';
		result.executable = what["permission"].str().find('x') != string::npos;

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

void ls(fs::path const & path, list<file_info> & result)
{
	string cmd = string{"ls -l "} + path.string();
	FILE * pin = popen(cmd.c_str(), "r");
	if (!pin)
		return;

	char buf[1024];
	while (fgets(buf, sizeof buf, pin))
	{
		string line(buf);
		trim(line);
		file_info fi;
		if (parse_ls_line(line, fi))
			result.push_back(fi);
	}

	pclose(pin);
}

void rm(fs::path const & p)
{
	string cmd = string{"rm -rf "} + p.string();
	system(cmd.c_str());
}
