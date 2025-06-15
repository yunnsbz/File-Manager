#ifndef MOVEFILEOPERATION_H
#define MOVEFILEOPERATION_H

#include "IFileOperation.h"

#include <QProgressDialog>
#include <QThread>


class MoveFileOperation : public IFileOperation
{
    Q_OBJECT

public:

    // kesme yada kopyalama işlemleri burada yapılır.
    MoveFileOperation(QSet<QString> SourcePaths, QString destinationPath, bool shouldRemove, QObject* parent = nullptr);

    virtual void start() override;

    bool copyDirectoryRecursively(const QString &srcPath, const QString &destPath);

protected:


private:
    QSet<QString> m_op_files_src;
    QString m_target_dir_;
    bool m_op_should_remove_;
};

#endif // MOVEFILEOPERATION_H
