#ifndef ABSTRACTFILEOP_H
#define ABSTRACTFILEOP_H

#include "IFileOperation.h"

#include <QQueue>
#include <QMap>
#include <QVariant>
#include <QString>
#include <QMutex>


class AbstractFileOp : public IFileOperation {
    Q_OBJECT
protected:
    QQueue<QVariantMap> requestQueue;
    QMutex queueMutex;

public:
    void addOperations(QVariantMap params) override {
        QMutexLocker locker(&queueMutex);
        requestQueue.enqueue(params);
    }
};

#endif // ABSTRACTFILEOP_H
