#ifndef FILEOPERATIONVIEW_H
#define FILEOPERATIONVIEW_H

#include <QObject>

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

#endif // FILEOPERATIONVIEW_H
