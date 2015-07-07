#include "main_window.hpp"
#include <string>
#include <cstdlib>
#include <QVBoxLayout>

using std::string;


main_window::main_window()
{
	QWidget * w = new QWidget{this};

	QVBoxLayout * layout = new QVBoxLayout(w);
	layout->addWidget(&_wsys_btn);

	_wsys_btn.setText("+w /system");
	connect(&_wsys_btn, SIGNAL(clicked(bool)), this, SLOT(wsys_event(bool)));

	_dirw.setMinimumSize(400, 600);  // TODO: count geometry
	layout->addWidget(&_dirw);

	setCentralWidget(w);
}

void main_window::wsys_event(bool)
{
	mount_system_as_rw();  // TODO: je prostrednik nutny ?
}

void main_window::mount_system_as_rw()
{
	static string adb = "/home/ja/opt/android-sdk-linux/platform-tools/adb";
	string cmd = adb + R"( shell echo mount -o remount,rw /system /system \| su)";
	system(cmd.c_str());
}
