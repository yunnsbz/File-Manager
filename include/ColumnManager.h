#ifndef COLUMNMANAGER_H
#define COLUMNMANAGER_H

#include <QObject>
#include <QColumnView>
#include <QFileSystemModel>
#include <FM_Macros.hpp>
FM_BEGIN_NAMESPACE

class ColumnManager : public QObject
{
    Q_OBJECT

public:
    explicit ColumnManager(QColumnView* columnView, QObject *parent);

private slots:
    void onColumnViewClicked(const QModelIndex &index);

private:
    QColumnView* m_columnView;
    QFileSystemModel* m_fileModel;

};

FM_END_NAMESPACE
#endif // COLUMNMANAGER_H
