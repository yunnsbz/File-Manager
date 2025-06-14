#include "FileOperationManager.h"
#include "DeleteFileOperation.h"

#include "mainwindow.hpp"

#include <QMessageBox>
#include <QTimer>

FileOperationManager::FileOperationManager(QObject * parent)
    :
    QObject(parent),
    mainWindow_(static_cast<MainWindow*>(parent))
{

};


void FileOperationManager::DeleteOperation(QList<QString> srcList)
{
    auto* op = new DeleteFileOperation(srcList);

    QThread* thread = new QThread(this);
    op->moveToThread(thread);

    // Progress dialog tanımla ama hemen gösterme
    auto* progressDialog = new QProgressDialog("Deleting files...", "Cancel", 0, 0);
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setMinimumDuration(0); // İlk başta görünmeyecek

    // 500 ms sonra gösterilecek ama sadece hâlâ çalışıyorsa
    QTimer::singleShot(500, this, [=]() {
        if (thread->isRunning()) {
            progressDialog->show();
        }
    });

    // İşlem bittiğinde progressDialog'u gizle
    connect(op, &IFileOperation::finished, progressDialog, &QProgressDialog::hide);
    connect(op, &IFileOperation::finished, progressDialog, &QProgressDialog::deleteLater);

    // Sinyaller
    connect(thread, &QThread::started, op, &DeleteFileOperation::start);
    connect(op, &IFileOperation::progress, this, &FileOperationManager::onProgress);
    connect(op, &IFileOperation::error, this, &FileOperationManager::onError);
    connect(op, &IFileOperation::finished, this, &FileOperationManager::onFinished);

    connect(op, &IFileOperation::finished, thread, &QThread::quit);
    connect(thread, &QThread::finished, op, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    thread->start();
}

//  FileOperationManager::PasteOperation(QString dst)
//
// QVariantMap params;
//
// QQueue<QString> copyQueue;
//
// auto op = operationMap["paste"];
//
// if (op)
// {
//     // set içindekileri queue'ye aktar:
//     for (const QString& item : copiedPaths) {
//         copyQueue.enqueue(item);
//     }
//
//     // set'i boşalt ki tekrar kullanılabilsin:
//     copiedPaths.clear();
//
//     // kopyalanmış yolların tamaını addOperation için src ile birleştirip param haline getirip sıraya ekliyoruz
//     for (int i = 0; i < copyQueue.count(); ++i)
//     {
//             params["src"] = copyQueue.dequeue();
//             params["dst"] = dst;
//
//             params["op"] = "paste";
//
//             // işlem tamamlandığında işlem geçmişine ekle:
//             connect(op, &IFileOperation::finished, this, [this, params](){
//                 operationHistory.append(params);
//             });
//
//             currentOperation = params;
//             op->addOperations(params);
//         }
//
//         // tüm yapılacaklar sıraya eklendiğinde işlemi başlat
//         op->start();
//     }
// }

// void FileOperationManager::addToCut(QString src)
// {
//     copiedPaths.insert(src);
//
//     //TODO: seçimin UI üzerinde grileşmesi gerekir.
// }
//
// void FileOperationManager::addToCopy(QString src)
// {
//     copiedPaths.insert(src);
// }
//

void FileOperationManager::onProgress(int percent) {
    qDebug() << "Progress:" << percent << "%";
}

void FileOperationManager::onError(const QString &msg) {
    qWarning() << "Error:" << msg;
}

void FileOperationManager::onFinished(const QString &info) {
    qDebug() << "Operation Finished:" << info;
}

