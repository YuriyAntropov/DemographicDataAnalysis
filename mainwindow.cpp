#include "MainWindow.h"
#include <QDockWidget>
#include <QMenu>
#include <QMenuBar>
#include <QResizeEvent>
#include <QDebug>
#include <QApplication>
#include <QScreen>
#include <QMessageBox>
#include <QProcess> // Добавляем для QProcess
#include "FindCountryDialog.h" // Добавляем новый заголовок

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

    // Устанавливаем стиль для всего окна
    setStyleSheet(
        "QMainWindow { background-color: #212121; }"
        "QPushButton { background-color: #424242; color: #E0E0E0; border-radius: 5px; padding: 5px; }"
        "QPushButton:hover { background-color: #42A5F5; color: #FFFFFF; }"
        "QPushButton:pressed { background-color: #1E88E5; }"
        "QSlider::groove:vertical { background: #424242; width: 6px; border-radius: 3px; }"
        "QSlider::handle:vertical { background: #42A5F5; height: 20px; width: 20px; margin: -5px 0; border-radius: 10px; }"
        "QSlider::handle:vertical:hover { background: #1E88E5; }"
        "QMenuBar { background-color: #2E2E2E; color: #E0E0E0; }"
        "QMenu { background-color: #2E2E2E; color: #E0E0E0; }"
        "QMenu::item:selected { background-color: #42A5F5; color: #FFFFFF; }"
        );



    demographicsManager = new DemographicsManager(this);
    mapWidget = new MapWidget(demographicsManager, this);
    setCentralWidget(mapWidget);

    statsWidget = new StatsWidget(this, this);
    statsDock = new QDockWidget("Statistics", this);
    statsDock->setWidget(statsWidget);
    statsDock->setMinimumWidth(400);
    statsDock->setMaximumWidth(400);
    statsDock->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    statsDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    statsDock->setVisible(false);
    addDockWidget(Qt::RightDockWidgetArea, statsDock);
    statsDock->installEventFilter(this);

    dataLoader = new DataLoader(this);
    connect(dataLoader, &DataLoader::dataLoaded, this, &MainWindow::handleDataLoaded);
    connect(dataLoader, &DataLoader::countryCodesLoaded, this, &MainWindow::handleCountryCodesLoaded);
    connect(dataLoader, &DataLoader::debugMessage, this, &MainWindow::handleDebugMessage);

    connect(mapWidget, &MapWidget::countryClicked, this, &MainWindow::showCountryStats);
    connect(mapWidget, &MapWidget::showStatsRequested, this, &MainWindow::onShowStatsRequested);
    connect(mapWidget, &MapWidget::scaleChanged, this, &MainWindow::updateZoomSlider);

    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    // Меню "File"
    QMenu *fileMenu = menuBar->addMenu("File");
    QAction *duplicateAction = fileMenu->addAction("Duplicate Window"); // Дублировать окно
    QAction *closeAction = fileMenu->addAction("Close");                // Закрыть окно
    connect(duplicateAction, &QAction::triggered, this, &MainWindow::openNewWindow); // Подключаем слот
    connect(closeAction, &QAction::triggered, this, &QMainWindow::close);           // Закрываем окно

    // Меню "Window"
    QMenu *windowMenu = menuBar->addMenu("Window");
    QAction *fullScreenAction = windowMenu->addAction("Full Screen");
    QAction *leftHalfAction = windowMenu->addAction("Left Half of Screen");
    QAction *rightHalfAction = windowMenu->addAction("Right Half of Screen");
    QAction *topHalfAction = windowMenu->addAction("Top Half of Screen");
    QAction *bottomHalfAction = windowMenu->addAction("Bottom Half of Screen");
    QAction *restoreDefaultAction = windowMenu->addAction("Restore Default");
    connect(fullScreenAction, &QAction::triggered, this, &MainWindow::setFullScreen);
    connect(leftHalfAction, &QAction::triggered, this, &MainWindow::setLeftHalf);
    connect(rightHalfAction, &QAction::triggered, this, &MainWindow::setRightHalf);
    connect(topHalfAction, &QAction::triggered, this, &MainWindow::setTopHalf);
    connect(bottomHalfAction, &QAction::triggered, this, &MainWindow::setBottomHalf); // Здесь была опечатка "customHalfAction", исправим ниже
    connect(restoreDefaultAction, &QAction::triggered, this, &MainWindow::restoreDefaultWindow);

    // Меню "View"
    QMenu *viewMenu = menuBar->addMenu("View");
    QAction *zoomInAction = viewMenu->addAction("Zoom In");
    QAction *zoomOutAction = viewMenu->addAction("Zoom Out");
    QAction *fitToWindowAction = viewMenu->addAction("Fit to Window");
    QAction *showStatsAction = viewMenu->addAction("Show Stats Panel");
    QAction *hideStatsAction = viewMenu->addAction("Hide Stats Panel");
    QAction *toggleCompareAction = viewMenu->addAction("Toggle Compare Mode");
    connect(zoomInAction, &QAction::triggered, this, &MainWindow::onZoomInClicked);
    connect(zoomOutAction, &QAction::triggered, this, &MainWindow::onZoomOutClicked);
    connect(fitToWindowAction, &QAction::triggered, this, &MainWindow::fitToWindow);
    connect(showStatsAction, &QAction::triggered, this, &MainWindow::showStatsPanel);
    connect(hideStatsAction, &QAction::triggered, this, &MainWindow::hideStatsPanel);
    connect(toggleCompareAction, &QAction::triggered, this, &MainWindow::toggleCompareMode);

    // Меню "Data"
    QMenu *dataMenu = menuBar->addMenu("Data");
    QAction *refreshAction = dataMenu->addAction("Refresh Data"); // Перенесли сюда
    QAction *showWorldStatsAction = dataMenu->addAction("Show World Stats");
    connect(refreshAction, &QAction::triggered, this, &MainWindow::refreshData); // Подключаем слот
    connect(showWorldStatsAction, &QAction::triggered, this, &MainWindow::showWorldStats);

    // Меню "Help"
    QMenu *helpMenu = menuBar->addMenu("Help");
    QAction *aboutAction = helpMenu->addAction("About");
    QAction *documentationAction = helpMenu->addAction("Documentation");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
    connect(documentationAction, &QAction::triggered, this, &MainWindow::showDocumentation);

    toggleButton = new VerticalButton(this);
    toggleButton->setText("Show Stats ▲");
    toggleButton->setFixedSize(30, height() / 2);
    connect(toggleButton, &QPushButton::clicked, this, &MainWindow::toggleStatsPanel);

    zoomSlider = new QSlider(Qt::Vertical, this);
    syncZoomSlider();
    zoomSlider->setFixedSize(30, 200);
    connect(zoomSlider, &QSlider::valueChanged, this, &MainWindow::onZoomSliderValueChanged);

    zoomInButton = new QPushButton("+", this);
    zoomInButton->setFixedSize(30, 30);
    zoomInButton->setCursor(Qt::PointingHandCursor);
    connect(zoomInButton, &QPushButton::clicked, this, &MainWindow::onZoomInClicked);

    zoomOutButton = new QPushButton("−", this);
    zoomOutButton->setFixedSize(30, 30);
    zoomOutButton->setCursor(Qt::PointingHandCursor);
    connect(zoomOutButton, &QPushButton::clicked, this, &MainWindow::onZoomOutClicked);

    compareButton = new QPushButton("Compare", this);
    compareButton->setFixedSize(100, 30);
    connect(compareButton, &QPushButton::clicked, this, &MainWindow::toggleCompareMode);

    findButton = new QPushButton("Find", this);
    findButton->setFixedSize(100, 30);
    connect(findButton, &QPushButton::clicked, this, &MainWindow::showFindCountryDialog);

    updateToggleButtonPosition();
    updateZoomSliderPosition();
    updateCompareButtonPosition();
    updateFindButtonPosition();

    dataLoader->loadCountryCodes();
    dataLoader->loadData();

    resize(1920, 1080);
    mapWidget->fitToWindow();
}

