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

    ui->codeButton->setToolTip(tr("打开文件扫描二维码"));

    ui->versionSpinBox->setMinimum(1);
    ui->versionSpinBox->setMaximum(MAX_LEVEL);
    ui->versionSpinBox->setValue(1);
    qrCode = new QRCode(this);
    on_generateButton_clicked();

    historyStatistics = new HistoryStatistics(this);

    connect(this,SIGNAL(decodeSignal(int)),historyStatistics,SLOT(on_decode_triggered(int)));
    connect(this,SIGNAL(encodeSignal(int)),historyStatistics,SLOT(on_encode_triggered(int)));
    connect(ui->clearLogAction,SIGNAL(triggered()),historyStatistics,SLOT(on_clearLog_triggered()));
}

MainWindow::~MainWindow()
{
    // delete this->historyStatistics;
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

    ui->codeButton->setIcon(QIcon(QPixmap::fromImage(qrCode->getImage())));
    ui->codeButton->setIconSize(QSize(QRCODE_WIDTH, QRCODE_WIDTH));
    ui->codeButton->adjustSize();

    ui->generateButton->setEnabled(false);
    ui->versionSpinBox->setValue(qrCode->getVersion());
    ui->statusBar->showMessage(tr("生成成功"), STATUSBAR_SHOW_TIPS_TIME);

    emit encodeSignal(sizeof(char)*(ui->textEdit->toPlainText().size()));
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

void MainWindow::on_openFileForDecodeAction_triggered()
{
    path = toDir(path);
    QString fileName =  QFileDialog::getOpenFileName(this, tr("打开二维码"), path, tr("Image File(*.bmp *.jpg *.png"));
    if(fileName.isEmpty()){
        qDebug()<<tr("cancel decode from image file");
        return;
    }
    path = fileName;
    QImage imageToDecode(path);
    QZXing decoder;
    decoder.setDecoder( QZXing::DecoderFormat_QR_CODE | QZXing::DecoderFormat_EAN_13 );
    QString result = decoder.decodeImage(imageToDecode);
    qDebug()<<tr("decode result:") + result<<endl;
    ui->textEdit->setText(result);
    ui->codeButton->setIcon(QIcon(QPixmap(path)));
    ui->statusBar->showMessage(tr("解码成功"), STATUSBAR_SHOW_TIPS_TIME);

    emit decodeSignal(sizeof(char)*result.size());
}

void MainWindow::on_codeButton_clicked()
{
    on_openFileForDecodeAction_triggered();
}

void MainWindow::on_showStatisticsAction_triggered()
{
    const HistoryStatistics&hs = *historyStatistics;
    QString text;
    QTextStream out(&text);
    out<<tr("编码次数: ")<<hs.getEncodeTimes()<<endl;
    out<<tr("编码成功率: ")<<100.0*hs.getEncodeSuccessTimes()/hs.getEncodeTimes()<<"%"<<endl;
    out<<tr("编码字节累计: ")<<hs.getEncodeBytesCount()<<endl;

    out<<tr("解码次数: ")<<hs.getDecodeTimes()<<endl;
    out<<tr("解码成功率: ")<<100.0*hs.getDecodeSuccessTimes()/hs.getDecodeTimes()<<"%"<<endl;
    out<<tr("解码字节累计: ")<<hs.getDecodeBytesCount()<<endl;
    QMessageBox::information(this, tr("统计信息"), text, QMessageBox::Ok);
}

void MainWindow::on_clearLogAction_triggered()
{
    ui->statusBar->showMessage(tr("清空编码/解码历史"), STATUSBAR_SHOW_TIPS_TIME);
}
