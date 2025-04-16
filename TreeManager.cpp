
#include "TreeManager.h"

TreeManager::TreeManager(QTreeView *tableView, QObject *parent)
    :
    QObject(parent),
    tableView(tableView)
{

}
