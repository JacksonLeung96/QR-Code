#ifndef QRCODE_H
#define QRCODE_H

#include <QObject>
#include<QPainter>
#include<qrencode.h>
#include<QDebug>

class QRCode : public QObject
{
    Q_OBJECT
public:
    explicit QRCode(QObject *parent = nullptr);
    bool generate(const QString&text, int width, int height, int version = 0);
    QImage getImage();
    void saveImgAs(QString fileName);
    int getVersion();
signals:

public slots:

private:
    QRcode*qrCode;
    QString text;
    QImage*img = nullptr;
    void drawImg(QImage*img, int width, int height);
};

#endif // QRCODE_H
