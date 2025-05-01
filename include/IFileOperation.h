#ifndef IFILEOPERATION_H
#define IFILEOPERATION_H



#include <QObject>


#define PURE = 0

class IFileOperation : public QObject {
    Q_OBJECT

public:
    explicit IFileOperation(QObject *parent = nullptr) : QObject(parent) {}

    virtual void start() PURE;
    virtual void addOperations(QVariantMap params) PURE;
    virtual void undo(QVariantMap params) PURE;
    virtual void cancel() PURE;

signals:
    void progress(int percent);
    void error(const QString &message);
    void finished(const QString &operation);
};

#endif // IFILEOPERATION_H
