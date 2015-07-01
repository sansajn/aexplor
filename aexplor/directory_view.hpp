#pragma once
#include <string>
#include <vector>
#include <QListView>
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
	void item_dropped(std::vector<std::string> const & files);
	void directory_created(QString local_name);

protected:
	void keyPressEvent(QKeyEvent * event) override;
	void dropEvent(QDropEvent * event) override;
	void dragEnterEvent(QDragEnterEvent * event) override;
	void dragMoveEvent(QDragMoveEvent * event) override;

private:
	std::string current_directory() const;
	void mkdir_event();

	QPoint _start_drag_pos;
};
