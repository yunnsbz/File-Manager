#ifndef DELETEFILEOPERATION_H
#define DELETEFILEOPERATION_H

#include "IFileOperation.h"

#include <QObject>
#include <QList>
#include <QString>
#include <FM_Macros.hpp>
FM_BEGIN_NAMESPACE

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

FM_END_NAMESPACE
#endif // DELETEFILEOPERATION_H
