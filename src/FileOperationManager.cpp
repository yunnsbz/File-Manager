#include "FileOperationManager.h"


FileOperationManager::FileOperationManager(QObject *parent) : QObject(parent) {}

void FileOperationManager::executeCommand(IFileOperation *cmd) {
    connect(cmd, &IFileOperation::progress, this, &FileOperationManager::onProgress);
    connect(cmd, &IFileOperation::error, this, &FileOperationManager::onError);
    connect(cmd, &IFileOperation::finished, this, &FileOperationManager::onFinished);

    if (cmd->execute()) {
        commandHistory.append(cmd); // başarılıysa geri al listesine ekle
    } else {
        cmd->deleteLater();
    }
}

void FileOperationManager::undoLast() {
    if (!commandHistory.isEmpty()) {
        IFileOperation *cmd = commandHistory.takeLast();
        cmd->undo();
        cmd->deleteLater();
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
