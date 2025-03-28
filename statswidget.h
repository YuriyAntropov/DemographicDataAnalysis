#ifndef STATSWIDGET_H
#define STATSWIDGET_H

#include "AbstractWidget.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QChartView>
#include <QChart>
#include <QBarSeries>
#include <QBarSet>
#include <QValueAxis>
#include <QtCharts/QBarCategoryAxis>
#include <QPushButton>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QElapsedTimer> // Добавляем для замера времени
#include <algorithm> // Добавляем для std::sort

class MainWindow;

class StatsWidget : public AbstractWidget {
    Q_OBJECT
public:
    explicit StatsWidget(MainWindow *mainWindow, QWidget *parent = nullptr);
    ~StatsWidget();

    // Реализация виртуального метода
    void updateDisplay() override;

public slots:
    void showCountryStats(const QString &countryName, qint64 population,
                          const QString &capital, const QString &region, const QString &subregion,
                          qreal area, const QStringList &currencies, const QStringList &languages,
                          const QStringList &timezones, const QString &phoneCode, const QStringList &tld,
                          const QPointF &countryCoordinates, const QPointF &capitalCoordinates,
                          const QString &flagUrl, const QString &coatOfArmsUrl, const QString &drivingSide);;
    void showWorldStats();
    void showComparison(const QStringList &countries);

private slots:

    void onFlagReplyFinished(QNetworkReply *reply);
    void onCoatOfArmsReplyFinished(QNetworkReply *reply);
    void showPopulationComparison();
    void showAreaComparison();
    void showDensityComparison();
    void sortCountriesByPopulation();
    void sortCountriesByName(); // Функция для сортировки по имени
    void sortCountriesByArea(); // Слот для сортировки по площади
    void onEditDataClicked();

private:
    void createComparisonButtons();
    void hideAllCharts();
    void populateWorldStatsTable();
    void recreateChartViews();

    MainWindow *mainWindow;
    QVBoxLayout *layout;
    QNetworkAccessManager *networkManager;
    QLabel *titleLabel;
    QPushButton *populationButton;
    QPushButton *areaButton;
    QPushButton *densityButton;
    QHBoxLayout *buttonLayout;
    QTableWidget *countriesTable;
    QPushButton *sortByPopulationButton;
    QLabel *sortTimeLabel; // QLabel для времени сортировки
    QPushButton *sortByNameButton; // Кнопка для сортировки по имени
    QPushButton *sortByAreaButton;
    QPushButton *editDataButton;
    QString currentCountryCode; // Для хранения кода текущей страны
    QChartView *sortTimeChartView; // Новый график для времени сортировки

    QLabel *populationLabel;
    QLabel *capitalLabel;
    QLabel *regionLabel;
    QLabel *subregionLabel;
    QLabel *areaLabel;
    QLabel *currenciesLabel;
    QLabel *languagesLabel;
    QLabel *timezonesLabel;
    QLabel *phoneCodeLabel;
    QLabel *tldLabel;
    QLabel *countryCoordinatesLabel;
    QLabel *capitalCoordinatesLabel;
    QLabel *flagLabel;
    QLabel *coatOfArmsLabel;
    QLabel *drivingSideLabel;

    QChartView *populationChartView;
    QChartView *areaChartView;
    QChartView *densityChartView;

    QStringList currentCountries;
};

#endif // STATSWIDGET_H
