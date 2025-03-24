#include "ChartManager.h"
#include <QtCharts/QChart>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>

ChartManager::ChartManager(QWidget *parent) : QChartView(parent) {
    QPieSeries *series = new QPieSeries(this);
    series->append("0-14", 15);
    series->append("15-64", 70);
    series->append("65+", 15);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Age Distribution");
    chart->legend()->show();
    setChart(chart);
}

void ChartManager::showAgeDistribution(const QJsonObject &ageData) {
    qDebug() << "Age data:" << ageData;
    QPieSeries *series = new QPieSeries(this);
    series->append("0-14", ageData.value("0-14").toDouble());
    series->append("15-64", ageData.value("15-64").toDouble());
    series->append("65+", ageData.value("65+").toDouble());

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Age Distribution");
    chart->legend()->show();
    setChart(chart);
}
