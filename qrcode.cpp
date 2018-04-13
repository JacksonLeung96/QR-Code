#include "qrcode.h"

QRCode::QRCode(QObject *parent) : QObject(parent)
{

}

bool QRCode::generate(const QString&text, int width, int height, int version){
    if(text == this->text){
        return true;
    }
    if(img!=nullptr){
        delete img;
        img = nullptr;
    }
    img = new QImage(width, height, QImage::Format_ARGB32);
    qrCode = QRcode_encodeString(text.toStdString().c_str(), version, QR_ECLEVEL_H, QR_MODE_8, 1);
    drawImg(img, width, height);
    return true;
}

void QRCode::drawImg(QImage*img, int width, int height){
    QPainter painter(img);
    {
        QColor background(Qt::white);
        QRectF rect(0, 0, width, height);
        painter.setBrush(background);
        painter.drawRect(rect);
    }
    QColor foreground(Qt::black);
    painter.setBrush(foreground);
    const int qrWidth = qrCode->width > 0 ? qrCode->width : 1;
    double scaleX = 1.0 * width / qrWidth;
    double scaleY = 1.0 * height / qrWidth;
    for(int y = 0; y < qrWidth; ++y)
    {
        for(int x = 0; x < qrWidth; ++x)
        {
            unsigned char b = qrCode->data[y * qrWidth + x];
            if(b & 1)
            {
                QRectF rect(x * scaleX, y * scaleY, scaleX, scaleY);
                //qDebug()<<rect.width()<<" "<<rect.height()<<endl;
                painter.drawRect(rect);
            }
        }
    }
}

QImage QRCode::getImage(){
    return *img;
}

void QRCode::saveImgAs(QString fileName){
    img->save(fileName);
}

int QRCode::getVersion(){
    return qrCode->version;
}
