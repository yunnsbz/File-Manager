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
    auto getLastLeftTabIndex() const -> int;

    // setters:
    void setLastLeftTabIndex(int value);


public slots:
    void onTabMoved(int toIndex, int fromIndex);
    void addNewTab();


protected:


private:
    auto createTabFromSplitter(QSplitter* splitter) -> QWidget*;

    void Setup_();


private:
    MainWindow* mainWindow;
    QTabWidget* tabWidget;
    QFileSystemModel* fileModel{};

    // last opened tabs (when moving to another tab system should know the last one)
    int lastRightTabIndex = 0;
    int lastLeftTabIndex = 0;
};


#endif // TABMANAGER_H
