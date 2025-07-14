#include "SettingsDialog.h"
#include "ui/ui_settingsdialog.h"

#include <QFile>
#include <QApplication>
#include <QTreeWidgetItem>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    connect(ui->treeWidget, &QTreeWidget::currentItemChanged, this, &SettingsDialog::onSettingItemChanged);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::SetOldButtonStyle(bool buttonStyle)
{
    if(buttonStyle){
        // style:
        QString qss;
        QFile file1(":/resources/styles/styleDark.qss");
        QFile file2(":/resources/styles/button-style-new.qss");

        file1.open(QFile::ReadOnly);
        file2.open(QFile::ReadOnly);

        qss += file1.readAll();
        qss += file2.readAll();

        qApp->setStyleSheet(qss);
    }
    else{
        // style:
        QString qss;
        QFile file1(":/resources/styles/styleDark.qss");

        file1.open(QFile::ReadOnly);

        qss += file1.readAll();

        qApp->setStyleSheet(qss);
    }
}

void SettingsDialog::on_pushButton_clicked()
{
    SetOldButtonStyle(tabCloseButtonOld);

    if(tabCloseButtonOld)
    {
        tabCloseButtonOld = false;
    }
    else
    {
        tabCloseButtonOld = true;
    }
}

void SettingsDialog::onSettingItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* /*previous*/) {
    if (!current) return;

    const QString settingsName = current->text(0);

    // TODO (yunnsbz): change this to enum and switch case

    if (settingsName == "Styles")
        ui->stackedWidget->setCurrentIndex(0);
    else if (settingsName == "Tab Buttons")
        ui->stackedWidget->setCurrentIndex(1);
    else if (settingsName == "Files")
        ui->stackedWidget->setCurrentIndex(2);
    // ... vb.
}
