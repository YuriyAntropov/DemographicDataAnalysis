#include "StatsWidget.h"
#include "MainWindow.h"
#include <QNetworkRequest>
#include <QDebug>
#include <QScrollBar>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "DataEditDialog.h"

StatsWidget::StatsWidget(MainWindow *mainWindow, QWidget *parent) : AbstractWidget(parent), mainWindow(mainWindow) {
    layout = new QVBoxLayout(this);

    setStyleSheet("StatsWidget { background-color: #2E2E2E; }");

    networkManager = new QNetworkAccessManager(this);

    titleLabel = new QLabel("Statistics", this);
    titleLabel->setStyleSheet("font-size: 16pt; font-weight: bold; color: #FFFFFF;");
    layout->addWidget(titleLabel);

    createComparisonButtons();

    // Инициализируем графики один раз
    populationChartView = new QChartView(this);
    populationChartView->setRenderHint(QPainter::Antialiasing);
    populationChartView->setMinimumHeight(300);
    populationChartView->setVisible(false);
    layout->addWidget(populationChartView);

    areaChartView = new QChartView(this);
    areaChartView->setRenderHint(QPainter::Antialiasing);
    areaChartView->setMinimumHeight(300);
    areaChartView->setVisible(false);
    layout->addWidget(areaChartView);

    densityChartView = new QChartView(this);
    densityChartView->setRenderHint(QPainter::Antialiasing);
    densityChartView->setMinimumHeight(300);
    densityChartView->setVisible(false);
    layout->addWidget(densityChartView);

    countriesTable = new QTableWidget(this);
    countriesTable->setColumnCount(3);
    countriesTable->setHorizontalHeaderLabels({"Country", "Population", "Area (km²)"});
    countriesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    countriesTable->setSelectionMode(QAbstractItemView::NoSelection);
    countriesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    countriesTable->setVisible(false);
    layout->addWidget(countriesTable);

    editDataButton = new QPushButton("Edit Data", this);
    editDataButton->setVisible(false);
    connect(editDataButton, &QPushButton::clicked, this, &StatsWidget::onEditDataClicked);
    layout->addWidget(editDataButton);

    sortByPopulationButton = new QPushButton("Sort by Population", this);
    sortByPopulationButton->setVisible(false);
    connect(sortByPopulationButton, &QPushButton::clicked, this, &StatsWidget::sortCountriesByPopulation);
    layout->addWidget(sortByPopulationButton);

    sortByNameButton = new QPushButton("Sort by Name", this);
    sortByNameButton->setVisible(false);
    connect(sortByNameButton, &QPushButton::clicked, this, &StatsWidget::sortCountriesByName);
    layout->addWidget(sortByNameButton);

    sortByAreaButton = new QPushButton("Sort by Area", this);
    sortByAreaButton->setVisible(false);
    connect(sortByAreaButton, &QPushButton::clicked, this, &StatsWidget::sortCountriesByArea);
    layout->addWidget(sortByAreaButton);

    sortTimeLabel = new QLabel("Sort Time: N/A", this);
    sortTimeLabel->setStyleSheet("font-size: 12pt; color: #E0E0E0;");
    sortTimeLabel->setVisible(false); // Скрываем по умолчанию
    layout->addWidget(sortTimeLabel);

    sortTimeChartView = new QChartView(this);
    sortTimeChartView->setRenderHint(QPainter::Antialiasing);
    sortTimeChartView->setMinimumHeight(200);
    sortTimeChartView->setVisible(false);
    layout->addWidget(sortTimeChartView);

    // Инициализация остальных меток
    populationLabel = new QLabel("Population: N/A", this);
    populationLabel->setStyleSheet("font-size: 12pt; color: #E0E0E0;");
    layout->addWidget(populationLabel);

    capitalLabel = new QLabel("Capital: N/A", this);
    capitalLabel->setStyleSheet("font-size: 12pt; color: #E0E0E0;");
    layout->addWidget(capitalLabel);

    regionLabel = new QLabel("Region: N/A", this);
    regionLabel->setStyleSheet("font-size: 12pt; color: #E0E0E0;");
    layout->addWidget(regionLabel);

    subregionLabel = new QLabel("Subregion: N/A", this);
    subregionLabel->setStyleSheet("font-size: 12pt; color: #E0E0E0;");
    layout->addWidget(subregionLabel);

    areaLabel = new QLabel("Area: N/A", this);
    areaLabel->setStyleSheet("font-size: 12pt; color: #E0E0E0;");
    layout->addWidget(areaLabel);

    currenciesLabel = new QLabel("Currencies: N/A", this);
    currenciesLabel->setStyleSheet("font-size: 12pt; color: #E0E0E0;");
    layout->addWidget(currenciesLabel);

    languagesLabel = new QLabel("Languages: N/A", this);
    languagesLabel->setStyleSheet("font-size: 12pt; color: #E0E0E0;");
    layout->addWidget(languagesLabel);

    timezonesLabel = new QLabel("Timezones: N/A", this);
    timezonesLabel->setStyleSheet("font-size: 12pt; color: #E0E0E0;");
    layout->addWidget(timezonesLabel);

    phoneCodeLabel = new QLabel("Phone Code: N/A", this);
    phoneCodeLabel->setStyleSheet("font-size: 12pt; color: #E0E0E0;");
    layout->addWidget(phoneCodeLabel);

    tldLabel = new QLabel("TLD: N/A", this);
    tldLabel->setStyleSheet("font-size: 12pt; color: #E0E0E0;");
    layout->addWidget(tldLabel);

    countryCoordinatesLabel = new QLabel("Country Coordinates: N/A", this);
    countryCoordinatesLabel->setStyleSheet("font-size: 12pt; color: #E0E0E0;");
    layout->addWidget(countryCoordinatesLabel);

    capitalCoordinatesLabel = new QLabel("Capital Coordinates: N/A", this);
    capitalCoordinatesLabel->setStyleSheet("font-size: 12pt; color: #E0E0E0;");
    layout->addWidget(capitalCoordinatesLabel);

    flagLabel = new QLabel("Flag: N/A", this);
    flagLabel->setStyleSheet("font-size: 12pt; color: #E0E0E0;");
    layout->addWidget(flagLabel);

    coatOfArmsLabel = new QLabel("Coat of Arms: N/A", this);
    coatOfArmsLabel->setStyleSheet("font-size: 12pt; color: #E0E0E0;");
    layout->addWidget(coatOfArmsLabel);

    drivingSideLabel = new QLabel("Driving Side: N/A", this);
    drivingSideLabel->setStyleSheet("font-size: 12pt; color: #E0E0E0;");
    layout->addWidget(drivingSideLabel);

    layout->addStretch();
    setLayout(layout);

    // Применяем стили
    setStyleSheet(
        "StatsWidget { background-color: #2E2E2E; }"
        "QLabel { color: #E0E0E0; }"
        "QPushButton { background-color: #424242; color: #E0E0E0; border-radius: 5px; padding: 5px; }"
        "QPushButton:hover { background-color: #42A5F5; color: #FFFFFF; }"
        "QPushButton:pressed { background-color: #1E88E5; }"
        "QTableWidget { background-color: #2E2E2E; color: #E0E0E0; border: none; }"
        "QTableWidget::item { padding: 5px; }"
        "QTableWidget::item:nth-child(even) { background-color: #383838; }"
        "QHeaderView::section { background-color: #424242; color: #FFFFFF; padding: 5px; }"
        );
}

