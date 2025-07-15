#include "FileOperationView.h"
#include "FileModelOperations.h"
#include "FileOperationManager.h"
#include "mainwindow.hpp"

#include <QToolButton>
#include <QMessageBox>
#include <QModelIndexList>
#include <QString>
#include <QList>
#include <QtLogging>

FM_BEGIN_NAMESPACE

FileOperationView::FileOperationView(QObject *parent)
    :
    QObject(parent),
    m_mainWindow(static_cast<MainWindow*>(parent))
{
    connect(m_mainWindow->getCopyButton(),&QToolButton::clicked, this, &FileOperationView::onCopyButtonClicked);
    connect(m_mainWindow->getCutButton(),&QToolButton::clicked, this, &FileOperationView::onCutButtonClicked);
    connect(m_mainWindow->getDeleteButton(),&QToolButton::clicked, this, &FileOperationView::onDelButtonClicked);
    connect(m_mainWindow->getPasteButton(),&QToolButton::clicked, this, &FileOperationView::onPasteButtonClicked);
    connect(m_mainWindow->getRenameButton(),&QToolButton::clicked, this, &FileOperationView::onRenameButtonClicked);
}

void FileOperationView::onCopyButtonClicked()
{
    // seçilmiş satırların indexlerini al
    QModelIndexList selectedIndexes = m_mainWindow->getTableViewLeft()->selectionModel()->selectedRows();
    if(selectedIndexes.count() != 0)
    {
        for (const QModelIndex &index : selectedIndexes) {
            const QString filePath = m_mainWindow->getFileModelOpLeft()->GetFileModel()->filePath(index);
            qDebug()<< "path added to copy (left table):" << filePath;
            m_mainWindow->getFileOpManager()->addToCopy(filePath);
        }
    }
    else
    {
        // ilk tabloda seçilmemişse
        selectedIndexes = m_mainWindow->getTableViewRight()->selectionModel()->selectedRows();
        for (const QModelIndex &index : selectedIndexes) {
            const QString filePath = m_mainWindow->getFileModelOpRight()->GetFileModel()->filePath(index);
            qDebug()<< "path added to copy (right table): " << filePath;
            m_mainWindow->getFileOpManager()->addToCopy(filePath);
        }
    }
}

void FileOperationView::onDelButtonClicked()
{
    QList<QString> srcList;

    const QModelIndexList selectedIndexes = (! m_mainWindow->isWorkingOnRightPane())
            ? m_mainWindow->getTableViewLeft()->selectionModel()->selectedRows()
            : m_mainWindow->getTableViewRight()->selectionModel()->selectedRows()
            ;

    if ( ! selectedIndexes.empty())
    {
        // seçili dosyaları al (sağ yada sol pane)
        for (const QModelIndex& index : selectedIndexes) {
        if(m_mainWindow->isWorkingOnRightPane())
            srcList.append(m_mainWindow->getFileModelOpRight()->GetFileModel()->filePath(index));
        else
            srcList.append(m_mainWindow->getFileModelOpLeft()->GetFileModel()->filePath(index));
        }

        const QString text = QString("Seçili %1 dosyayı silmek üzeresiniz. Emin misiniz?")
                               .arg(srcList.size());

        QMessageBox::StandardButton const reply = QMessageBox::question(
                m_mainWindow,
                "Dosyaları Sil",
                text,
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::No
            );

        if(reply == QMessageBox::Yes){
            m_mainWindow->getFileOpManager()->DeleteOperation(srcList);
        }
    }
}

void FileOperationView::onRenameButtonClicked()
{
    QModelIndexList selectedIndexes = m_mainWindow->getTableViewLeft()->selectionModel()->selectedRows();
    if(selectedIndexes.count() != 0)
    {
        // tek bir dosya ismi değiştirilecekse yerinde değişim yapılabilir.
        if(selectedIndexes.count() == 1)
        {
            const QModelIndex index = m_mainWindow->getTableViewLeft()->currentIndex();
            if (index.isValid())
            {
                m_mainWindow->getTableViewLeft()->edit(index);
            }
        }
        else if(selectedIndexes.count() > 1)
        {
            // TODO(yunnsbz): todo.
        }
    }
    else
    {
        // ilk tabloda seçilmemişse
        selectedIndexes = m_mainWindow->getTableViewRight()->selectionModel()->selectedRows();
        // tek bir dosya ismi değiştirilecekse yerinde değişim yapılabilir.
        if(selectedIndexes.count() == 1)
        {
            const QModelIndex index = m_mainWindow->getTableViewRight()->currentIndex();
            if (index.isValid())
            {
                m_mainWindow->getTableViewRight()->edit(index);
            }
        }
        else if(selectedIndexes.count() > 1)
        {
            // TODO(yunnsbz): todo.
        }
    }
}

void FileOperationView::onCutButtonClicked()
{
    QModelIndexList selectedIndexes = m_mainWindow->getTableViewLeft()->selectionModel()->selectedRows();
    if(selectedIndexes.count() != 0)
    {
        for (const QModelIndex &index : selectedIndexes) {
            const QString filePath = m_mainWindow->getFileModelOpLeft()->GetFileModel()->filePath(index);
            qDebug()<< "path added to cut (left table):" << filePath;
            m_mainWindow->getFileOpManager()->addToCut(filePath);
        }
    }
    else
    {
        // ilk tabloda seçilmemişse
        selectedIndexes = m_mainWindow->getTableViewRight()->selectionModel()->selectedRows();
        for (const QModelIndex &index : selectedIndexes) {
            const QString filePath = m_mainWindow->getFileModelOpRight()->GetFileModel()->filePath(index);
            qDebug()<< "path added to cut (right table): " << filePath;
            m_mainWindow->getFileOpManager()->addToCut(filePath);
        }
    }

    // TODO(yunnsbz): şeçilenler grileştirilmeli (kesmek için işaretlenmişler)
}

void FileOperationView::onPasteButtonClicked()
{
    if(m_mainWindow->isWorkingOnRightPane()){
        m_mainWindow->getFileOpManager()->MoveOperation(
                    m_mainWindow->getFileModelOpRight()->GetCurrentPath(
                        m_mainWindow->getTabWidgetRight()->currentIndex()));
    }
    else{
        m_mainWindow->getFileOpManager()->MoveOperation(
                    m_mainWindow->getFileModelOpLeft()->GetCurrentPath(
                        m_mainWindow->getTabWidgetLeft()->currentIndex()));
    }
}

FM_END_NAMESPACE
