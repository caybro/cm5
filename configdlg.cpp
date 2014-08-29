#include <QFileDialog>

#include "configdlg.h"
#include "ui_configdlg.h"

ConfigDlg::ConfigDlg(const QString &rootDir, const QString &editor, QWidget *parent):
    QDialog(parent),
    ui(new Ui::ConfigDlg)
{
    ui->setupUi(this);

    ui->rootDir->setText(rootDir);
    ui->editor->setText(editor);

    connect(ui->btnRootDir, &QToolButton::clicked, this, &ConfigDlg::slotChooseRootDir);
    connect(ui->btnEditor, &QToolButton::clicked, this, &ConfigDlg::slotChooseEditor);
}

ConfigDlg::~ConfigDlg()
{
    delete ui;
}

QString ConfigDlg::rootDir() const
{
    return ui->rootDir->text();
}

QString ConfigDlg::editor() const
{
    return ui->editor->text();
}

void ConfigDlg::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ConfigDlg::slotChooseRootDir()
{
    const QString rootdir = QFileDialog::getExistingDirectory(this, tr("Choose Root Directory"), QDir::homePath());

    if (!rootdir.isEmpty()) {
        ui->rootDir->setText(rootdir);
    }
}

void ConfigDlg::slotChooseEditor()
{
    const QString editor = QFileDialog::getOpenFileName(this, tr("Choose Editor"));

    if (!editor.isEmpty() && QFileInfo(editor).isExecutable()) {
        ui->editor->setText(editor);
    }
}
