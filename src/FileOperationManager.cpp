#include "FileOperationManager.h"


FileOperationManager::FileOperationManager(QObject *) {

    };

void FileOperationManager::executeOperations() {

}

void FileOperationManager::undoLast() {

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
