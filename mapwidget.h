#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include "AbstractWidget.h" // Заменяем QWidget на AbstractWidget
#include <QMap>
#include <QPointF>
#include <QList>
#include <QNetworkReply>
#include "DemographicsManager.h"
#include "CustomException.h" // Добавляем заголовок исключений

class InfoBubble;

class MapWidget : public AbstractWidget {
    Q_OBJECT
public:
    struct Country {
        QString name;
        qreal area;
        QVector<QVector<QPointF>> polygons;
        QPointF centroid;
        qreal landWidth;
        QVector<QPointF> largestPolygon;
    };

    explicit MapWidget(DemographicsManager *demographicsManager, QWidget *parent = nullptr);
    void setCountryCodes(const QMap<QString, QString> &codes);
    QString getCountryCode(const QString &countryName) const;
    QPointF getCountryCentroid(const QString &countryName) const;
    void fitToWindow();
    void setScale(qreal newScale, const QPointF &focusPoint);
    qreal getScale() const { return scale; }
    qreal getMinScale() const { return 5.5; } // Минимальный масштаб теперь 5.5
    qreal getMaxScale() const { return 100.0; }
    void clearSelection();
    void setCompareMode(bool enabled);
    QStringList getSelectedCountries() const { return selectedCountries; }
    void showInfoBubble(const QString &countryName, const QPointF &position, qint64 population);
    void highlightCountry(const QString &countryName, bool zoom = false); // Новый метод

    const QMap<QString, QString>& getCountryCodes() const { return countryCodes; }

    // Реализация чисто виртуального метода
    void updateDisplay() override;

signals:
    void countryClicked(const QString &countryName, qint64 population);
    void showStatsRequested();
    void scaleChanged(qreal newScale);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private slots:
    void onMoreStatsClicked();

private:
    void loadGeoJson(const QString &filePath);
    QString normalizeCountryName(const QString &name) const;
    void clampOffset();
    void hideInfoBubble();
    qreal getMapWidth() const { return 360.0; }
    qreal calculateCountryWidth(const Country &country) const;
    qreal calculatePolygonArea(const QVector<QPointF> &points) const;
    qreal calculatePolygonWidth(const QVector<QPointF> &points) const;
    bool isPointInsidePolygon(const QPointF &point, const QVector<QPointF> &polygon) const;
    bool isTextInsidePolygon(const QString &text, const QPointF &textPos, const QFontMetrics &fm, const QVector<QPointF> &polygon, qreal scale) const;
    QPointF findPoleOfInaccessibility(const QVector<QPointF> &points) const;
    QPointF calculateCentroid(const QVector<QPointF> &points) const;

    QMap<QString, QString> countryCodes;
    QStringList selectedCountries;
    QPointF panOffset;
    qreal scale = 1.0;
    bool isCompareMode = false;
    QPointF bubblePosition;
    QString bubbleCountryName;
    qint64 bubblePopulation = 0;
    bool showBubble = false;
    bool isPanning = false;
    QPointF lastMousePos;
    QString hoveredCountry;
    QString selectedCountry;
    QString currentCountry;
    InfoBubble *infoBubble;
    QList<Country> countries;
    QMap<QString, qreal> countryScales;
    qreal minLat = 90;
    qreal maxLat = -90;
    DemographicsManager *demographicsManager;
};

#endif // MAPWIDGET_H
