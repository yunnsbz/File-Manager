#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QFileSystemModel;

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


protected:


private slots:
    void on_actionExit_triggered();
    void on_FileTreeView_doubleClicked(const QModelIndex &index);
    void on_tableView_doubleClicked(const QModelIndex &index);
    void onTreeSelectionChanged(const QModelIndex &current, const QModelIndex &previous);

    void on_splitter_splitterMoved(int pos, int index);

    void on_actionList_View_triggered();

private:
    Ui::MainWindow *ui;
    QFileSystemModel *FileModel;

    bool treeActive = true;

};
#endif // MAINWINDOW_H
