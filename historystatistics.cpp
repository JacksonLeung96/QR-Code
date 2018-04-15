#include "historystatistics.h"

HistoryStatistics::HistoryStatistics(QObject *parent) : QObject(parent)
{
    QFile file("statistics.log");
    if(file.open(QFile::ReadOnly | QFile::Text)){
        QTextStream in(&file);
        while(!in.atEnd()){
            QString key;
            int value;
            in>>key>>value;
            if(key.isEmpty()){
                continue;
            }
            record[key]=value;
            qDebug()<<key<<": "<<value<<endl;
        }
        file.close();
    }
    else{
        qDebug()<<tr("error: can't open statistic file")<<endl;
    }
}

HistoryStatistics::~HistoryStatistics(){
    qDebug()<<tr("destory HistoryStatistics")<<endl;
    QFile file("statistics.log");
    if(file.open(QFile::WriteOnly | QFile::Text)){
        QTextStream out(&file);
        for(QMap<QString,int>::const_iterator it = record.cbegin(); it != record.cend(); ++it){
            out<<it.key()<<" "<<it.value()<<endl;
            qDebug()<<it.key()<<": "<<it.value()<<endl;
        }
        out.flush();
        file.close();
    }
    else{
        qDebug()<<tr("error: can't open statistic file for update log")<<endl;
        qDebug()<<file.error()<<endl;
        qDebug()<<file.errorString()<<endl;
    }
}

void HistoryStatistics::on_encode_triggered(int cnt){
    record["encodeTimes"]+=1;
    if(cnt>0){
        record["encodeSuccessTimes"]+=1;
        record["encodeBytesCount"]+=cnt;
    }
}

void HistoryStatistics::on_decode_triggered(int cnt){
    record["decodeTimes"]+=1;
    if(cnt>0){
        record["decodeSuccessTimes"]+=1;
        record["decodeBytesCount"]+=cnt;
    }
}

int HistoryStatistics::getDecodeBytesCount()const{
    return record["decodeBytesCount"];
}

int HistoryStatistics::getEncodeBytesCount()const{
    return record["encodeBytesCount"];
}

int HistoryStatistics::getDecodeTimes()const{
    return record["decodeTimes"];
}
int HistoryStatistics::getEncodeTimes()const{
    return record["encodeTimes"];
}

int HistoryStatistics::getDecodeSuccessTimes()const{
    return record["decodeSuccessTimes"];
}

int HistoryStatistics::getEncodeSuccessTimes()const{
    return record["encodeSuccessTimes"];
}

void HistoryStatistics::on_clearLog_triggered(){
    for(QMap<QString,int>::iterator it = record.begin(); it != record.end(); ++it){
        it.value()=0;
    }
}
