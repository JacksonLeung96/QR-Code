#ifndef HISTORYSTATISTICS_H
#define HISTORYSTATISTICS_H

#include <QObject>

class HistoryStatistics : public QObject
{
    Q_OBJECT
public:
    explicit HistoryStatistics(QObject *parent = nullptr);

signals:

public slots:
};

#endif // HISTORYSTATISTICS_H