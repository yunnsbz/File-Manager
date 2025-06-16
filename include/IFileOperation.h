#ifndef IFILEOPERATION_H
#define IFILEOPERATION_H

#include <QObject>
#include <QString>
#include <QFileInfo>
#include <QList>

class IFileOperation : public QObject
{
    Q_OBJECT

public:
    explicit IFileOperation(QObject* parent = nullptr)
        :
        QObject(parent)
    {

    }


public:
    virtual void start() = 0;


signals:
    void progress(int percent);
    void error(const QString &message);
    void finished(const QString &operation);
};

#endif // IFILEOPERATION_H