void MainWindow::openNewWindow() {
    QProcess::startDetached(QApplication::applicationFilePath()); // Запускаем новый экземпляр программы
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    updateToggleButtonPosition();
    updateZoomSliderPosition();
    updateCompareButtonPosition();
    updateFindButtonPosition(); // Добавляем обновление позиции кнопки Find
    toggleButton->setFixedHeight(height() / 2);
    mapWidget->fitToWindow();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    // Можно добавить логику подтверждения выхода
    event->accept(); // Либо event->ignore(); если хотите предотвратить закрытие
}

void MainWindow::toggleCompareMode() {
    isCompareMode = !isCompareMode;
    mapWidget->setCompareMode(isCompareMode);
    compareButton->setText(isCompareMode ? "Disable Compare" : "Compare");
    if (!isCompareMode) {
        selectedCountries.clear();
        mapWidget->clearSelection();
    }
    if (isStatsVisible) {
        if (isCompareMode) {
            statsWidget->showComparison(mapWidget->getSelectedCountries());
        } else if (!lastSelectedCountry.isEmpty()) {
            QString countryCode = mapWidget->getCountryCode(lastSelectedCountry);
            qint64 population = demographicsManager->getPopulation(countryCode);
            QString capital = demographicsManager->getCapital(countryCode);
            QString region = demographicsManager->getRegion(countryCode);
            QString subregion = demographicsManager->getSubregion(countryCode);
            qreal area = demographicsManager->getArea(countryCode);
            QStringList currencies = demographicsManager->getCurrencies(countryCode);
            QStringList languages = demographicsManager->getLanguages(countryCode);
            QStringList timezones = demographicsManager->getTimezones(countryCode);
            QString phoneCode = demographicsManager->getPhoneCode(countryCode);
            QStringList tld = demographicsManager->getTLD(countryCode);
            QPointF countryCoordinates = demographicsManager->getCountryCoordinates(countryCode);
            QPointF capitalCoordinates = demographicsManager->getCapitalCoordinates(countryCode);
            QString flagUrl = demographicsManager->getFlagUrl(countryCode);
            QString coatOfArmsUrl = demographicsManager->getCoatOfArmsUrl(countryCode);
            QString drivingSide = demographicsManager->getDrivingSide(countryCode);
            statsWidget->showCountryStats(lastSelectedCountry, population, capital, region, subregion, area, currencies, languages, timezones,
                                          phoneCode, tld, countryCoordinates, capitalCoordinates, flagUrl, coatOfArmsUrl, drivingSide);
        } else {
            statsWidget->showWorldStats();
        }
    }
    update();
}

