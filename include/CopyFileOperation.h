#ifndef COPYOPERATION_H
#define COPYOPERATION_H

#include "AbstractFileOp.h"

#include <qdir.h>

class CopyFileOperation : public AbstractFileOp {
    Q_OBJECT
public:
    void start() override {
    }
};

#endif // COPYOPERATION_H
