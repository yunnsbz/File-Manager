#ifndef FILEOPERATIONVIEW_H
#define FILEOPERATIONVIEW_H

#include <QObject>

class FileOperationView : public QObject
{
    Q_OBJECT

public:
    explicit FileOperationView(QObject* parent);


};

inline FileOperationView::FileOperationView(QObject *parent)
{

}

#endif // FILEOPERATIONVIEW_H