// Реализация чисто виртуального метода
void StatsWidget::updateDisplay() {
    if (currentCountries.isEmpty() && currentCountryCode.isEmpty()) {
        showWorldStats(); // Если ничего не выбрано, показываем мировую статистику
    } else if (!currentCountryCode.isEmpty()) {
        // Обновляем статистику для текущей страны
        DemographicsManager *demographicsManager = mainWindow->getDemographicsManager();
        const DemographicsManager::CountryData &data = demographicsManager->getDemographics()[currentCountryCode];
        showCountryStats(
            titleLabel->text(),
            data.population,
            data.capital,
            data.region,
            data.subregion,
            data.area,
            data.currencies,
            data.languages,
            data.timezones,
            data.phoneCode,
            data.tld,
            data.countryCoordinates,
            data.capitalCoordinates,
            data.flagUrl,
            data.coatOfArmsUrl,
            data.drivingSide
            );
    } else {
        showComparison(currentCountries); // Обновляем сравнение
    }
}

StatsWidget::~StatsWidget() {
    delete networkManager;
}

void StatsWidget::createComparisonButtons() {
    buttonLayout = new QHBoxLayout();
    populationButton = new QPushButton("Population\nCompare", this);
    areaButton = new QPushButton("Area\nCompare", this);
    densityButton = new QPushButton("Density\nCompare", this);

    populationButton->setMinimumSize(80, 70);
    areaButton->setMinimumSize(80, 70);
    densityButton->setMinimumSize(80, 70);

    QFont buttonFont = populationButton->font();
    buttonFont.setPointSize(10);
    populationButton->setFont(buttonFont);
    areaButton->setFont(buttonFont);
    densityButton->setFont(buttonFont);

    populationButton->setStyleSheet("QPushButton { text-align: center; }");
    areaButton->setStyleSheet("QPushButton { text-align: center; }");
    densityButton->setStyleSheet("QPushButton { text-align: center; }");

    buttonLayout->addWidget(populationButton);
    buttonLayout->addWidget(areaButton);
    buttonLayout->addWidget(densityButton);

    populationButton->setVisible(false);
    areaButton->setVisible(false);
    densityButton->setVisible(false);

    layout->addLayout(buttonLayout);

    connect(populationButton, &QPushButton::clicked, this, &StatsWidget::showPopulationComparison);
    connect(areaButton, &QPushButton::clicked, this, &StatsWidget::showAreaComparison);
    connect(densityButton, &QPushButton::clicked, this, &StatsWidget::showDensityComparison);
}

void StatsWidget::hideAllCharts() {
    // Безопасно удаляем и пересоздаем графики
    if (populationChartView) {
        delete populationChartView;
        populationChartView = nullptr;
    }
    if (areaChartView) {
        delete areaChartView;
        areaChartView = nullptr;
    }
    if (densityChartView) {
        delete densityChartView;
        densityChartView = nullptr;
    }

    recreateChartViews(); // Пересоздаем пустые QChartView
}

