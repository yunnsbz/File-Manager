#ifndef PASTEFILEOPERATION_H
#define PASTEFILEOPERATION_H

#include "AbstractFileOp.h"

class PasteFileOperation : public AbstractFileOp
{
    Q_OBJECT

public:
    void start() override;
    void addOperations(QVariantMap params) override;
    void undo(QVariantMap params) override;
    void cancel() override;

};

#endif // PASTEFILEOPERATION_H
