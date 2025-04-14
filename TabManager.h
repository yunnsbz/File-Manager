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

class TabManager : public QObject
{
    Q_OBJECT

public:
    explicit TabManager(QTabWidget* tabWidget, QFileSystemModel* fileModel, QObject* parent);

    void setup();

    void moveTabWidget(int index);


    void setFileIndexMap(QTableView* tableView);
    void onTabCloseRequested(int index);
    void onTabClicked(int index);
    QModelIndex getTabModelIndex(int tabIndex) const;

    void RemoveTabContent(int tabIndex);

    // setters:
    void setLastLeftTabIndex(int value){lastLeftTabIndex = value;}

    // getters:
    int getLastLeftTabIndex(){return lastLeftTabIndex;}

public slots:
    void onTabMoved(int fromIndex, int toIndex);
    void addNewTab();

private:
    QTabWidget* tabWidget;
    QFileSystemModel* fileModel;
    MainWindow* mainWindow;

    QMap<int, QModelIndex> tabContentMap;

    // last opened tabs (when moving to another tab system should know the last one)
    int lastRightTabIndex = 0;
    int lastLeftTabIndex = 0;

    QWidget* createTabFromSplitter(QSplitter* splitter);
};


#endif // TABMANAGER_H