void StatsWidget::recreateChartViews() {
    // Удаляем старые графики
    if (populationChartView) {
        layout->removeWidget(populationChartView);
        delete populationChartView;
        populationChartView = nullptr;
    }
    if (areaChartView) {
        layout->removeWidget(areaChartView);
        delete areaChartView;
        areaChartView = nullptr;
    }
    if (densityChartView) {
        layout->removeWidget(densityChartView);
        delete densityChartView;
        densityChartView = nullptr;
    }

    // Пересоздаем графики и добавляем их после buttonLayout
    populationChartView = new QChartView(this);
    populationChartView->setRenderHint(QPainter::Antialiasing);
    populationChartView->setMinimumHeight(300);
    populationChartView->setVisible(false);
    layout->insertWidget(2, populationChartView); // Индекс 2: после titleLabel (0) и buttonLayout (1)

    areaChartView = new QChartView(this);
    areaChartView->setRenderHint(QPainter::Antialiasing);
    areaChartView->setMinimumHeight(300);
    areaChartView->setVisible(false);
    layout->insertWidget(3, areaChartView); // Индекс 3: после populationChartView

    densityChartView = new QChartView(this);
    densityChartView->setRenderHint(QPainter::Antialiasing);
    densityChartView->setMinimumHeight(300);
    densityChartView->setVisible(false);
    layout->insertWidget(4, densityChartView); // Индекс 4: после areaChartView
}

void StatsWidget::populateWorldStatsTable() {
    countriesTable->setRowCount(0);

    const QMap<QString, DemographicsManager::CountryData>& demographics = mainWindow->getDemographicsManager()->getDemographics();

    QMap<QString, QString> codeToEnglishName;
    QJsonDocument doc;
    QFile file("data.json");
    if (file.open(QIODevice::ReadOnly)) {
        doc = QJsonDocument::fromJson(file.readAll());
        file.close();
    }
    if (!doc.isNull()) {
        QJsonArray countries = doc.array();
        for (const QJsonValue &value : countries) {
            QJsonObject country = value.toObject();
            QString code = country["cca3"].toString();
            QJsonObject nameObj = country["name"].toObject();
            QString englishName = nameObj["common"].toString();
            if (!englishName.isEmpty() && !code.isEmpty()) {
                codeToEnglishName[code] = englishName;
            }
        }
    }

    countriesTable->setRowCount(demographics.size());
    int row = 0;
    for (auto it = demographics.constBegin(); it != demographics.constEnd(); ++it) {
        QString countryCode = it.key();
        QString countryName = codeToEnglishName.value(countryCode, countryCode);
        if (!countryName.isEmpty()) {
            countryName = countryName.at(0).toUpper() + countryName.mid(1);
        }
        qint64 population = it.value().population;
        qreal area = it.value().area;

        QTableWidgetItem *nameItem = new QTableWidgetItem(countryName);
        QTableWidgetItem *populationItem = new QTableWidgetItem(population >= 0 ? QString::number(population) : "N/A");
        populationItem->setData(Qt::UserRole, population >= 0 ? population : QVariant());
        QTableWidgetItem *areaItem = new QTableWidgetItem(area >= 0 ? QString::number(area) : "N/A");
        areaItem->setData(Qt::UserRole, area >= 0 ? area : QVariant());

        countriesTable->setItem(row, 0, nameItem);
        countriesTable->setItem(row, 1, populationItem);
        countriesTable->setItem(row, 2, areaItem);
        row++;
    }

    countriesTable->sortItems(0, Qt::AscendingOrder); // Начальная сортировка по имени
}

void StatsWidget::showWorldStats() {
    if (!mainWindow || !mainWindow->getDemographicsManager()) {
        titleLabel->setText("Error: Demographics data not available");
        return;
    }

    hideAllCharts();

    populationButton->setVisible(false);
    areaButton->setVisible(false);
    densityButton->setVisible(false);

    populationLabel->setVisible(false);
    capitalLabel->setVisible(false);
    regionLabel->setVisible(false);
    subregionLabel->setVisible(false);
    areaLabel->setVisible(false);
    currenciesLabel->setVisible(false);
    languagesLabel->setVisible(false);
    timezonesLabel->setVisible(false);
    phoneCodeLabel->setVisible(false);
    tldLabel->setVisible(false);
    countryCoordinatesLabel->setVisible(false);
    capitalCoordinatesLabel->setVisible(false);
    flagLabel->setVisible(false);
    flagLabel->setPixmap(QPixmap());
    coatOfArmsLabel->setVisible(false);
    coatOfArmsLabel->setPixmap(QPixmap());
    drivingSideLabel->setVisible(false);
    editDataButton->setVisible(false); // Скрываем кнопку "Edit Data"

    countriesTable->setVisible(true);
    sortByPopulationButton->setVisible(true);
    sortByNameButton->setVisible(true);
    sortByAreaButton->setVisible(true);
    sortTimeChartView->setVisible(true);

    titleLabel->setText("World Statistics");

    populateWorldStatsTable();
}

