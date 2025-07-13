#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

#include "FileOperationManager.h"

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

class ApplicationStateHandler;
class TabManager;
class ToolBarManager;
class TableManager;
class TreeManager;
class FileModelOperations;
class ThemeManger;
class SettingsDialog;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

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
    void ActivateTreeView();
    void ActivateColumnView();
    void DeactivateDualPane();
    void DeactivateTreeView();

    [[nodiscard]] auto getTabWidgetLeft()   const -> QTabWidget*    { return ui->tabWidget; }
    [[nodiscard]] auto getTabWidgetRight()  const -> QTabWidget*    { return ui->tabWidget_2; }
    [[nodiscard]] auto getTabBarLeft()      const -> QTabBar*       { return ui->tabWidget->tabBar(); }
    [[nodiscard]] auto getTabBarRight()     const -> QTabBar*       { return ui->tabWidget_2->tabBar(); }
    [[nodiscard]] auto getTreeViewLeft()    const -> QTreeView*     { return ui->FileTreeView; }
    [[nodiscard]] auto getTreeViewRight()   const -> QTreeView*     { return ui->FileTreeView_2; }
    [[nodiscard]] auto getTableViewLeft()   const -> QTableView*    { return ui->tableView; }
    [[nodiscard]] auto getTableViewRight()  const -> QTableView*    { return ui->tableView_2; }

    [[nodiscard]] auto getPasteButton()     const -> QToolButton*   { return ui->toolPasteButton; }
    [[nodiscard]] auto getCutButton()       const -> QToolButton*   { return ui->toolCutButton; }
    [[nodiscard]] auto getCopyButton()      const -> QToolButton*   { return ui->toolCopyButton; }
    [[nodiscard]] auto getDeleteButton()    const -> QToolButton*   { return ui->toolDelButton; }
    [[nodiscard]] auto getRenameButton()    const -> QToolButton*   { return ui->toolRenameButton; }

    [[nodiscard]] auto isWorkingOnRightPane()   const -> bool       { return m_isWorkingOnRightPane; }

    [[nodiscard]] auto getFileModelOpLeft()     const -> FileModelOperations*   { return m_fileModelOpLeft; }
    [[nodiscard]] auto getFileModelOpRight()    const -> FileModelOperations*   { return m_fileModelOpRight; }
    [[nodiscard]] auto getFileOpManager()       const -> FileOperationManager*  { return m_fileOpManager; }

private:
    struct UIManager final
    {
        UIManager(Ui::MainWindow*& theUi, QMainWindow* pWnd);
    };


private slots:
    // tree
    void on_FileTreeView_clicked(const QModelIndex &index);
    void on_FileTreeView_2_clicked(const QModelIndex &index);
    void onTreeSelectionChanged(const QModelIndex &current, const QModelIndex &previous);

    // menu bar
    void on_actionExit_triggered();
    void on_actionTree_View_triggered();
    void on_actionAbout_triggered();
    void on_actionColumn_View_triggered();
    void on_actionDual_Pane_View_triggered();
    void on_actionSettings_triggered();

    // column view
    void on_columnView_clicked(const QModelIndex &index);

    // spliter
    void on_splitter_splitterMoved(int pos, int index);


    void on_toolCmdButton_pressed();

    // tab bar
    void on_tabWidget_tabBarClicked(int index);
    void on_tabWidget_2_tabBarClicked(int index);
    void on_tabWidget_tabCloseRequested(int index);
    void on_tabWidget_2_tabCloseRequested(int index);

    // table view
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_tableView_2_doubleClicked(const QModelIndex &index);

    // tool bar
    void on_toolUpButton_clicked();
    void on_toolBackButton_clicked();
    void on_toolForwardButton_clicked();
    void on_lineEdit_returnPressed();
    void on_toolSearchButton_clicked();
    void on_toolHistoryButton_clicked();

private:
    virtual auto eventFilter(QObject* obj, QEvent* event) -> bool override;

    virtual void keyPressEvent(QKeyEvent* event) override; // şimdilik mainWİdget'da bazı ögelere focus için kullanılıyor


private:
    void SetLabelText_(const QString& path);


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

    // history butonlarının hangi tabWidget için çalıştığını belirtmek için kullanılır.
    bool isWorkingOnLeftPane{}; // TODO(fatpound): kod eklenecek
    bool m_isWorkingOnRightPane{};

    bool leftTabIsReset_  = true;
    bool rightTabIsReset_ = true;

    bool treeViewActive = true;
    bool dualPaneActive = true;
    bool columnViewActive{};

    bool searchOn{};
};
#endif // MAINWINDOW_H
