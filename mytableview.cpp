#include "mytableview.h"

myTableView::myTableView(QWidget *parent)
	: QTableView(parent), myModel(new QStandardItemModel())
{
    //this->setAttribute(Qt::WA_DeleteOnClose);
    this->setModel(myModel);
}
void myTableView::removeallitem()
{
	for (int i = 0; i < myModel->rowCount(); i++){
		myModel->removeRow(0);
	}
	 m_item_list.clear();
}
void myTableView::setWidget(int row, int col, QWidget* wid)
{
	QStandardItem* itemTemp = new QStandardItem();
	myModel->setItem(row, col, itemTemp);
	this->setIndexWidget(itemTemp->index(), wid);
	m_item_list.push_back(itemTemp);
}
void myTableView::setItem(int row, int col, QString text)
{
	QStandardItem* itemTemp = new QStandardItem(text);
	myModel->setItem(row, col, itemTemp);
	m_item_list.push_back(itemTemp);
}

QWidget* myTableView::getWidget(int row, int col)
{
	return this->indexWidget(this->myModel->index(row,col));
}
QStandardItem * myTableView::getItem(int row, int col)
{
	return myModel->item(row,col)? myModel->item(row, col):new QStandardItem();
}


void myTableView::flushColumnWidth()
{
	//����ÿһ�еĿ���
	for (int i = 0; i < m_col_width.size(); i++) {
		this->setColumnWidth(i, m_col_width[i]);
	}
}

int myTableView::count()
{
    return m_item_list.size();
}


void myTableView::setCss()
{
    this->setStyleSheet("QTableView#tableView{"
                        "background-color:#d0e3ff;"
                        "border-radius:10;"
                        "color:#000000;"
                        "gridline-color: rgb(111, 156, 207);"
                        "font:14pt Adobe Arabic}"
                   );
}
myTableView::~myTableView()
{

}
