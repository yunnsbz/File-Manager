#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QString>
#include <QTreeWidget>
#include <FM_Macros.hpp>

namespace Ui {
class SettingsDialog;
}

FM_BEGIN_NAMESPACE

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog() override;

    static void setOldButtonStyle(bool buttonStyle);

    void onSettingItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* /*previous*/);

private slots:
    void on_pushButton_clicked();

private:
    Ui::SettingsDialog *ui;
    bool tabCloseButtonOld{};
};

FM_END_NAMESPACE
#endif // SETTINGSDIALOG_H
