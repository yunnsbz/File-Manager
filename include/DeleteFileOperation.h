#ifndef DELETEFILEOPERATION_H
#define DELETEFILEOPERATION_H

#include "AbstractFileOp.h"

class DeleteFileOperation : public AbstractFileOp
{
    Q_OBJECT

public:
    void start() override;
    void addOperations(QVariantMap params) override;
    void undo(QVariantMap params) override;
    void cancel() override;

};

#endif // DELETEFILEOPERATION_H
