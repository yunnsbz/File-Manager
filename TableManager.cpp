#include "TableManager.h"



TableManager::TableManager(QTableView *tableView, QObject *parent)
:
QObject(parent),
tableView(tableView)
{

}
