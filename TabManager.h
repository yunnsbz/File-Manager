#ifndef TABMANAGER_H
#define TABMANAGER_H

#include <QObject>
#include <QTabWidget>
#include <QMap>
#include <QModelIndex>
#include <QSplitter>
#include <QTableView>
#include <QTreeView>

class QFileSystemModel;
class MainWindow;
class ToolBarManager;

class TabManager : public QObject
{
    Q_OBJECT

public:
    explicit TabManager(QTabWidget* tabWidget, QObject* parent);

    void moveTabWidget(int index);
    void onTabCloseRequested(int index);
    void onTabClicked(int index);

    // getters:
    auto GetSplitter();
    int getLastLeftTabIndex(){return lastLeftTabIndex;}

    // setters:
    void setLastLeftTabIndex(int value){lastLeftTabIndex = value;}


public slots:
    void onTabMoved(int toIndex, int fromIndex);
    void addNewTab();


protected:


private:
    QWidget* createTabFromSplitter(QSplitter* splitter);

    void Setup_();


private:
    QTabWidget* tabWidget;
    QFileSystemModel* fileModel;
    MainWindow* mainWindow;
    ToolBarManager* toolBarManager;

    // last opened tabs (when moving to another tab system should know the last one)
    int lastRightTabIndex = 0;
    int lastLeftTabIndex = 0;
};


#endif // TABMANAGER_H
