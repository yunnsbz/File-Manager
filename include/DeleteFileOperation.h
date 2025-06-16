#ifndef DELETEFILEOPERATION_H
#define DELETEFILEOPERATION_H

#include "IFileOperation.h"

#include <QProgressDialog>
#include <QThread>

class DeleteFileOperation : public IFileOperation
{
    Q_OBJECT

public:
    DeleteFileOperation(QList<QString> files, QObject* parent = nullptr);

    virtual void start() override;


protected:


private:
    QList<QString> m_op_files_;
};

#endif // DELETEFILEOPERATION_H
