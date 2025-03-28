#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include "MapWidget.h"
#include "StatsWidget.h"
#include "DataLoader.h"
#include "DemographicsManager.h"
#include "VerticalButton.h"
#include "DocumentationDialog.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

    // Геттеры для mapWidget, demographicsManager и selectedCountries
    MapWidget* getMapWidget() const { return mapWidget; }
    DemographicsManager* getDemographicsManager() const { return demographicsManager; }
    QStringList getSelectedCountries() const { return selectedCountries; }

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

public slots:
    void handleDataLoaded(const QJsonDocument &doc);
    void handleCountryCodesLoaded(const QMap<QString, QString> &codes);
    void handleDebugMessage(const QString &message);
    void showCountryStats(const QString &countryName, qint64 population);
    void toggleStatsPanel();
    void onShowStatsRequested();
    void refreshData();
    void onZoomSliderValueChanged(int value);
    void onZoomInClicked();
    void onZoomOutClicked();
    void updateZoomSlider(qreal newScale);
    void toggleCompareMode();
    void showFindCountryDialog();

    void openNewWindow();

    // Слоты для меню "Window"
    void setFullScreen();
    void setLeftHalf();
    void setRightHalf();
    void setTopHalf();
    void setBottomHalf();
    void restoreDefaultWindow();

    // Слоты для меню "View"
    void fitToWindow();
    void showStatsPanel();
    void hideStatsPanel();

    // Слоты для меню "Data"
    void showWorldStats();

    // Слоты для меню "Help"
    void showAbout();
    void showDocumentation();

private:
    void updateToggleButtonPosition();
    void updateZoomSliderPosition();
    void syncZoomSlider();
    void updateCompareButtonPosition();
    void updateFindButtonPosition(); // Функция для позиционирования кнопки Find
    static constexpr qreal INITIAL_SCALE = 5.5; // Начальный масштаб при запуске

    MapWidget *mapWidget;
    StatsWidget *statsWidget;
    QDockWidget *statsDock;
    DataLoader *dataLoader;
    DemographicsManager *demographicsManager;
    VerticalButton *toggleButton;
    QSlider *zoomSlider;
    QPushButton *zoomInButton;
    QPushButton *zoomOutButton;
    QPushButton *compareButton; // Кнопка Compare
    QPushButton *findButton; // Кнопка Find
    QString lastSelectedCountry;
    bool isStatsVisible = false;
    bool isCompareMode = false;
    QStringList selectedCountries;
};

#endif // MAINWINDOW_H
