#pragma once
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
	void item_opened(QModelIndex idx);

private:
	void on_return();
	void keyPressEvent(QKeyEvent * event) override;
};
