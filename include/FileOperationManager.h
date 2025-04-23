#ifndef FILEOPERATIONMANAGER_H
#define FILEOPERATIONMANAGER_H

#include <QString>
#include <QFileInfo>
#include <QDir>
#include <QObject>

class FileOperationManager : public QObject {
    Q_OBJECT

public:
    explicit FileOperationManager(QObject *parent = nullptr);

    bool copy(const QString &sourcePath, const QString &destPath, bool overwrite = false);
    bool copyToClipboard(const QString &sourcePath, bool overwrite = false);
    bool pasteFromClipboard(const QString &destPath, bool overwrite = false);
    bool move(const QString &sourcePath, const QString &destPath, bool overwrite = false);
    bool remove(const QString &path);
    bool rename(const QString &oldPath, const QString &newPath);

signals:
    void progress(int percent);
    void error(const QString &message);
    void finished(const QString &operation);
};

#endif // FILEOPERATIONMANAGER_H
