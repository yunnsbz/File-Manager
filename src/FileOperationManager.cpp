#include "FileOperationManager.h"


FileOperationManager::FileOperationManager(QObject *parent) : QObject(parent) {}

void FileOperationManager::executeOperation(IFileOperation *operation) {
    connect(operation, &IFileOperation::progress, this, &FileOperationManager::onProgress);
    connect(operation, &IFileOperation::error, this, &FileOperationManager::onError);
    connect(operation, &IFileOperation::finished, this, &FileOperationManager::onFinished);

    if (operation->execute()) {
        operationHistory.append(operation); // başarılıysa geri al listesine ekle
    } else {
        operation->deleteLater();
    }
}

void FileOperationManager::undoLast() {
    if (!operationHistory.isEmpty()) {
        IFileOperation *operation = operationHistory.takeLast();
        operation->undo();
        operation->deleteLater();
    }
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
