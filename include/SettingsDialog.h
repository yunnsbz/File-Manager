#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QString>
#include <QTreeWidget>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog() override;

    static void SetOldButtonStyle(bool buttonStyle);

    void onSettingItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* /*previous*/);

private slots:
    void on_pushButton_clicked();

private:
    Ui::SettingsDialog *ui;
    bool tabCloseButtonOld{};
};

#endif // SETTINGSDIALOG_H
