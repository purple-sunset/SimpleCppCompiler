#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include<QMainWindow>
#include<QListWidget>
#include "parser.h"
#include "codeeditor.h"
#include "token.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void on_btnOpen_clicked();

    void on_btnParse_clicked();

    void on_fileName_currentIndexChanged(const QString &arg1);

    void on_resultList_itemClicked(QListWidgetItem);

private:
    Ui::MainWindow *ui;
    Parser *p;
};

#endif // MAINWINDOW_H
