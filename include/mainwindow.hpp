#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

#include <QMainWindow>
#include <QObject>
#include <QFileSystemModel>
#include <QWidget>
#include <QModelIndex>
#include <QEvent>
#include <QString>
#include <QKeyEvent>
#include <QList>
#include <QToolButton>
#include <QTabWidget>
#include <QTabBar>
#include <QTreeView>
#include <QTableView>
#include <QStackedWidget>

#include <FM_Macros.hpp>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

FM_BEGIN_NAMESPACE

class ApplicationStateHandler;
class FileOperationManager;
class TabManager;
class ToolBarManager;
class TableManager;
class TreeManager;
class FileModelOperations;
class ThemeManger;
class SettingsDialog;
class FileOperationView;
class MenuBarView;
class EventHandler;

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

    /**
     * m_isWorkingOnRightPane değeri doğru bir şekilde set edilmeden çağırılmamalıdır.
     */
    void updateNavButtons(const int tabIndex);

    // nav buttons:
    void upperFolderOnClick(bool OnRightPane);
    void BackButtonOnClick(bool OnRightPane);
    void ForwardButtonOnClick(bool OnRightPane);
    void ScrollColumn(int direction);

    void ActivateDualPane();
    void ActivateTreeView();
    void ActivateColumnView();
    void DeactivateDualPane();
    void DeactivateTreeView();
    void DeactivateColumnView();

    [[nodiscard]] auto getTabWidgetLeft()   const -> QTabWidget*    { return ui->tabWidgetLeft; }
    [[nodiscard]] auto getTabWidgetRight()  const -> QTabWidget*    { return ui->tabWidgetRight; }
    [[nodiscard]] auto getTabBarLeft()      const -> QTabBar*       { return ui->tabWidgetLeft->tabBar(); }
    [[nodiscard]] auto getTabBarRight()     const -> QTabBar*       { return ui->tabWidgetRight->tabBar(); }
    [[nodiscard]] auto getTreeViewLeft()    const -> QTreeView*     { return ui->fileTreeViewLeft; }
    [[nodiscard]] auto getTreeViewRight()   const -> QTreeView*     { return ui->fileTreeViewRight; }
    [[nodiscard]] auto getTableViewLeft()   const -> QTableView*    { return ui->tableViewLeft; }
    [[nodiscard]] auto getTableViewRight()  const -> QTableView*    { return ui->tableViewRight; }
    [[nodiscard]] auto getStackedWidget()   const -> QStackedWidget*{ return ui->stackedWidget; }

    [[nodiscard]] auto getPasteButton()     const -> QToolButton*   { return ui->toolPasteButton; }
    [[nodiscard]] auto getCutButton()       const -> QToolButton*   { return ui->toolCutButton; }
    [[nodiscard]] auto getCopyButton()      const -> QToolButton*   { return ui->toolCopyButton; }
    [[nodiscard]] auto getDeleteButton()    const -> QToolButton*   { return ui->toolDelButton; }
    [[nodiscard]] auto getRenameButton()    const -> QToolButton*   { return ui->toolRenameButton; }

    [[nodiscard]] auto getActionAbout()         const -> QAction*   { return ui->actionAbout; }
    [[nodiscard]] auto getActionColumnView()    const -> QAction*   { return ui->actionColumn_View; }
    [[nodiscard]] auto getActionDualPaneView()  const -> QAction*   { return ui->actionDual_Pane_View; }
    [[nodiscard]] auto getActionTreeView()      const -> QAction*   { return ui->actionTree_View; }
    [[nodiscard]] auto getActionExit()          const -> QAction*   { return ui->actionExit; }
    [[nodiscard]] auto getActionSettings()      const -> QAction*   { return ui->actionSettings; }

    [[nodiscard]] auto isWorkingOnRightPane()   const -> bool       { return m_isWorkingOnRightPane; }
    [[nodiscard]] auto isTreeViewActive()       const -> bool       { return m_treeViewActive; }
    [[nodiscard]] auto isDualPaneActive()       const -> bool       { return m_dualPaneActive; }
    [[nodiscard]] auto isColumnViewActive()     const -> bool       { return m_columnViewActive; }

    [[nodiscard]] auto getFileModelOpLeft()     const -> FileModelOperations*   { return m_fileModelOpLeft; }
    [[nodiscard]] auto getFileModelOpRight()    const -> FileModelOperations*   { return m_fileModelOpRight; }
    [[nodiscard]] auto getFileOpManager()       const -> FileOperationManager*  { return m_fileOpManager; }
    [[nodiscard]] auto getSettingsWindow()      const -> SettingsDialog*        { return m_settingsDialog; }

private:
    struct UIManager final
    {
        UIManager(Ui::MainWindow*& theUi, QMainWindow* pWnd);
    };

private slots:
    void onTreeLeftSelectionChanged(const QModelIndex &current, const QModelIndex &previous);
    void onTreeRightSelectionChanged(const QModelIndex &current, const QModelIndex &previous);

    // tree
    void on_fileTreeViewLeft_clicked(const QModelIndex &index);
    void on_fileTreeViewRight_clicked(const QModelIndex &index);

    // column view
    void on_columnView_clicked(const QModelIndex &index);

    // spliter
    void on_splitterLeft_splitterMoved(int pos, int index);

    // tab bar
    void on_tabWidgetLeft_tabBarClicked(int index);
    void on_tabWidgetRight_tabBarClicked(int index);
    void on_tabWidgetLeft_tabCloseRequested(int index);
    void on_tabWidgetRight_tabCloseRequested(int index);

    // table view
    void on_tableViewLeft_doubleClicked(const QModelIndex &index);
    void on_tableViewRight_doubleClicked(const QModelIndex &index);

    // nav buttons
    void on_toolUpButton_clicked();
    void on_toolBackButton_clicked();
    void on_toolForwardButton_clicked();

    // other buttons
    void on_lineEdit_returnPressed();
    void on_toolSearchButton_clicked();
    void on_toolHistoryButton_clicked();
    void on_toolCmdButton_pressed();

private:
    void SetLabelText_(QString path);


private:
    Ui::MainWindow* ui;

    UIManager m_ui_mgr_;

    FileModelOperations* m_fileModelOpLeft;
    FileModelOperations* m_fileModelOpRight;
    QFileSystemModel* m_columnFileModel;

    ToolBarManager* m_toolBarManager;
    ThemeManger* m_menuManager;

    TabManager* m_tabManagerLeft;
    TabManager* m_tabManagerRight;
    TableManager* m_tableManagerLeft;
    TableManager* m_tableManagerRight;
    TreeManager* m_treeManagerLeft;
    TreeManager* m_treeManagerRight;

    FileOperationManager* m_fileOpManager;

    ApplicationStateHandler* m_appStateHandler;
    SettingsDialog* m_settingsDialog;

    FileOperationView* m_fileOperationView;
    MenuBarView* m_menuBarView;

    EventHandler* m_eventHandler;

    // history butonlarının hangi tabWidget için çalıştığını belirtmek için kullanılır.
    bool m_isWorkingOnLeftPane {};
    bool m_isWorkingOnRightPane {};

    bool m_treeViewActive   = true;
    bool m_dualPaneActive   = true;
    bool m_columnViewActive = false;

    bool m_searchOn {};
};

FM_END_NAMESPACE

#endif // MAINWINDOW_H
