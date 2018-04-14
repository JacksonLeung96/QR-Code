#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<qrcode.h>
#include<QDebug>
#include<QFile>
#include<QFileDialog>
#include<QFileInfo>
#include<QDir>
#include<QTextEdit>
#include<QMessageBox>
#include<QLabel>
#include<QPushButton>
#include<QLCDNumber>
#include<QSpinBox>
#include<QDateTime>
#include<QMutex>
#include<QStatusBar>
#include<QZXing>
#include<historystatistics.h>

class QRCode;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    const static int MAX_LEVEL = 40;
    const static int SYMBOL_NUM_BASE = 21;
    const static int SYMBOL_NUM_STEP = 4;
    const static int QRCODE_WIDTH = 400;
    const static int STATUSBAR_SHOW_TIPS_TIME = 3000;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_selectFileButton_clicked();

    void on_saveFileButton_clicked();

    void on_textEdit_textChanged();

    void on_generateButton_clicked();

    void on_exportAsButton_clicked();

    void on_versionSpinBox_valueChanged(int arg1);

    void on_exportButton_clicked();

    void on_openFileForDecodeAction_triggered();

    void on_codeButton_clicked();

    void on_showStatisticsAction_triggered();

    void on_clearLogAction_triggered();

signals:
    void encodeSignal(int cnt);
    void decodeSignal(int cnt);

private:
    Ui::MainWindow *ui;
    QString path;
    QString toDir(const QString&filePath);
    bool getOpenTextFileName(QString&path);
    QRCode*qrCode;
    int SymbolNum[MAX_LEVEL];
    QMutex exportFileCntMutex;
    HistoryStatistics*historyStatistics;
};

#endif // MAINWINDOW_H
