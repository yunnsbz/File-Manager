#include "ColumnManager.h"

#include <QFileSystemModel>
#include <QDesktopServices>
#include <QUrl>

#include "FM_Macros.hpp"
FM_BEGIN_NAMESPACE

ColumnManager::ColumnManager(QColumnView* columnView, QObject *parent)
    :
    QObject(parent),
    m_columnView(columnView)
{
    // set to disk directory
    m_fileModel = new QFileSystemModel(this);
    m_fileModel->setRootPath("");
    m_fileModel->setReadOnly(false);

    m_columnView->setModel(m_fileModel);
    m_columnView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(m_columnView, &QColumnView::clicked, this, &ColumnManager::onColumnViewClicked);
}

void ColumnManager::onColumnViewClicked(const QModelIndex &index)
{
    if (!m_fileModel->hasChildren(index))
    {
        const QString filePath = m_fileModel->filePath(index);
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    }
}

FM_END_NAMESPACE
