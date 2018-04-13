#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //symbol number = ( symbol number base(21) + (level - 1) * symbol number step(4) )^2
    for(int i = 0; i < MAX_LEVEL; ++i){
        SymbolNum[i] = SYMBOL_NUM_BASE + i * SYMBOL_NUM_STEP;
        SymbolNum[i] *= SymbolNum[i];
    }

    ui->versionSpinBox->setMinimum(1);
    ui->versionSpinBox->setMaximum(MAX_LEVEL);
    ui->versionSpinBox->setValue(1);
    qrCode = new QRCode(this);
    on_generateButton_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::toDir(const QString&filePath){
    QString path;
    QFileInfo fileInfo(filePath);
    if(filePath.isEmpty()||
            (!fileInfo.isDir() && !fileInfo.isFile())){
        path = "/";
    }
    if(fileInfo.isFile()){
        path = fileInfo.dir().path();
    }
    qDebug()<<tr("get dir: ")+path<<endl;
    return path;
}

bool MainWindow::getOpenTextFileName(QString&path){
    path = QFileDialog::getOpenFileName(this, tr("打开文本文件"), path, "Text File(*.txt)");
    if(path.isNull()){
        qDebug()<<tr("Cancel Open File")<<endl;
        return false;
    }
    qDebug()<<tr("select file: ") + path<<endl;
    return true;
}

void MainWindow::on_selectFileButton_clicked()
{
    // under the last path's dir
    this->path = toDir(path);
    if(!getOpenTextFileName(path)){
        return;
    }
    QFile file(path);
    if(file.open(QFile::ReadOnly | QFile::Text)){
        qDebug()<<tr("open file success")<<endl;

        QString text(file.readAll());
        ui->textEdit->setText(text);

        file.close();
    }
    else{
        qDebug()<<tr("open file failure")<<endl;
        QMessageBox::warning(this, tr("警告"), tr("无法打开文件")+file.fileName());
    }
}

void MainWindow::on_saveFileButton_clicked()
{
    // under the last path's dir
    path = toDir(path);
    if(!getOpenTextFileName(path)){
        return;
    }
    QFile file(path);
    if(file.open(QFile::WriteOnly | QFile::Text)){
        qDebug()<<tr("open file success")<<endl;
        QString text = ui->textEdit->toPlainText();
        file.write(text.toStdString().c_str());
    }
    else{
        qDebug()<<tr("open file failure")<<endl;
        QMessageBox::warning(this, tr("警告"), tr("无法打开文件")+file.fileName());
    }
}

void MainWindow::on_textEdit_textChanged()
{
    //change symbol number and level
    ui->generateButton->setEnabled(true);
}

void MainWindow::on_generateButton_clicked()
{
    int codeVersion = ui->versionSpinBox->value();
    qrCode->generate(ui->textEdit->toPlainText(), QRCODE_WIDTH, QRCODE_WIDTH, codeVersion);
    ui->codeLabel->setPixmap(QPixmap::fromImage(qrCode->getImage()));
    ui->generateButton->setEnabled(false);
    ui->versionSpinBox->setValue(qrCode->getVersion());
    ui->statusBar->showMessage(tr("生成成功"), STATUSBAR_SHOW_TIPS_TIME);
}

void MainWindow::on_exportAsButton_clicked()
{
    path = toDir(path);
    path = QFileDialog::getSaveFileName(this, tr("导出为图片"), path, tr("Image File(*.jpg *.png *.bmp)"));
    qrCode->saveImgAs(path);
    ui->statusBar->showMessage(tr("导出为")+path, STATUSBAR_SHOW_TIPS_TIME);
}

void MainWindow::on_versionSpinBox_valueChanged(int arg1)
{
    ui->symbolNumLcdNumber->display(QString::number(SymbolNum[arg1-1]));
    ui->generateButton->setEnabled(true);
}

void MainWindow::on_exportButton_clicked()
{
    static QDateTime previousTime;
    static QDateTime currentTime;
    currentTime = QDateTime::currentDateTime();
    QString cntStr;

    //exportFileCntMutex.lock();

    static int cnt = 0;
    if(currentTime.toString("yyyyMMddhhmmss") == previousTime.toString("yyyyMMddhhmmss")){
        ++cnt;
    }
    else{
        cnt = 0;
    }
    cntStr = QString::number(cnt);
    previousTime = currentTime;

    //exportFileCntMutex.unlock();


    while(cntStr.size()<3){
        cntStr = "0" + cntStr;
    }
    QString fileName = currentTime.toString("yyyyMMddhhmmss") + cntStr;
    fileName +=".bmp";
    qDebug()<<tr("save as: ") + fileName<<endl;
    qrCode->saveImgAs(fileName);

    ui->statusBar->showMessage(tr("导出为")+fileName, STATUSBAR_SHOW_TIPS_TIME);
}
