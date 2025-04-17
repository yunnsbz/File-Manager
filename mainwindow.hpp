#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QFileSystemModel>
#include <QWidget>
#include <QModelIndex>
#include <QEvent>
#include <QString>
#include <QList>

class TabManager;
class ToolBarManager;
class FileModelOperations;
class TableManager;
class TreeManager;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

struct UIManager final
{
    UIManager(Ui::MainWindow*& theUi, QMainWindow* pWnd);
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    explicit MainWindow(const MainWindow&)                 = delete;
    explicit MainWindow(MainWindow&&) noexcept             = delete;
    auto operator = (const MainWindow&)     -> MainWindow& = delete;
    auto operator = (MainWindow&&) noexcept -> MainWindow& = delete;
    ~MainWindow();


public:
    auto GetCurrentTabIndex() -> int;
    auto GetPreviousTabIndex() -> int;

    auto getUI() -> Ui::MainWindow*;

    void SetTabContent(int tabIndex);
    void OnTabMoved(int toIndex, int fromIndex);
    void updateHistoryButtons(int const tabIndex);


protected:


private slots:
    void onTreeSelectionChanged(const QModelIndex &current, const QModelIndex &previous);
    void on_actionExit_triggered();
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_splitter_splitterMoved(int pos, int index);
    void on_actionList_View_triggered();
    void on_tabWidget_tabCloseRequested(int index);
    void on_tabWidget_tabBarClicked(int index);
    void on_FileTreeView_clicked(const QModelIndex &index);
    void on_toolBackButton_clicked();
    void on_toolForwardButton_clicked();
    void on_actionAbout_triggered();
    void on_lineEdit_returnPressed();


// virtual methods
private:
    virtual auto eventFilter(QObject* obj, QEvent* event) -> bool override;


private:
    void UpdateLabel_(const QString& path);


// member variables
private:
    Ui::MainWindow *ui;
    UIManager m_ui_mgr_;

    FileModelOperations* fileOperations;
    ToolBarManager* toolBarManager;
    TabManager* tabManager;
    TableManager* tableManager;
    TreeManager* treeManager;

    // treeView is collapsed or not:
    bool treeActive = true;

    // tree view expanded Indexes
    QList<QList<QModelIndex>> tabsExpandedIndexes;
};
#endif // MAINWINDOW_H