void MainWindow::handleDataLoaded(const QJsonDocument &data) {
    demographicsManager->loadFromJson(data);
}

void MainWindow::handleCountryCodesLoaded(const QMap<QString, QString> &countryCodes) {
    mapWidget->setCountryCodes(countryCodes);
}

void MainWindow::showCountryStats(const QString &countryName, qint64 population) {
    QString countryCode = mapWidget->getCountryCode(countryName);
    if (countryCode.isEmpty()) {
        qDebug() << "Country code not found for" << countryName << "- cannot show stats";
        population = -1;
    } else {
        qDebug() << "Found country code" << countryCode << "for" << countryName;
        population = demographicsManager->getPopulation(countryCode);
        qDebug() << "Population for" << countryCode << ":" << population;
    }

    if (!isCompareMode) {
        lastSelectedCountry = countryName;

        QString capital = demographicsManager->getCapital(countryCode);
        QString region = demographicsManager->getRegion(countryCode);
        QString subregion = demographicsManager->getSubregion(countryCode);
        qreal area = demographicsManager->getArea(countryCode);
        QStringList currencies = demographicsManager->getCurrencies(countryCode);
        QStringList languages = demographicsManager->getLanguages(countryCode);
        QStringList timezones = demographicsManager->getTimezones(countryCode);
        QString phoneCode = demographicsManager->getPhoneCode(countryCode);
        QStringList tld = demographicsManager->getTLD(countryCode);
        QPointF countryCoordinates = demographicsManager->getCountryCoordinates(countryCode);
        QPointF capitalCoordinates = demographicsManager->getCapitalCoordinates(countryCode);
        QString flagUrl = demographicsManager->getFlagUrl(countryCode);
        QString coatOfArmsUrl = demographicsManager->getCoatOfArmsUrl(countryCode);
        QString drivingSide = demographicsManager->getDrivingSide(countryCode);

        qDebug() << "Stats for" << countryName << "- Population:" << population
                 << "Area:" << area << "Capital:" << capital;

        QPointF centroid = mapWidget->getCountryCentroid(countryName);
        if (!centroid.isNull()) {
            mapWidget->showInfoBubble(countryName, centroid, population);
        }

        if (isStatsVisible) {
            statsWidget->showCountryStats(countryName, population, capital, region, subregion, area, currencies, languages, timezones,
                                          phoneCode, tld, countryCoordinates, capitalCoordinates, flagUrl, coatOfArmsUrl, drivingSide);
        }
    } else {
        if (!selectedCountries.contains(countryName)) {
            selectedCountries.append(countryName);
        }
        if (isStatsVisible) {
            statsWidget->showComparison(mapWidget->getSelectedCountries());
        }
    }
}

