#ifndef STRINGFINDERDIALOG_H
#define STRINGFINDERDIALOG_H

#include <QDialog>

namespace Ui {
class StringFinderDialog;
}

class StringFinderDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit StringFinderDialog(QWidget *parent = 0);
    ~StringFinderDialog();
    
private slots:
    void on_pushButtonFindStrings_clicked();

private:
    Ui::StringFinderDialog *ui;
};

#endif // STRINGFINDERDIALOG_H
