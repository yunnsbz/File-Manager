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

public:
    void SetTableToDefault();
    void SetTableContent(int tabIndex);
    void navigateToFolder(int tabIndex, QModelIndex firstColumnIndex);
    void SetColumnResize();
    void openMediaWindow(QString);

    void setRootIndex(QModelIndex modelIndex);

private:
    QTabWidget* m_tabWidget;

    FileModelOperations* fileModelOp_;

    QTableView* tableView_;

};

FM_END_NAMESPACE

#endif // TABLEMANAGER_H
