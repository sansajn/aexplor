#pragma once
#include <string>
#include <QListView>
#include <QKeyEvent>
#include <QModelIndex>

class directory_view : public QListView
{
	Q_OBJECT

public:
	using base = QListView;

	directory_view();

signals:
	void go_up();
	void item_opened(QModelIndex idx);
	void item_removed(QItemSelectionModel * selection);
	void directory_created(QString local_name);

private:
	std::string current_directory() const;
	void mkdir_event();
	void keyPressEvent(QKeyEvent * event) override;
};
