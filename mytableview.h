#ifndef MYTABLEVIEW_H
#define MYTABLEVIEW_H

#include <QWidget>
#include <QList>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>
class myTableView : public QTableView
{
	Q_OBJECT

public:
	myTableView(QWidget *parent);
	~myTableView();
	void removeallitem();
	void setWidget(int row ,int col,QWidget* wid);
	void setItem(int row,int col,QString text);
	QWidget* getWidget(int row, int col);
	QStandardItem* getItem(int row, int col);

	void flushColumnWidth();
	int count();
    void setCss();
public:
    QStandardItemModel* myModel;

private:
	QList<QStandardItem*> m_item_list;
	QList<int> m_col_width;
};

#endif // MYTABLEVIEW_H
