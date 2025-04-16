#ifndef TABMANAGER_H
#define TABMANAGER_H

#include <QObject>
#include <QTabWidget>
#include <QMap>
#include <QModelIndex>
#include <QSplitter>
#include <QTableView>
#include <QTreeView>

struct TabContent
{
    QModelIndex ModelIndex;

    // for tree view
    QSet<QString> ExpandedPaths;

    TabContent() = default;
    TabContent(QModelIndex ModelIndex, QSet<QString> ExpandedPaths)
    {
        this->ModelIndex = ModelIndex;
        this->ExpandedPaths = ExpandedPaths;
    }
};


class QFileSystemModel;
class MainWindow;
class ToolBarManager;

class TabManager : public QObject
{
    Q_OBJECT

public:
    explicit TabManager(QTabWidget* tabWidget, QFileSystemModel* fileModel, ToolBarManager* toolBarManager, QObject* parent);

    void moveTabWidget(int index);
    void setFileIndexMap(QTableView* tableView);
    void onTabCloseRequested(int index);
    void onTabClicked(int index);

    void RemoveTabContent(int tabIndex);
    void onBackButtonClicked();
    void onForwardButtonClicked();


    // getters:
    auto getTabModelIndex(int tabIndex) const -> QModelIndex;
    int getLastLeftTabIndex(){return lastLeftTabIndex;}
    auto getTreeExpandedPaths(int tabIndex) const -> QSet<QString>;

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

    QMap<int, TabContent> tabContentMap;

    QList<TabContent> BackHistoryTabContent;
    QList<TabContent> ForwardHistoryTabContent;

    // last opened tabs (when moving to another tab system should know the last one)
    int lastRightTabIndex = 0;
    int lastLeftTabIndex = 0;
};


#endif // TABMANAGER_H
