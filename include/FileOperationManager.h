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

    // execute all operations on the queue.
    // after adding all the operations you should call this.
    void executeOperations();

    // undo last operation queue
    void undoLast();


    void addMoveOperation(QString src,QString dst);
    void addCutOperation(QString src);
    void addPasteOperation(QString dst);
    void addCopyOperation(QString src);
    void addRenameOperation(QString src,QString name);
    void addNewFolderOperation(QString dst);



private:
    QList<QVariantMap> operationHistory;
    QMap<QString, IFileOperation*> OperationMap;
    QVariantMap currentOperation;


private slots:
    void onProgress(int percent);
    void onError(const QString &msg);
    void onFinished(const QString &info);
};

#endif // FILEOPERATIONMANAGER_H
