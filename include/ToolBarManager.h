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

    void setBackButtonEnabled(bool enabled);
    void setForwardButtonEnabled(bool enabled);
    void setUpButtonEnabled(bool enabled);

private:
    QHBoxLayout* m_toolBar_;
    MainWindow* m_mainWindow_;
};

FM_END_NAMESPACE
#endif // TOOLBARMANAGER_H
