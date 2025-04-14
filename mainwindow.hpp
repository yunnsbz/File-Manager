#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


class QFileSystemModel;
class TabManager;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

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

    void SetTabContent(int tabIndex);
    void setDefaultContent();
protected:


private slots:
    void on_actionExit_triggered();
    void on_tableView_doubleClicked(const QModelIndex &index);
    void onTreeSelectionChanged(const QModelIndex &current, const QModelIndex &previous);

    void on_splitter_splitterMoved(int pos, int index);

    void on_actionList_View_triggered();

    void on_tabWidget_tabCloseRequested(int index);

    void on_tabWidget_tabBarClicked(int index);

    void on_FileTreeView_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    QFileSystemModel *FileModel;
    TabManager* tabManager;

    bool eventFilter(QObject* obj, QEvent* event) override;

    // treeView is collapsed or not:
    bool treeActive = true;

    // tree view expanded Indexes
    QList<QList<QModelIndex>> tabsExpandedIndexes;
};
#endif // MAINWINDOW_H
