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
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    auto getUI() -> Ui::MainWindow*;

private:
    Ui::MainWindow *ui;
    QFileSystemModel *FileModel;
};
#endif // MAINWINDOW_H
