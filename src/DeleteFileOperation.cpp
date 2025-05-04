


#include "DeleteFileOperation.h"
#include <qdir.h>

// params: src, op (all three are QString)
void DeleteFileOperation::start()
{

}

void DeleteFileOperation::addOperations(QVariantMap params)
{
    AbstractFileOp::addOperations(params);

    // ek birşey yapılması gerekirse...
}

void DeleteFileOperation::undo(QVariantMap )
{

}

void DeleteFileOperation::cancel()
{

}
