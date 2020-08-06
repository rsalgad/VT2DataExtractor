#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QFileDialog>
#include <QDebug>
#include <QDirIterator>
#include <QListWidgetItem>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    folderPath = "";
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btn_selectJob_clicked()
{
    QDir mDir;
    filePath = mDir.filePath(QFileDialog::getOpenFileName(this, tr("Open File"), folderPath, tr("JOB (*.job)")));
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    QString job_ID = "";

    while (!in.atEnd()) {
        QString s = in.readLine();
        if (s.contains("Load Series ID")){
            QStringList list = s.split(":");
            job_ID = list[1].trimmed();
            break;
        }
    }

    QDir d = QFileInfo(filePath).absoluteDir();
    folderPath = d.absolutePath();

    QDirIterator dirIt(folderPath);
    result_files.clear();
    while (dirIt.hasNext()) {
        dirIt.next();
        if (QFileInfo(dirIt.filePath()).isFile())
        if (QFileInfo(dirIt.filePath()).suffix() == "A2E")
        result_files << dirIt.filePath();
    }
    UpdateListView();
}

void MainWindow::UpdateListView(){
    ui->listWidget->clear();
    for (int i = 0; i < result_files.length(); i++){
        QFileInfo fileInfo(result_files[i]);
            ui->listWidget->addItem(fileInfo.fileName());
    }

}

void MainWindow::on_btn_Get_Data_clicked()
{
    int nodeID;
    if (ui->lineEdit_node->text() != ""){
        nodeID = ui->lineEdit_node->text().toInt();
    }

    ui->textEdit->clear();
    QString string = "";

    for (int i = 0; i < result_files.length(); i++){
        QFile file(result_files[i]);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;

        QTextStream in(&file);

        bool store = false;
        QStringList allDispList;

        while (!in.atEnd()) {
            QString s = in.readLine();
            if (ui->radioButton_Rx->isChecked() || ui->radioButton_Ry->isChecked()){
                if (s.contains("REACTIONS")){
                    store = true;
                }

                if (store){
                    if(s.contains("DISPLACEMENTS")){
                        store = false;
                        break;
                    }
                    allDispList << s;
                }
            } else {
                if (s.contains("DISPLACEMENTS")){
                    store = true;
                }

                if (store){
                    if(s.contains("ELEMENT TOTAL STRAINS")){
                        store = false;
                        break;
                    }
                    allDispList << s;
                }
            }
        }

        for (int i = 0; i < allDispList.length(); i++){
            QStringList unformatOneLineList = allDispList[i].split(" ");
            QStringList formattedOneLineList;

            for (int j = 0; j < unformatOneLineList.length(); j++){
                if (unformatOneLineList[j] != ""){
                    formattedOneLineList << unformatOneLineList[j];
                }
            }

            if (formattedOneLineList.length() > 2 && formattedOneLineList.length() > 3){
                if (formattedOneLineList[0] == QString::number(nodeID)){
                    if (ui->radioButton_dx->isChecked() || ui->radioButton_Rx->isChecked()){
                        string += formattedOneLineList[1];
                    } else {
                        string += formattedOneLineList[2];
                    }
                    string += "\n";
                } else if (formattedOneLineList[3] == QString::number(nodeID)){
                    if (ui->radioButton_dx->isChecked() || ui->radioButton_Rx->isChecked()){
                        string += formattedOneLineList[4];
                    } else {
                        string += formattedOneLineList[5];
                    }
                    string += "\n";
                }
            } else if (formattedOneLineList.length() > 2 && formattedOneLineList.length() < 4) {
                if (formattedOneLineList[0] == QString::number(nodeID)){
                    if (ui->radioButton_dx->isChecked() || ui->radioButton_Rx->isChecked()){
                        string += formattedOneLineList[1];
                    } else {
                        string += formattedOneLineList[2];
                    }
                    string += "\n";
                }
            }
        }
        ui->textEdit->setText(string);
    }

}

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    int fileIndex = ui->listWidget->currentRow();
    QFile file(result_files[fileIndex]);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    ui->textEdit->clear();

    QTextStream in(&file);

    bool store = false;
    QStringList list;

    while (!in.atEnd()) {
        QString s = in.readLine();
        if (s.contains("DISPLACEMENTS")){
            store = true;
        }

        if (store){
            if(s.contains("ELEMENT TOTAL STRAINS")){
                store = false;
                break;
            }
            list << s;
        }
    }

    QString string = "";
    for (int i = 0; i < list.length(); i++){
        QStringList list2 = list[i].split(" ");
        QStringList list3;

        for (int j = 0; j < list2.length(); j++){
            if (list2[j] != ""){
                list3 << list2[j];
            }
        }

        for (int j = 0; j < list3.length(); j++){
            string += list3[j];
            if (j != list3.length() - 1){
                string += "\t";
            } else {
                string += "\n";
            }
        }
    }
    ui->textEdit->setText(string);
}
