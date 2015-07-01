#include "directory_widget.hpp"
#include <string>
#include <QVBoxLayout>
#include "directory_view.hpp"

using std::string;

string const adb_path = "/home/ja/opt/android-sdk-linux/platform-tools";

directory_widget::directory_widget(string const & root, QWidget * parent)
	: QWidget(parent)
{
	QVBoxLayout * layout = new QVBoxLayout(this);  // TODO: ako je to zo zivonostou ?

	_view = new directory_view(root, adb_path + "/adb shell");
	_path = new QLabel(QString::fromUtf8(root.c_str()));

	layout->addWidget(_path);
	layout->addWidget(_view);

	connect(_view, SIGNAL(directory_changed(QString)), _path, SLOT(setText(QString)));

	_mkdir_btn = new QPushButton{"mkdir <f7>"};
	_del_btn = new QPushButton{"delete <del>"};
	_rename_btn = new QPushButton{"rename <f2>"};
	_view_btn = new QPushButton{"view <f3>"};

	QHBoxLayout * bottom_layout = new QHBoxLayout;
	bottom_layout->addWidget(_mkdir_btn);
	bottom_layout->addWidget(_del_btn);
	bottom_layout->addWidget(_rename_btn);
	bottom_layout->addWidget(_view_btn);

	layout->addLayout(bottom_layout);
}
