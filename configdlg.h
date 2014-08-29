#ifndef CONFIGDLG_H
#define CONFIGDLG_H

#include <QDialog>

namespace Ui {
class ConfigDlg;
}

class ConfigDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDlg(const QString & rootDir = QString(), const QString & editor = QString(), QWidget *parent = 0);
    ~ConfigDlg();

    QString rootDir() const;
    QString editor() const;

protected:
    void changeEvent(QEvent *e);

private slots:
    void slotChooseRootDir();
    void slotChooseEditor();

private:
    Ui::ConfigDlg *ui;
};

#endif // CONFIGDLG_H
