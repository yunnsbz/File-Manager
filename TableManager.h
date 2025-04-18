#ifndef TABLEMANAGER_H
#define TABLEMANAGER_H

#include <QObject>
#include <QTableView>
#include <QFileSystemModel>

class FileModelOperations;

class TableManager : public QObject
{
    Q_OBJECT

public:
    explicit TableManager(QTableView *tableView, FileModelOperations* fileModelOp1, QObject *parent);


public:
    void SetTableToDefault();
    void SetTableContent(int tabIndex);
    void navigateToFolder(int tabIndex, QModelIndex firstColumnIndex);
    void SetColumnResize();

protected:


private:
    FileModelOperations* fileModelOp1;

    QTableView* tableView;
};

#endif // TABLEMANAGER_H