// Реализация сортировки по площади
void StatsWidget::sortCountriesByArea() {
    QList<std::tuple<QString, qint64, qreal>> originalData;
    for (int i = 0; i < countriesTable->rowCount(); ++i) {
        QString country = countriesTable->item(i, 0)->text();
        QVariant populationVar = countriesTable->item(i, 1)->data(Qt::UserRole);
        QVariant areaVar = countriesTable->item(i, 2)->data(Qt::UserRole);

        qint64 population = populationVar.isValid() ? populationVar.toLongLong() : -1;
        qreal area = areaVar.isValid() ? areaVar.toReal() : -1.0;

        originalData.append(std::make_tuple(country, population, area));
    }

    // Замеры времени
    QElapsedTimer bubbleTimer;
    bubbleTimer.start();
    QList<std::tuple<QString, qint64, qreal>> bubbleData = originalData;
    for (int i = 0; i < bubbleData.size() - 1; ++i) {
        for (int j = 0; j < bubbleData.size() - i - 1; ++j) {
            if (std::get<2>(bubbleData[j]) < std::get<2>(bubbleData[j + 1])) {
                bubbleData.swapItemsAt(j, j + 1);
            }
        }
    }
    double bubbleTimeMs = bubbleTimer.nsecsElapsed() / 1000000.0;

    QElapsedTimer qtTimer;
    qtTimer.start();
    countriesTable->sortItems(2, Qt::DescendingOrder);
    double qtTimeMs = qtTimer.nsecsElapsed() / 1000000.0;

    QElapsedTimer stdTimer;
    stdTimer.start();
    QList<std::tuple<QString, qint64, qreal>> stdData = originalData;
    std::sort(stdData.begin(), stdData.end(), [](const std::tuple<QString, qint64, qreal> &a, const std::tuple<QString, qint64, qreal> &b) {
        qreal areaA = std::get<2>(a);
        qreal areaB = std::get<2>(b);
        // Учитываем случай, когда area < 0 (N/A)
        if (areaA < 0) return false; // N/A в конец
        if (areaB < 0) return true;  // N/A в конец
        return areaA > areaB;
    });
    double stdTimeMs = stdTimer.nsecsElapsed() / 1000000.0;

    // Обновляем таблицу
    countriesTable->setSortingEnabled(false);
    for (int i = 0; i < stdData.size(); ++i) {
        QString country = std::get<0>(stdData[i]);
        qint64 population = std::get<1>(stdData[i]);
        qreal area = std::get<2>(stdData[i]);

        countriesTable->setItem(i, 0, new QTableWidgetItem(country));
        QTableWidgetItem *popItem = new QTableWidgetItem(population >= 0 ? QString::number(population) : "N/A");
        popItem->setData(Qt::UserRole, population);
        countriesTable->setItem(i, 1, popItem);

        QTableWidgetItem *areaItem = new QTableWidgetItem(area >= 0 ? QString::number(area) : "N/A");
        areaItem->setData(Qt::UserRole, area >= 0 ? area : QVariant());
        countriesTable->setItem(i, 2, areaItem);
    }

    // Создаём график
    QChart *chart = new QChart();
    chart->setTitle("Sort by Area Time (ms)");

    QBarSeries *series = new QBarSeries();
    QBarSet *bubbleSet = new QBarSet("Bubble Sort");
    QBarSet *qtSet = new QBarSet("Qt Sort");
    QBarSet *stdSet = new QBarSet("std::sort");
    *bubbleSet << bubbleTimeMs;
    *qtSet << qtTimeMs;
    *stdSet << stdTimeMs;
    bubbleSet->setColor(Qt::blue);
    qtSet->setColor(Qt::red);
    stdSet->setColor(Qt::green);
    series->append(bubbleSet);
    series->append(qtSet);
    series->append(stdSet);

    chart->addSeries(series);
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Time (ms)");
    axisY->setRange(0, qMax(bubbleTimeMs, qMax(qtTimeMs, stdTimeMs)) * 1.1);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    sortTimeChartView->setChart(chart);
}

