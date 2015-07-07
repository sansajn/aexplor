#include "main_window.hpp"
#include <string>
#include <cstdlib>
#include <QVBoxLayout>
#include "system.hpp"

using std::string;


main_window::main_window()
{
	QWidget * w = new QWidget{this};

	QVBoxLayout * layout = new QVBoxLayout(w);
	layout->addWidget(&_wsys_btn);

	_wsys_btn.setText("+w /system");
	_wsys_btn.setToolTip("remount /system as read,write");
	connect(&_wsys_btn, SIGNAL(clicked(bool)), this, SLOT(wsys_click_event(bool)));

	_dirw.setMinimumSize(400, 600);  // TODO: count geometry
	layout->addWidget(&_dirw);

	setCentralWidget(w);
}

void main_window::wsys_click_event(bool)
{
	get_system_impl()->mount_system_as_rw();
}
