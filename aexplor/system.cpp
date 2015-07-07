#include "system.hpp"
#include <boost/regex.hpp>
#include <boost/algorithm/string/trim.hpp>

using std::list;
using std::string;
using boost::trim;

static string escaped(fs::path const & p);

class linux_system : public abstract_system
{
public:
	void ls(fs::path const & path, list<file_info> & result) override;
	bool ls_file(fs::path const & path, file_info & result) override;
	void cp(fs::path const & src, fs::path const & dst) override;
	void rm(fs::path const & p) override;
	void mkdir(fs::path const & dir) override;
	void rename(fs::path const oldname, fs::path const & newname) override;
	string const & platform() const override;

private:
	bool parse_ls_line(string const & line, file_info & result);
};

class android_system : public abstract_system  // TODO: zdielaj kod medzi linux a androidom
{
public:
	android_system(string const & adb) : _adb{adb} {}
	void ls(fs::path const & path, list<file_info> & result) override;
	bool ls_file(fs::path const & path, file_info & result) override;
	void cp(fs::path const & src, fs::path const & dst) override;
	void rm(fs::path const & p) override;
	void mkdir(fs::path const & dir) override;
	void rename(fs::path const oldname, fs::path const & newname) override;
	string const & platform() const override;

	void mount_system_as_rw() const override;

private:
	string shell_command(string const & cmd, bool root = false);
	bool root_needed(fs::path const dirname);
	bool parse_ls_line(string const & line, file_info & result);

	string _adb;
};

abstract_system * get_system_impl()
{
//	static linux_system sys{};
	static android_system sys{"/home/ja/opt/android-sdk-linux/platform-tools/adb"};
	return &sys;
}

bool abstract_system::parse_link(string const & name, string & link_name, string & link_to)
{
	boost::regex e{R"((?'name'.+)\s->\s(?'to'.+))"};  // source -> data/source/

	boost::smatch what;
	if (boost::regex_match(name, what, e))
	{
		link_name = what["name"];
		link_to = what["to"];
		return true;
	}

	return false;
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
bool linux_system::parse_ls_line(string const & line, file_info & result)
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

void linux_system::ls(fs::path const & path, list<file_info> & result)
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

bool linux_system::ls_file(fs::path const & path, file_info & result)
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

void linux_system::cp(fs::path const & src, fs::path const & dst)
{
	string cmd = string{"cp "} + escaped(src) + " " + escaped(dst);
	system(cmd.c_str());
}

void linux_system::rm(fs::path const & p)
{
	string cmd = string{"rm -rf "} + escaped(p);
	system(cmd.c_str());
}

void linux_system::mkdir(fs::path const & dir)
{
	string cmd = string{"mkdir -p "} + escaped(dir);
	system(cmd.c_str());
}

void linux_system::rename(fs::path const oldname, fs::path const & newname)
{
	string cmd = string{"mv "} + escaped(oldname) + " " + escaped(newname);
	system(cmd.c_str());
}

string const & linux_system::platform() const
{
	static string s{"linux"};
	return s;
}

void android_system::ls(fs::path const & path, list<file_info> & result)
{
	string cmd = _adb + " shell ls -l " + escaped(path);
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

bool android_system::ls_file(fs::path const & path, file_info & result)
{
	string cmd = _adb + " shell ls -ld " + escaped(path);
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

void android_system::cp(fs::path const & src, fs::path const & dst)
{
	// TODO: push to protected directory ?
	string cmd = _adb + " push " + escaped(src) + " " + escaped(dst);
	system(cmd.c_str());
}

void android_system::rm(fs::path const & p)
{
	string cmd = shell_command(string{"rm -r "} + escaped(p), root_needed(p));
	system(cmd.c_str());
}

void android_system::mkdir(fs::path const & dir)
{
	string cmd = shell_command(string{"mkdir -p "} + escaped(dir), root_needed(dir));
	system(cmd.c_str());
}

void android_system::rename(fs::path const oldname, fs::path const & newname)
{
	string cmd = shell_command(string{"mv "} + escaped(oldname) + " " + escaped(newname), root_needed(oldname));
	system(cmd.c_str());
}

string const & android_system::platform() const
{
	static string s{"android"};
	return s;
}

void android_system::mount_system_as_rw() const
{
	string cmd = _adb + " shell echo mount -o remount,rw /system /system \\| su";
	system(cmd.c_str());
}

string android_system::shell_command(string const & cmd, bool root)
{
	if (root)
		return _adb + " shell echo " + cmd + " \\| su";  // example 'adb shell echo ls \| su'
	else
		return _adb + " shell " + cmd;
}

bool android_system::root_needed(fs::path const dirname)
{
	// TODO: use permissions instead of path

	auto it = dirname.begin();
	if (*it == fs::path{"/" })
	{
		++it;
		if (*it == fs::path{"system"})
			return true;
	}
	return false;
}

static string strip_newline(string const & s)
{
	string::const_iterator it = s.end();
	--it;
	while (*it == '\r' || *it == '\n')
	{
		if (it == s.begin())
			return string{};
		else
			--it;
	}

	++it;
	return string{s.begin(), it};
}

bool android_system::parse_ls_line(string const & line, file_info & result)
{
	// 'ls -l' output format
	// drwxr-xr-x root     root              2015-06-23 23:05 acct
	// -rw-r--r-- root     root          136 1970-01-01 01:00 default.prop
	static string type_expr = R"((?'type'.))";
	static string permission_expr = R"((?'permission'(?:[r-][w-][x-]){3}))";
	static string user_expr = R"((?'user'\w+))";
	static string group_expr = R"((?'group'\w+))";
	static string size_expr = R"((?'size'\d+))";  // [optional]
	static string date_expr = R"((?'date'\d{4}-\d{2}-\d{2} \d{2}:\d{2}))";
	static string name_expr = R"((?'name'.+))";
	static string gap = R"(\s+)";

	static string expr = type_expr + permission_expr + gap +
		user_expr + gap + group_expr + gap + "(?:" + size_expr + gap + ")?" + date_expr + gap + name_expr;

	string text = strip_newline(line);
	boost::regex e{expr};

	boost::smatch what;
	if (boost::regex_match(text, what, e))
	{
		char type = what["type"].str()[0];

		result.name = what["name"];
		result.permission = what["permission"];
		result.directory = type == 'd';
		result.link = type == 'l';
		result.executable = what["permission"].str().find('x') != string::npos;
		if (what.position("size") > 0)
			result.size = std::stoul(what["size"]);

		if (result.link)  // if it is link look for target
		{
			string name, to;
			if (parse_link(result.name, name, to))
			{
				file_info fi;
				ls_file(to, fi);
				result.directory = fi.directory;
				result.executable = fi.executable;
			}
		}

		return true;
	}
	else
		result.name = line;

	return true;  // want to see also unmatched files
}

string escaped(fs::path const & p)
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
