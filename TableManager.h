#ifndef TABLEMANAGER_H
#define TABLEMANAGER_H

#include <QObject>
#include <QTableView>
#include <QFileSystemModel>

class TableManager : public QObject
{
    Q_OBJECT

public:
    explicit TableManager(QTableView* tableView, QObject* parent);

    void SetTableToDefault();
    void SetTableContent(int tabIndex);

    void navigateToFolder(int tabIndex, QModelIndex firstColumnIndex);

private:
    QTableView* tableView;


};

#endif // TABLEMANAGER_H