void MainWindow::refreshData() {
    dataLoader->loadData();
}

void MainWindow::toggleStatsPanel() {
    if (isStatsVisible) {
        statsDock->setVisible(false);
        toggleButton->setText("Show Stats ▲");
        isStatsVisible = false;
        findButton->setVisible(true);    // Показываем кнопку "Найти"
        compareButton->setVisible(true); // Показываем кнопку "Сравнить"
    } else {
        statsDock->setVisible(true);
        toggleButton->setText("Hide Stats ▼");
        isStatsVisible = true;
        findButton->setVisible(false);    // Скрываем кнопку "Найти"
        compareButton->setVisible(false); // Скрываем кнопку "Сравнить"

        if (!statsWidget || !demographicsManager || !mapWidget) {
            qDebug() << "Error: Required components not initialized";
            return;
        }

        if (isCompareMode) {
            statsWidget->showComparison(mapWidget->getSelectedCountries());
        } else if (!lastSelectedCountry.isEmpty()) {
            QString countryCode = mapWidget->getCountryCode(lastSelectedCountry);
            if (!countryCode.isEmpty()) {
                qint64 population = demographicsManager->getPopulation(countryCode);
                QString capital = demographicsManager->getCapital(countryCode);
                QString region = demographicsManager->getRegion(countryCode);
                QString subregion = demographicsManager->getSubregion(countryCode);
                qreal area = demographicsManager->getArea(countryCode);
                QStringList currencies = demographicsManager->getCurrencies(countryCode);
                QStringList languages = demographicsManager->getLanguages(countryCode);
                QStringList timezones = demographicsManager->getTimezones(countryCode);
                QString phoneCode = demographicsManager->getPhoneCode(countryCode);
                QStringList tld = demographicsManager->getTLD(countryCode);
                QPointF countryCoordinates = demographicsManager->getCountryCoordinates(countryCode);
                QPointF capitalCoordinates = demographicsManager->getCapitalCoordinates(countryCode);
                QString flagUrl = demographicsManager->getFlagUrl(countryCode);
                QString coatOfArmsUrl = demographicsManager->getCoatOfArmsUrl(countryCode);
                QString drivingSide = demographicsManager->getDrivingSide(countryCode);
                statsWidget->showCountryStats(lastSelectedCountry, population, capital, region, subregion, area, currencies, languages, timezones,
                                              phoneCode, tld, countryCoordinates, capitalCoordinates, flagUrl, coatOfArmsUrl, drivingSide);
            } else {
                statsWidget->showWorldStats();
            }
        } else {
            statsWidget->showWorldStats();
        }
    }
    updateToggleButtonPosition();
}

