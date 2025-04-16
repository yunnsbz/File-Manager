#ifndef TABLEMANAGER_H
#define TABLEMANAGER_H

#include <qobject.h>
#include <QTableView>

class TableManager : public QObject
{
    Q_OBJECT

public:
    explicit TableManager(QTableView* tableView, QObject* parent);

private:
    QTableView* tableView;


};

#endif // TABLEMANAGER_H
