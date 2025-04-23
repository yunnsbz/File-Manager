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
    void executeCommand(IFileOperation *cmd);
    void undoLast();

private:
    QVector<IFileOperation*> commandHistory;

private slots:
    void onProgress(int percent);
    void onError(const QString &msg);
    void onFinished(const QString &info);
};

#endif // FILEOPERATIONMANAGER_H
