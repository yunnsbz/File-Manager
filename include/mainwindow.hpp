#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "FileOperationManager.h"
#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QObject>
#include <QFileSystemModel>
#include <QWidget>
#include <QModelIndex>
#include <QEvent>
#include <QString>
#include <QList>
#include <qevent.h>

class ApplicationStateHandler;
class TabManager;
class ToolBarManager;
class TableManager;
class TreeManager;
class FileModelOperations;
class ThemeManger;

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
    explicit MainWindow(QWidget* parent = nullptr);
    explicit MainWindow(const MainWindow&)                 = delete;
    explicit MainWindow(MainWindow&&) noexcept             = delete;

    auto operator = (const MainWindow&)     -> MainWindow& = delete;
    auto operator = (MainWindow&&) noexcept -> MainWindow& = delete;
    ~MainWindow() override;


public:
    auto getUI() -> Ui::MainWindow*;

    void SetTabContent(int tabIndex, bool rightPane);
    void OnTabMoved(int toIndex, int fromIndex);
    void OnTabMoved2(int toIndex, int fromIndex);
    void updateNavButtons(int tabIndex, bool forRightPane);

    // nav buttons:
    void upperFolderOnClick(bool OnRightPane);
    void BackButtonOnClick(bool OnRightPane);
    void ForwardButtonOnClick(bool OnRightPane);
    void ScrollColumn(int direction);

    void ActivateDualPane();
    void DeactivateDualPane();
    void DeactivateTreeView();
    void ActivateTreeView();

    void ActivateColumnView();

protected:


private slots:
    void onTreeSelectionChanged(const QModelIndex &current, const QModelIndex &previous);
    void on_actionExit_triggered();
    void on_splitter_splitterMoved(int pos, int index);
    void on_actionTree_View_triggered();

    void on_toolBackButton_clicked();
    void on_toolForwardButton_clicked();
    void on_actionAbout_triggered();
    void on_lineEdit_returnPressed();
    void on_toolCmdButton_pressed();
    void on_actionDual_Pane_View_triggered();
    void on_actionColumn_View_triggered();

    void on_tabWidget_tabBarClicked(int index);
    void on_tabWidget_2_tabBarClicked(int index);
    void on_tabWidget_tabCloseRequested(int index);
    void on_tabWidget_2_tabCloseRequested(int index);
    void on_FileTreeView_clicked(const QModelIndex &index);
    void on_FileTreeView_2_clicked(const QModelIndex &index);
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_tableView_2_doubleClicked(const QModelIndex &index);
    void on_toolSearchButton_clicked();
    void on_toolUpButton_clicked();
    void on_actionOptions_triggered();
    void on_columnView_clicked(const QModelIndex &index);

    void on_toolHistoryButton_clicked();

    void on_toolCopyButton_clicked();

    void on_toolPasteButton_clicked();

    void on_toolDelButton_clicked();

private:
    virtual auto eventFilter(QObject* obj, QEvent* event) -> bool override;

    virtual void keyPressEvent(QKeyEvent* event) override; // şimdilik mainWİdget'da bazı ögelere focus için kullanılıyor


private:
    void SetLabelText_(const QString& path);


private:
    Ui::MainWindow *ui;

    UIManager m_ui_mgr_;
    FileModelOperations* fileModelOp;
    FileModelOperations* fileModelOp2;
    QFileSystemModel* columnFileModel;

    ToolBarManager* toolBarManager;
    ThemeManger* menuManager;

    TabManager* tabManager;
    TabManager* tabManager2;
    TableManager* tableManager;
    TableManager* tableManager2;
    TreeManager* treeManager;
    TreeManager* treeManager2;

    FileOperationManager* FileOpManager;

    ApplicationStateHandler* AppStateHandler;

    // history butonlarının hangi tabWidget için çalıştığını belirtmek için kullanılır.
    bool isWorkingOnRightPane = false;

    bool leftTabIsReset_  = true;
    bool rightTabIsReset_ = true;

    bool treeViewActive = true;
    bool dualPaneActive = true;
    bool ColumnViewActive = false;

    bool searchOn = false;
    bool tabCloseButtonOld = false;

};
#endif // MAINWINDOW_H
