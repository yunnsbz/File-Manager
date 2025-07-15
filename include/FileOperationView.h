#ifndef FILEOPERATIONVIEW_H
#define FILEOPERATIONVIEW_H

#include <QObject>

#include <FM_Macros.hpp>
FM_BEGIN_NAMESPACE

class MainWindow;

class FileOperationView : public QObject
{
    Q_OBJECT

public:
    explicit FileOperationView(QObject* parent);


private:
    void onCopyButtonClicked();
    void onPasteButtonClicked();
    void onDelButtonClicked();
    void onCutButtonClicked();
    void onRenameButtonClicked();

private:
    MainWindow* m_mainWindow;

};

FM_END_NAMESPACE
#endif // FILEOPERATIONVIEW_H
