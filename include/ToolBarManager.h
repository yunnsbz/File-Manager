#ifndef TOOLBARMANAGER_H
#define TOOLBARMANAGER_H

#include <QObject>
#include <QHBoxLayout>
#include <FM_Macros.hpp>
FM_BEGIN_NAMESPACE
class MainWindow;

class ToolBarManager : public QObject
{
    Q_OBJECT

public:
    explicit ToolBarManager(QHBoxLayout* toolBar, QObject* parent);

    void SetBackButtonEnabled(bool enabled);
    void SetForwardButtonEnabled(bool enabled);
    void SetUpButtonEnabled(bool enabled);


protected:


private:
    QHBoxLayout* toolBar_;
    MainWindow* mainWindow_;
};

FM_END_NAMESPACE
#endif // TOOLBARMANAGER_H
