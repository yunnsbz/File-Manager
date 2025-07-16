#ifndef TABLEMANAGER_H
#define TABLEMANAGER_H

#include <QObject>
#include <QTableView>
#include <QTabWidget>

#include <FM_Macros.hpp>
FM_BEGIN_NAMESPACE

class FileModelOperations;

class TableManager : public QObject
{
    Q_OBJECT

public:
    explicit TableManager(QTabWidget* tabWidget, QTableView* tableView, FileModelOperations* fileModelOp, QObject* parent);

signals:
    void tableDoubleClicked(const QModelIndex &modelIndex);

private slots:
    void onTableDoubleClicked(const QModelIndex &modelIndex);
    void onContextMenuRequested(const QPoint &pos);

public:
    void setTableToDefault();
    void setTableContent(int tabIndex);
    void navigateToFolder(int tabIndex, QModelIndex firstColumnIndex);
    void setColumnResize();
    void openMediaWindow(QString);

    void setRootIndex(QModelIndex modelIndex);

private:
    QTabWidget* m_tabWidget;

    FileModelOperations* m_fileModelOp_;

    QTableView* m_tableView_;

};

FM_END_NAMESPACE

#endif // TABLEMANAGER_H
