#ifndef FILEOPERATIONMANAGER_H
#define FILEOPERATIONMANAGER_H

#include "IFileOperation.h"

#include <QString>
#include <QFileInfo>
#include <QDir>
#include <QObject>

class FileOperationManager : public QObject {
    Q_OBJECT

public:
    explicit FileOperationManager(QObject *parent = nullptr);
    void executeOperation(IFileOperation *operation);
    void undoLast();

private:
    QVector<IFileOperation*> operationHistory;

private slots:
    void onProgress(int percent);
    void onError(const QString &msg);
    void onFinished(const QString &info);
};

#endif // FILEOPERATIONMANAGER_H