void MainWindow::onShowStatsRequested() {
    if (!isStatsVisible) {
        statsDock->setVisible(true);
        isStatsVisible = true;
        toggleButton->setText("Hide Stats ▼");
        updateToggleButtonPosition();

        if (isCompareMode) {
            statsWidget->showComparison(mapWidget->getSelectedCountries());
        } else if (!lastSelectedCountry.isEmpty()) {
            QString countryCode = mapWidget->getCountryCode(lastSelectedCountry);
            qint64 population = demographicsManager->getPopulation(countryCode);
            QString capital = demographicsManager->getCapital(countryCode);
            QString region = demographicsManager->getRegion(countryCode);
            QString subregion = demographicsManager->getSubregion(countryCode);
            qreal area = demographicsManager->getArea(countryCode);
            QStringList currencies = demographicsManager->getCurrencies(countryCode);
            QStringList languages = demographicsManager->getLanguages(countryCode);
            QStringList timezones = demographicsManager->getTimezones(countryCode);
            QString phoneCode = demographicsManager->getPhoneCode(countryCode);
            QStringList tld = demographicsManager->getTLD(countryCode);
            QPointF countryCoordinates = demographicsManager->getCountryCoordinates(countryCode);
            QPointF capitalCoordinates = demographicsManager->getCapitalCoordinates(countryCode);
            QString flagUrl = demographicsManager->getFlagUrl(countryCode);
            QString coatOfArmsUrl = demographicsManager->getCoatOfArmsUrl(countryCode);
            QString drivingSide = demographicsManager->getDrivingSide(countryCode);
            statsWidget->showCountryStats(lastSelectedCountry, population, capital, region, subregion, area, currencies, languages, timezones,
                                          phoneCode, tld, countryCoordinates, capitalCoordinates, flagUrl, coatOfArmsUrl, drivingSide);
        } else {
            statsWidget->showWorldStats(); // Показываем мировую статистику, если нет выбранной страны
        }
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (obj == statsDock && event->type() == QEvent::Resize) {
        updateToggleButtonPosition();
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::updateToggleButtonPosition() {
    if (isStatsVisible) {
        QRect dockGeometry = statsDock->geometry();
        int dockLeft = dockGeometry.left();
        toggleButton->move(dockLeft - toggleButton->width(), (height() - toggleButton->height()) / 2);
    } else {
        toggleButton->move(width() - toggleButton->width(), (height() - toggleButton->height()) / 2);
    }
}

void MainWindow::updateCompareButtonPosition() {
    compareButton->move(width() - compareButton->width() - 10, 50);
}

void MainWindow::updateFindButtonPosition() {
    findButton->move(width() - compareButton->width() - findButton->width() - 20, 50); // Слева от Compare с отступом 10 пикселей
}

void MainWindow::updateZoomSlider(qreal newScale) {
    zoomSlider->blockSignals(true);
    zoomSlider->setValue(qBound(INITIAL_SCALE * 10, newScale * 10, mapWidget->getMaxScale() * 10));
    zoomSlider->blockSignals(false);
}

void MainWindow::onZoomSliderValueChanged(int value) {
    qreal newScale = value * 0.1;
    newScale = qBound(INITIAL_SCALE, newScale, mapWidget->getMaxScale());
    QPointF windowCenter(mapWidget->width() / 2.0, mapWidget->height() / 2.0);
    mapWidget->setScale(newScale, windowCenter);
}

void MainWindow::handleDebugMessage(const QString &message) {
    qDebug() << "Debug:" << message;
}

void MainWindow::syncZoomSlider() {
    qreal minScale = INITIAL_SCALE; // Минимальный масштаб теперь 5.5
    qreal maxScale = mapWidget->getMaxScale();
    zoomSlider->setRange(minScale * 10, maxScale * 10);
    zoomSlider->setValue(INITIAL_SCALE * 10); // Устанавливаем начальное значение
    qDebug() << "Zoom slider range set to:" << minScale * 10 << "to" << maxScale * 10;
}

void MainWindow::updateZoomSliderPosition() {
    zoomSlider->move(10, height() - zoomSlider->height() - 10 - zoomOutButton->height() - 5);
    zoomInButton->move(10, zoomSlider->y() - zoomInButton->height() - 5);
    zoomOutButton->move(10, zoomSlider->y() + zoomSlider->height() + 5);
}

void MainWindow::onZoomInClicked() {
    qreal currentScale = mapWidget->getScale();
    qreal newScale = currentScale + 1.0;
    newScale = qBound(mapWidget->getMinScale(), newScale, mapWidget->getMaxScale());
    QPointF windowCenter(mapWidget->width() / 2.0, mapWidget->height() / 2.0);
    mapWidget->setScale(newScale, windowCenter);
}

void MainWindow::onZoomOutClicked() {
    qreal currentScale = mapWidget->getScale();
    qreal newScale = currentScale - 1.0;
    newScale = qBound(INITIAL_SCALE, newScale, mapWidget->getMaxScale());
    QPointF windowCenter(mapWidget->width() / 2.0, mapWidget->height() / 2.0);
    mapWidget->setScale(newScale, windowCenter);
}

void MainWindow::setFullScreen() {
    if (isFullScreen()) {
        showNormal();
    } else {
        showFullScreen();
    }
}

void MainWindow::setLeftHalf() {
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int width = screenGeometry.width() / 2;
    int height = screenGeometry.height();
    setGeometry(screenGeometry.x(), screenGeometry.y(), width, height);
}

void MainWindow::setRightHalf() {
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int width = screenGeometry.width() / 2;
    int height = screenGeometry.height();
    setGeometry(screenGeometry.x() + width, screenGeometry.y(), width, height);
}

void MainWindow::setTopHalf() {
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int width = screenGeometry.width();
    int height = screenGeometry.height() / 2;
    setGeometry(screenGeometry.x(), screenGeometry.y(), width, height);
}

void MainWindow::setBottomHalf() {
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int width = screenGeometry.width();
    int height = screenGeometry.height() / 2;
    setGeometry(screenGeometry.x(), screenGeometry.y() + height, width, height);
}

void MainWindow::restoreDefaultWindow() {
    showNormal();
    resize(1920, 1080);
    move(0, 0);
}

void MainWindow::fitToWindow() {
    mapWidget->fitToWindow();
}

void MainWindow::showFindCountryDialog() {
    FindCountryDialog *dialog = new FindCountryDialog(this);
    if (dialog->exec() == QDialog::Accepted) {
        QString countryName = dialog->getCountryName();
        if (!countryName.isEmpty()) {
            QString countryCode = mapWidget->getCountryCode(countryName);
            if (!countryCode.isEmpty()) {
                // Подсвечиваем страну красным, увеличиваем и центрируем камеру
                mapWidget->highlightCountry(countryName, true);

                // Получаем данные о стране
                qint64 population = demographicsManager->getPopulation(countryCode);
                QString capital = demographicsManager->getCapital(countryCode);
                QString region = demographicsManager->getRegion(countryCode);
                QString subregion = demographicsManager->getSubregion(countryCode);
                qreal area = demographicsManager->getArea(countryCode);
                QStringList currencies = demographicsManager->getCurrencies(countryCode);
                QStringList languages = demographicsManager->getLanguages(countryCode);
                QStringList timezones = demographicsManager->getTimezones(countryCode);
                QString phoneCode = demographicsManager->getPhoneCode(countryCode);
                QStringList tld = demographicsManager->getTLD(countryCode);
                QPointF countryCoordinates = demographicsManager->getCountryCoordinates(countryCode);
                QPointF capitalCoordinates = demographicsManager->getCapitalCoordinates(countryCode);
                QString flagUrl = demographicsManager->getFlagUrl(countryCode);
                QString coatOfArmsUrl = demographicsManager->getCoatOfArmsUrl(countryCode);
                QString drivingSide = demographicsManager->getDrivingSide(countryCode);

                // Показываем статистику
                if (!isStatsVisible) {
                    toggleStatsPanel();
                }
                statsWidget->showCountryStats(countryName, population, capital, region, subregion, area, currencies, languages, timezones,
                                              phoneCode, tld, countryCoordinates, capitalCoordinates, flagUrl, coatOfArmsUrl, drivingSide);

                lastSelectedCountry = countryName;
            } else {
                QMessageBox::warning(this, "Country Not Found", "Country '" + countryName + "' not found in the database.");
            }
        }
    }
    delete dialog;
}

void MainWindow::showStatsPanel() {
    if (!isStatsVisible) {
        toggleStatsPanel();
    }
}

void MainWindow::hideStatsPanel() {
    if (isStatsVisible) {
        toggleStatsPanel();
    }
}

void MainWindow::showWorldStats() {
    if (!isStatsVisible) {
        toggleStatsPanel();
    }
    statsWidget->showWorldStats();
}

void MainWindow::showAbout() {
    QMessageBox::about(this, "About", "Demographic Data Analysis App\nVersion 1.0\nDeveloped by Antropov Yuriy");
}

void MainWindow::showDocumentation() {
    DocumentationDialog *dialog = new DocumentationDialog(this);
    dialog->exec(); // Открываем модальное окно
}
