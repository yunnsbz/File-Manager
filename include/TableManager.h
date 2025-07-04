#ifndef TABLEMANAGER_H
#define TABLEMANAGER_H

#include <QObject>
#include <QTableView>

class FileModelOperations;

class TableManager : public QObject
{
    Q_OBJECT

public:
    explicit TableManager(QTableView* tableView, FileModelOperations* fileModelOp, QObject* parent);


public:
    void SetTableToDefault();
    void SetTableContent(int tabIndex);
    void navigateToFolder(int tabIndex, QModelIndex firstColumnIndex);
    void SetColumnResize();
    void openMediaWindow(const QString);

protected:


private:
    FileModelOperations* fileModelOp_;

    QTableView* tableView_;
};

#endif // TABLEMANAGER_H
