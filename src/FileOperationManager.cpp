#include "FileOperationManager.h"
#include "DeleteFileOperation.h"

#include "MoveFileOperation.h"
#include "mainwindow.hpp"

#include <QMessageBox>
#include <QTimer>
#include <QThread>
#include <QProgressDialog>
#include <QtLogging>

#include <utility>

FM_BEGIN_NAMESPACE

FileOperationManager::FileOperationManager(QObject * parent)
    :
    QObject(parent),
    m_mainWindow_(static_cast<MainWindow*>(parent))
{

};


void FileOperationManager::deleteOperation(QList<QString> srcList)
{
    auto* op = new DeleteFileOperation(std::move(srcList));

    auto* thread = new QThread(this);
    op->moveToThread(thread);

    // Progress dialog tanımla ama hemen gösterme
    auto* progressDialog = new QProgressDialog("Deleting files...", "Cancel", 0, 0);
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setMinimumDuration(500); // yarım saniye sonra işlem hala devam ediyorsa görünecek
    progressDialog->setRange(0, 100);

    // İşlem bittiğinde progressDialog'u gizle
    connect(op, &IFileOperation::finished, this, [=]() {
        if (progressDialog->isVisible()) {
            progressDialog->hide();
        }
        progressDialog->deleteLater();
    });

    // Sinyaller
    connect(thread, &QThread::started, op, &DeleteFileOperation::start);
    connect(op, &IFileOperation::progress, this, &FileOperationManager::onProgress);
    connect(op, &IFileOperation::error, this, &FileOperationManager::onError);
    connect(op, &IFileOperation::finished, this, &FileOperationManager::onFinished);
    connect(op, &IFileOperation::progress, this, [=](int val) {
            progressDialog->setValue(val);
    });

    connect(op, &IFileOperation::finished, thread, &QThread::quit);
    connect(thread, &QThread::finished, op, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    thread->start();
}

void FileOperationManager::moveOperation(QString dst){

    if(!m_copiedPaths.empty())
    {
        // kesme, kopyalama yada taşıma işlemi
        auto* op = new MoveFileOperation(m_copiedPaths, std::move(dst), m_isFilesSelectedToCut);

        auto* thread = new QThread(this);
        op->moveToThread(thread);
        connect(thread, &QThread::started, op, &MoveFileOperation::start);

        // Progress dialog tanımla ama hemen gösterme
        auto* progressDialog = new QProgressDialog("Copying Files...", "Cancel", 0, 0);
        progressDialog->setWindowModality(Qt::WindowModal);
        progressDialog->setMinimumDuration(500); // İlk başta görünmeyecek
        progressDialog->setRange(0, 100);

        // İşlem bittiğinde progressDialog'u gizle
        connect(op, &IFileOperation::finished, this, [=]() {
            if (progressDialog != nullptr) {
            if (progressDialog->isVisible()) {
                progressDialog->hide();
            }
            progressDialog->deleteLater();
            }
        });

        // Sinyaller
        connect(op, &IFileOperation::progress, this, &FileOperationManager::onProgress);
        connect(op, &IFileOperation::error, this, &FileOperationManager::onError);
        connect(op, &IFileOperation::finished, this, &FileOperationManager::onFinished);
        connect(op, &IFileOperation::progress, this, [=](int val) {
            if(progressDialog != nullptr) {
                progressDialog->setValue(val);
            }
        });

        connect(op, &IFileOperation::finished, thread, &QThread::quit);
        connect(thread, &QThread::finished, op, &QObject::deleteLater);
        connect(thread, &QThread::finished, thread, &QObject::deleteLater);

        thread->start();

        m_copiedPaths.clear();
    }
}

void FileOperationManager::addToCut(const QString& src)
{
    // eğer öncesinde copy için eklenmişlerse listeyi boşalt. aynı anda hem kesme hem de kopyalama yapılmayacak
    if(!m_isFilesSelectedToCut)
    {
        m_copiedPaths.clear();
    }

    m_isFilesSelectedToCut = true;
    m_copiedPaths.insert(src);
}

void FileOperationManager::addToCopy(const QString& src)
{
    // eğer öncesinde kesme işlemi için liste doldurulmuşsa listeyi boşalt. aynı anda hem kesme hem de kopyalama yapılmayacak
    if(m_isFilesSelectedToCut)
    {
        m_copiedPaths.clear();
    }

    m_isFilesSelectedToCut = false;
    m_copiedPaths.insert(src);
}


void FileOperationManager::onProgress(int percent) {
    qDebug() << "Progress:" << percent << "%";
}

void FileOperationManager::onError(const QString &msg) {
    qWarning() << "Error:" << msg;
}

void FileOperationManager::onFinished(const QString &info) {
    qDebug() << "Operation Finished:" << info;
}

FM_END_NAMESPACE

