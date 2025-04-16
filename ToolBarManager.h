#ifndef TOOLBARMANAGER_H
#define TOOLBARMANAGER_H

#include <QObject>
#include <QHBoxLayout>

class MainWindow;

class ToolBarManager : public QObject
{
    Q_OBJECT

public:
    explicit ToolBarManager(QHBoxLayout* toolBar, QObject* parent);

    void SetBackButtonEnabled(bool enabled);
    void SetForwardButtonEnabled(bool enabled);


protected:


private:
    QHBoxLayout* toolBar;
    MainWindow* mainWindow;
};


#endif // TOOLBARMANAGER_H
