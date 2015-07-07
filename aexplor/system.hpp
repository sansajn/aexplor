#pragma once
#include <list>
#include <string>
#include <boost/filesystem/path.hpp>

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

class abstract_system
{
public:
	virtual ~abstract_system() {}

	virtual void ls(fs::path const & path, std::list<file_info> & result) = 0;
	virtual bool ls_file(fs::path const & path, file_info & result) = 0;
	virtual void cp(fs::path const & src, fs::path const & dst) = 0;
	virtual void rm(fs::path const & p) = 0;
	virtual void mkdir(fs::path const & dir) = 0;
	virtual void rename(fs::path const oldname, fs::path const & newname) = 0;
	virtual std::string const & platform() const = 0;

	virtual void mount_system_as_rw() const {}

	virtual bool parse_link(std::string const & name, std::string & link_name, std::string & link_to);
};

abstract_system * get_system_impl();
