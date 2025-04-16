#ifndef TREEMANAGER_H
#define TREEMANAGER_H

#include <qobject.h>
#include <qtreeview.h>

class TreeManager : public QObject
{
    Q_OBJECT

public:
    explicit TreeManager(QTreeView* tableView, QObject* parent);

private:
    QTreeView* tableView;

};

#endif // TREEMANAGER_H