void StatsWidget::sortCountriesByPopulation() {
    QList<std::tuple<QString, qint64, qreal>> originalData;
    for (int i = 0; i < countriesTable->rowCount(); ++i) {
        QString country = countriesTable->item(i, 0)->text();
        QVariant populationVar = countriesTable->item(i, 1)->data(Qt::UserRole);
        QVariant areaVar = countriesTable->item(i, 2)->data(Qt::UserRole);

        qint64 population = populationVar.isValid() ? populationVar.toLongLong() : -1;
        qreal area = areaVar.isValid() ? areaVar.toReal() : -1.0;

        originalData.append(std::make_tuple(country, population, area));
    }

    // Замеры времени
    QElapsedTimer bubbleTimer;
    bubbleTimer.start();
    QList<std::tuple<QString, qint64, qreal>> bubbleData = originalData;
    for (int i = 0; i < bubbleData.size() - 1; ++i) {
        for (int j = 0; j < bubbleData.size() - i - 1; ++j) {
            if (std::get<1>(bubbleData[j]) < std::get<1>(bubbleData[j + 1])) {
                bubbleData.swapItemsAt(j, j + 1);
            }
        }
    }
    double bubbleTimeMs = bubbleTimer.nsecsElapsed() / 1000000.0;

    QElapsedTimer qtTimer;
    qtTimer.start();
    countriesTable->sortItems(1, Qt::DescendingOrder);
    double qtTimeMs = qtTimer.nsecsElapsed() / 1000000.0;

    QElapsedTimer stdTimer;
    stdTimer.start();
    QList<std::tuple<QString, qint64, qreal>> stdData = originalData;
    std::sort(stdData.begin(), stdData.end(), [](const std::tuple<QString, qint64, qreal> &a, const std::tuple<QString, qint64, qreal> &b) {
        return std::get<1>(a) > std::get<1>(b);
    });
    double stdTimeMs = stdTimer.nsecsElapsed() / 1000000.0;

    // Обновляем таблицу
    countriesTable->setSortingEnabled(false);
    for (int i = 0; i < stdData.size(); ++i) {
        QString country = std::get<0>(stdData[i]);
        qint64 population = std::get<1>(stdData[i]);
        qreal area = std::get<2>(stdData[i]);

        countriesTable->setItem(i, 0, new QTableWidgetItem(country));
        QTableWidgetItem *popItem = new QTableWidgetItem(population >= 0 ? QString::number(population) : "N/A");
        popItem->setData(Qt::UserRole, population);
        countriesTable->setItem(i, 1, popItem);

        QTableWidgetItem *areaItem = new QTableWidgetItem(area >= 0 ? QString::number(area) : "N/A");
        areaItem->setData(Qt::UserRole, area >= 0 ? area : QVariant());
        countriesTable->setItem(i, 2, areaItem);
    }

    // Создаём график
    QChart *chart = new QChart();
    chart->setTitle("Sort by Population Time (ms)");

    QBarSeries *series = new QBarSeries();
    QBarSet *bubbleSet = new QBarSet("Bubble Sort");
    QBarSet *qtSet = new QBarSet("Qt Sort");
    QBarSet *stdSet = new QBarSet("std::sort");
    *bubbleSet << bubbleTimeMs;
    *qtSet << qtTimeMs;
    *stdSet << stdTimeMs;
    bubbleSet->setColor(Qt::blue);
    qtSet->setColor(Qt::red);
    stdSet->setColor(Qt::green);
    series->append(bubbleSet);
    series->append(qtSet);
    series->append(stdSet);

    chart->addSeries(series);
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Time (ms)");
    axisY->setRange(0, qMax(bubbleTimeMs, qMax(qtTimeMs, stdTimeMs)) * 1.1);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    sortTimeChartView->setChart(chart);
}

void StatsWidget::sortCountriesByName() {
    QList<std::tuple<QString, qint64, qreal>> originalData;
    for (int i = 0; i < countriesTable->rowCount(); ++i) {
        QString country = countriesTable->item(i, 0)->text();
        QVariant populationVar = countriesTable->item(i, 1)->data(Qt::UserRole);
        QVariant areaVar = countriesTable->item(i, 2)->data(Qt::UserRole);

        qint64 population = populationVar.isValid() ? populationVar.toLongLong() : -1;
        qreal area = areaVar.isValid() ? areaVar.toReal() : -1.0;

        originalData.append(std::make_tuple(country, population, area));
    }

    // Замеры времени
    QElapsedTimer bubbleTimer;
    bubbleTimer.start();
    QList<std::tuple<QString, qint64, qreal>> bubbleData = originalData;
    for (int i = 0; i < bubbleData.size() - 1; ++i) {
        for (int j = 0; j < bubbleData.size() - i - 1; ++j) {
            if (std::get<0>(bubbleData[j]) > std::get<0>(bubbleData[j + 1])) {
                bubbleData.swapItemsAt(j, j + 1);
            }
        }
    }
    double bubbleTimeMs = bubbleTimer.nsecsElapsed() / 1000000.0;

    QElapsedTimer qtTimer;
    qtTimer.start();
    countriesTable->sortItems(0, Qt::AscendingOrder);
    double qtTimeMs = qtTimer.nsecsElapsed() / 1000000.0;

    QElapsedTimer stdTimer;
    stdTimer.start();
    QList<std::tuple<QString, qint64, qreal>> stdData = originalData;
    std::sort(stdData.begin(), stdData.end(), [](const std::tuple<QString, qint64, qreal> &a, const std::tuple<QString, qint64, qreal> &b) {
        return std::get<0>(a) < std::get<0>(b);
    });
    double stdTimeMs = stdTimer.nsecsElapsed() / 1000000.0;

    // Обновляем таблицу
    countriesTable->setSortingEnabled(false);
    for (int i = 0; i < stdData.size(); ++i) {
        QString country = std::get<0>(stdData[i]);
        qint64 population = std::get<1>(stdData[i]);
        qreal area = std::get<2>(stdData[i]);

        countriesTable->setItem(i, 0, new QTableWidgetItem(country));
        QTableWidgetItem *popItem = new QTableWidgetItem(population >= 0 ? QString::number(population) : "N/A");
        popItem->setData(Qt::UserRole, population);
        countriesTable->setItem(i, 1, popItem);

        QTableWidgetItem *areaItem = new QTableWidgetItem(area >= 0 ? QString::number(area) : "N/A");
        areaItem->setData(Qt::UserRole, area >= 0 ? area : QVariant());
        countriesTable->setItem(i, 2, areaItem);
    }

    // Создаём график
    QChart *chart = new QChart();
    chart->setTitle("Sort by Name Time (ms)");

    QBarSeries *series = new QBarSeries();
    QBarSet *bubbleSet = new QBarSet("Bubble Sort");
    QBarSet *qtSet = new QBarSet("Qt Sort");
    QBarSet *stdSet = new QBarSet("std::sort");
    *bubbleSet << bubbleTimeMs;
    *qtSet << qtTimeMs;
    *stdSet << stdTimeMs;
    bubbleSet->setColor(Qt::blue);
    qtSet->setColor(Qt::red);
    stdSet->setColor(Qt::green);
    series->append(bubbleSet);
    series->append(qtSet);
    series->append(stdSet);

    chart->addSeries(series);
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Time (ms)");
    axisY->setRange(0, qMax(bubbleTimeMs, qMax(qtTimeMs, stdTimeMs)) * 1.1);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    sortTimeChartView->setChart(chart);
}

