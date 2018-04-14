#ifndef HISTORYSTATISTICS_H
#define HISTORYSTATISTICS_H

#include <QObject>
#include<QFile>
#include<QDebug>
#include<QMap>

class HistoryStatistics : public QObject
{
    Q_OBJECT
public:
    explicit HistoryStatistics(QObject *parent = nullptr);
    ~HistoryStatistics();
    int getEncodeTimes()const;
    int getDecodeTimes()const;
    int getEncodeSuccessTimes()const;
    int getDecodeSuccessTimes()const;
    int getEncodeBytesCount()const;
    int getDecodeBytesCount()const;
signals:

public slots:
    void on_decode_triggered(int cnt); // cnt: decode bytes count
    void on_encode_triggered(int cnt);
    void on_clearLog_triggered();
private:
    QMap<QString,int>record;
};

#endif // HISTORYSTATISTICS_H
