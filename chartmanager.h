#ifndef CHARTMANAGER_H
#define CHARTMANAGER_H

#include <QtCharts/QChartView>
#include <QJsonObject>

class ChartManager : public QChartView {
    Q_OBJECT
public:
    explicit ChartManager(QWidget *parent = nullptr);
    void showAgeDistribution(const QJsonObject &ageData);
};

#endif // CHARTMANAGER_H