void StatsWidget::showComparison(const QStringList &countries) {
    currentCountries = countries;

    hideAllCharts(); // Сбрасываем графики

    countriesTable->setVisible(false);
    sortByPopulationButton->setVisible(false);

    populationLabel->setVisible(false);
    capitalLabel->setVisible(false);
    regionLabel->setVisible(false);
    subregionLabel->setVisible(false);
    areaLabel->setVisible(false);
    currenciesLabel->setVisible(false);
    languagesLabel->setVisible(false);
    timezonesLabel->setVisible(false);
    phoneCodeLabel->setVisible(false);
    tldLabel->setVisible(false);
    countryCoordinatesLabel->setVisible(false);
    capitalCoordinatesLabel->setVisible(false);
    flagLabel->setVisible(false);
    flagLabel->setPixmap(QPixmap());
    coatOfArmsLabel->setVisible(false);
    coatOfArmsLabel->setPixmap(QPixmap());
    drivingSideLabel->setVisible(false);

    if (countries.isEmpty()) {
        titleLabel->setText("No countries selected for comparison");
        populationButton->setVisible(false);
        areaButton->setVisible(false);
        densityButton->setVisible(false);
        return;
    }

    titleLabel->setText("Comparison of " + QString::number(countries.size()) + " Countries");

    populationButton->setVisible(true);
    areaButton->setVisible(true);
    densityButton->setVisible(true);

    // Показываем сравнение по населению только если данные валидны
    if (mainWindow && mainWindow->getDemographicsManager()) {
        showPopulationComparison();
    } else {
        titleLabel->setText("Error: Demographics data not available");
    }
}

void StatsWidget::showCountryStats(const QString &countryName, qint64 population,
                                   const QString &capital, const QString &region, const QString &subregion,
                                   qreal area, const QStringList &currencies, const QStringList &languages,
                                   const QStringList &timezones, const QString &phoneCode, const QStringList &tld,
                                   const QPointF &countryCoordinates, const QPointF &capitalCoordinates,
                                   const QString &flagUrl, const QString &coatOfArmsUrl, const QString &drivingSide) {
    qDebug() << "Entering showCountryStats for" << countryName;

    if (!mainWindow || !mainWindow->getDemographicsManager()) {
        qDebug() << "Error: mainWindow or demographicsManager is null";
        titleLabel->setText("Error: Demographics data not available");
        return;
    }

    qDebug() << "Calling hideAllCharts";
    hideAllCharts();

    qDebug() << "Hiding UI elements";
    // Скрываем все элементы мировой статистики
    countriesTable->setVisible(false);
    sortByPopulationButton->setVisible(false);
    sortByNameButton->setVisible(false);
    sortByAreaButton->setVisible(false);
    sortTimeChartView->setVisible(false); // Скрываем график сортировки

    populationButton->setVisible(false);
    areaButton->setVisible(false);
    densityButton->setVisible(false);

    qDebug() << "Setting labels";
    titleLabel->setText(countryName.isEmpty() ? "Unknown Country" : countryName);
    populationLabel->setText("Population: " + (population >= 0 ? QString::number(population) : "N/A"));
    capitalLabel->setText("Capital: " + (capital.isEmpty() ? "N/A" : capital));
    regionLabel->setText("Region: " + (region.isEmpty() ? "N/A" : region));
    subregionLabel->setText("Subregion: " + (subregion.isEmpty() ? "N/A" : subregion));
    areaLabel->setText("Area: " + (area >= 0 ? QString::number(area) + " km²" : "N/A"));
    currenciesLabel->setText("Currencies: " + (currencies.isEmpty() ? "N/A" : currencies.join(", ")));
    languagesLabel->setText("Languages: " + (languages.isEmpty() ? "N/A" : languages.join(", ")));
    timezonesLabel->setText("Timezones: " + (timezones.isEmpty() ? "N/A" : timezones.join(", ")));
    phoneCodeLabel->setText("Phone Code: " + (phoneCode.isEmpty() ? "N/A" : phoneCode));
    tldLabel->setText("TLD: " + (tld.isEmpty() ? "N/A" : tld.join(", ")));
    countryCoordinatesLabel->setText("Country Coordinates: " + (countryCoordinates.isNull() ? "N/A" : QString("(%1, %2)").arg(countryCoordinates.x()).arg(countryCoordinates.y())));
    capitalCoordinatesLabel->setText("Capital Coordinates: " + (capitalCoordinates.isNull() ? "N/A" : QString("(%1, %2)").arg(capitalCoordinates.x()).arg(capitalCoordinates.y())));
    drivingSideLabel->setText("Driving Side: " + (drivingSide.isEmpty() ? "N/A" : drivingSide));

    qDebug() << "Clearing pixmaps";
    flagLabel->setPixmap(QPixmap());
    coatOfArmsLabel->setPixmap(QPixmap());

    qDebug() << "Starting flag request for URL:" << flagUrl;
    if (!flagUrl.isEmpty() && flagUrl != "N/A") {
        QNetworkRequest flagRequest{QUrl(flagUrl)};
        QNetworkReply *flagReply = networkManager->get(flagRequest);
        connect(flagReply, &QNetworkReply::finished, this, [this, flagReply]() {
            onFlagReplyFinished(flagReply);
        });
    } else {
        flagLabel->setText("Flag: N/A");
    }

    qDebug() << "Starting coat of arms request for URL:" << coatOfArmsUrl;
    if (!coatOfArmsUrl.isEmpty() && coatOfArmsUrl != "N/A") {
        QNetworkRequest coatRequest{QUrl(coatOfArmsUrl)};
        QNetworkReply *coatReply = networkManager->get(coatRequest);
        connect(coatReply, &QNetworkReply::finished, this, [this, coatReply]() {
            onCoatOfArmsReplyFinished(coatReply);
        });
    } else {
        coatOfArmsLabel->setText("Coat of Arms: N/A");
    }

    qDebug() << "Showing labels";
    populationLabel->setVisible(true);
    capitalLabel->setVisible(true);
    regionLabel->setVisible(true);
    subregionLabel->setVisible(true);
    areaLabel->setVisible(true);
    currenciesLabel->setVisible(true);
    languagesLabel->setVisible(true);
    timezonesLabel->setVisible(true);
    phoneCodeLabel->setVisible(true);
    tldLabel->setVisible(true);
    countryCoordinatesLabel->setVisible(true);
    capitalCoordinatesLabel->setVisible(true);
    flagLabel->setVisible(true);
    coatOfArmsLabel->setVisible(true);
    drivingSideLabel->setVisible(true);
    editDataButton->setVisible(true);

    qDebug() << "Setting current country code";
    currentCountryCode = mainWindow->getMapWidget()->getCountryCode(countryName);
    currentCountries.clear();

    qDebug() << "Exiting showCountryStats";
}

