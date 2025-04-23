#ifndef IFILEOPERATION_H
#define IFILEOPERATION_H

#include <QObject>

class IFileOperation : public QObject {
    Q_OBJECT

public:
    explicit IFileOperation(QObject *parent = nullptr) : QObject(parent) {}

    virtual bool execute() = 0;
    virtual bool undo() = 0;

signals:
    void progress(int percent);
    void error(const QString &message);
    void finished(const QString &operation);
};

#endif // IFILEOPERATION_H
