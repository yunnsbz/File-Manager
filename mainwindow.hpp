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

    auto getUI() -> Ui::MainWindow*;

private:
    Ui::MainWindow *ui;
    QFileSystemModel *FileModel;
};
#endif // MAINWINDOW_H