void StatsWidget::onEditDataClicked() {
    if (currentCountryCode.isEmpty()) {
        qDebug() << "No country code available for editing";
        return;
    }

    DataEditDialog *dialog = new DataEditDialog(currentCountryCode, mainWindow->getDemographicsManager(), this);
    if (dialog->exec() == QDialog::Accepted) {
        DemographicsManager::CountryData updatedData = dialog->getUpdatedData();
        mainWindow->getDemographicsManager()->updateCountryData(currentCountryCode, updatedData);

        // Обновляем отображение статистики
        showCountryStats(
            titleLabel->text(),
            updatedData.population,
            updatedData.capital,
            updatedData.region,
            updatedData.subregion,
            updatedData.area,
            updatedData.currencies,
            updatedData.languages,
            updatedData.timezones,
            updatedData.phoneCode,
            updatedData.tld,
            updatedData.countryCoordinates,
            updatedData.capitalCoordinates,
            updatedData.flagUrl,
            updatedData.coatOfArmsUrl,
            updatedData.drivingSide
            );
    }
    delete dialog;
}

void StatsWidget::showPopulationComparison() {
    QChart *chart = new QChart();
    chart->setTitle("Population Comparison");

    if (!mainWindow || !mainWindow->getDemographicsManager()) {
        titleLabel->setText("Error: Demographics data not available");
        return;
    }

    hideAllCharts();

    if (currentCountries.isEmpty()) {
        titleLabel->setText("No countries selected for comparison");
        return;
    }

    chart->setTitle("Population Comparison");

    QBarSeries *series = new QBarSeries();
    DemographicsManager *demographicsManager = mainWindow->getDemographicsManager();

    QList<QColor> colors = {Qt::blue, Qt::red, Qt::green, Qt::cyan, Qt::magenta, Qt::yellow, Qt::darkBlue, Qt::darkRed, Qt::darkGreen, Qt::darkCyan};

    qint64 maxPopulation = 0;
    int colorIndex = 0;

    for (const QString &country : currentCountries) {
        QString countryCode = mainWindow->getMapWidget()->getCountryCode(country);
        if (countryCode.isEmpty()) {
            continue;
        }
        qint64 population = demographicsManager->getPopulation(countryCode);
        if (population >= 0) {
            QBarSet *set = new QBarSet(country);
            *set << population;
            maxPopulation = qMax(maxPopulation, population);
            set->setColor(colors[colorIndex % colors.size()]);
            colorIndex++;
            series->append(set);
        }
    }

    if (series->count() == 0) {
        titleLabel->setText("No valid population data for comparison");
        delete chart;
        return;
    }

    chart->addSeries(series);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Population");
    axisY->setRange(0, maxPopulation * 1.1);
    chart->addAxis(axisY, Qt::AlignLeft);

    series->attachAxis(axisY);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    chart->addAxis(axisX, Qt::AlignBottom);

    series->attachAxis(axisX);

    populationChartView->setChart(chart);
    populationChartView->setVisible(true);
}

