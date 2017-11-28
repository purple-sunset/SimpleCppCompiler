#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QFileDialog>
#include<QFile>
#include<QTextStream>
#include<QMessageBox>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    p = new Parser();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete p;
}


void MainWindow::on_btnOpen_clicked()
{
    QString fName = QFileDialog::getOpenFileName(this, tr("Open File"));
    QFile file(fName);
    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "Info", file.errorString());


    QTextStream in(&file);
    ui->fileName->addItem(fName);
    ui->codeTextEdit->document()->setPlainText(in.readAll());
}

void MainWindow::on_fileName_currentIndexChanged(const QString &arg1)
{
    QFile file(arg1);
    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "Info", file.errorString());


    QTextStream in(&file);
    ui->codeTextEdit->document()->setPlainText(in.readAll());
}

void MainWindow::on_btnParse_clicked()
{
    delete(p);
    ui->resultList->clear();
    p = new Parser();
    p->parseToken(ui->codeTextEdit->toPlainText());
    p->check();

    if(p->getErrors()->size() == 0)
    {
        ui->resultList->addItem("No error");
        //return;
    }

    for(int i=0; i<p->getErrors()->size(); i++)
    {
        Error e = p->getErrors()->at(i);
        ui->resultList->addItem(e.Print());
    }

}


void MainWindow::on_resultList_itemClicked(QListWidgetItem)
{
    int index = ui->resultList->currentRow();
    Error e = p->getErrors()->at(index);
    ui->codeTextEdit->setFocus();
    QTextCursor curs = ui->codeTextEdit->textCursor(); //copies current cursor
    curs.movePosition(QTextCursor::Start);
    curs.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, e.getLine());
    curs.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, e.getColumn());
    ui->codeTextEdit->setTextCursor(curs);
}