void StatsWidget::showAreaComparison() {
    hideAllCharts();

    if (currentCountries.isEmpty()) {
        titleLabel->setText("No countries selected for comparison");
        return;
    }

    QChart *chart = new QChart();
    chart->setTitle("Area Comparison");

    QBarSeries *series = new QBarSeries();
    series->setParent(chart);

    DemographicsManager *demographicsManager = mainWindow->getDemographicsManager();
    QList<QColor> colors = {Qt::blue, Qt::red, Qt::green, Qt::cyan, Qt::magenta, Qt::yellow, Qt::darkBlue, Qt::darkRed, Qt::darkGreen, Qt::darkCyan};

    qreal maxArea = 0;
    int colorIndex = 0;

    for (const QString &country : currentCountries) {
        QString countryCode = mainWindow->getMapWidget()->getCountryCode(country);
        if (countryCode.isEmpty()) {
            continue;
        }

        QBarSet *set = new QBarSet(country);
        set->setParent(series);
        qreal area = demographicsManager->getArea(countryCode);
        if (area >= 0) {
            *set << area;
            maxArea = qMax(maxArea, area);
            set->setColor(colors[colorIndex % colors.size()]);
            colorIndex++;
            series->append(set);
        } else {
            delete set;
        }
    }

    if (series->count() == 0) {
        titleLabel->setText("No area data available for comparison");
        delete chart;
        return;
    }

    chart->addSeries(series);

    QValueAxis *axisY = new QValueAxis();
    axisY->setParent(chart);
    axisY->setTitleText("Area (km²)");
    axisY->setRange(0, maxArea * 1.1);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->setParent(chart);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    areaChartView->setChart(chart);
    areaChartView->setVisible(true);
}

void StatsWidget::showDensityComparison() {
    hideAllCharts();

    if (currentCountries.isEmpty()) {
        titleLabel->setText("No countries selected for comparison");
        return;
    }

    QChart *chart = new QChart();
    chart->setTitle("Population Density Comparison");

    QBarSeries *series = new QBarSeries();
    series->setParent(chart);

    DemographicsManager *demographicsManager = mainWindow->getDemographicsManager();
    QList<QColor> colors = {Qt::blue, Qt::red, Qt::green, Qt::cyan, Qt::magenta, Qt::yellow, Qt::darkBlue, Qt::darkRed, Qt::darkGreen, Qt::darkCyan};

    qreal maxDensity = 0;
    int colorIndex = 0;

    for (const QString &country : currentCountries) {
        QString countryCode = mainWindow->getMapWidget()->getCountryCode(country);
        if (countryCode.isEmpty()) {
            continue;
        }

        QBarSet *set = new QBarSet(country);
        set->setParent(series);
        qint64 population = demographicsManager->getPopulation(countryCode);
        qreal area = demographicsManager->getArea(countryCode);
        if (population >= 0 && area > 0) {
            qreal density = population / area;
            *set << density;
            maxDensity = qMax(maxDensity, density);
            set->setColor(colors[colorIndex % colors.size()]);
            colorIndex++;
            series->append(set);
        } else {
            delete set;
        }
    }

    if (series->count() == 0) {
        titleLabel->setText("No density data available for comparison");
        delete chart;
        return;
    }

    chart->addSeries(series);

    QValueAxis *axisY = new QValueAxis();
    axisY->setParent(chart);
    axisY->setTitleText("Density (people/km²)");
    axisY->setRange(0, maxDensity * 1.1);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->setParent(chart);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    densityChartView->setChart(chart);
    densityChartView->setVisible(true);
}

void StatsWidget::onFlagReplyFinished(QNetworkReply *reply) {
    qDebug() << "Processing flag reply";
    if (!flagLabel) {
        qDebug() << "Error: flagLabel is null";
        reply->deleteLater();
        return;
    }
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        QPixmap pixmap;
        if (pixmap.loadFromData(data)) {
            pixmap = pixmap.scaled(100, 100, Qt::KeepAspectRatio);
            flagLabel->setPixmap(pixmap);
            qDebug() << "Flag loaded successfully";
        } else {
            flagLabel->setText("Flag: Failed to load");
            qDebug() << "Flag failed to load pixmap";
        }
    } else {
        flagLabel->setText("Flag: Failed to load - " + reply->errorString());
        qDebug() << "Flag request error:" << reply->errorString();
    }
    reply->deleteLater();
}

void StatsWidget::onCoatOfArmsReplyFinished(QNetworkReply *reply) {
    qDebug() << "Processing coat of arms reply";
    if (!coatOfArmsLabel) {
        qDebug() << "Error: coatOfArmsLabel is null";
        reply->deleteLater();
        return;
    }
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        QPixmap pixmap;
        if (pixmap.loadFromData(data)) {
            pixmap = pixmap.scaled(100, 100, Qt::KeepAspectRatio);
            coatOfArmsLabel->setPixmap(pixmap);
            qDebug() << "Coat of arms loaded successfully";
        } else {
            coatOfArmsLabel->setText("Coat of Arms: Failed to load");
            qDebug() << "Coat of arms failed to load pixmap";
        }
    } else {
        coatOfArmsLabel->setText("Coat of Arms: Failed to load - " + reply->errorString());
        qDebug() << "Coat of arms request error:" << reply->errorString();
    }
    reply->deleteLater();
}
