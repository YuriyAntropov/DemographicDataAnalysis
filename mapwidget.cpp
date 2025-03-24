#include "MapWidget.h"
#include "InfoBubble.h"
#include <QPainter>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDebug>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QNetworkReply>
#include <cmath>

MapWidget::MapWidget(DemographicsManager *demographicsManager, QWidget *parent)
    : AbstractWidget(parent), demographicsManager(demographicsManager) {
    try {
        loadGeoJson("custom.geo.json");
    } catch (const CustomException& e) {
        qDebug() << "Error in MapWidget constructor:" << e.getErrorMessage() << "(Type:" << static_cast<int>(e.getErrorType()) << ")";
        // Можно добавить обработку, например, показать сообщение пользователю
    }
    scale = 5.5; // Устанавливаем начальный масштаб 5.5
    hoveredCountry = "";
    selectedCountry = "";

    infoBubble = new InfoBubble(this);
    infoBubble->hide();
    connect(infoBubble, &InfoBubble::moreStatsClicked, this, &MapWidget::onMoreStatsClicked);

    setMouseTracking(true);
}

// Реализация чисто виртуального метода
void MapWidget::updateDisplay() {
    update(); // Просто вызываем перерисовку виджета
}

void MapWidget::setCountryCodes(const QMap<QString, QString> &codes) {
    countryCodes.clear();
    for (auto it = codes.constBegin(); it != codes.constEnd(); ++it) {
        QString normalizedName = normalizeCountryName(it.key());
        countryCodes[normalizedName] = it.value();
//        qDebug() << "Set country code for" << normalizedName << ":" << it.value();
    }
    countryCodes["n. cyprus"] = "N_CYP"; // Специальное правило для Северного Кипра
}

QString MapWidget::getCountryCode(const QString &countryName) const {
    QString normalizedName = normalizeCountryName(countryName);
    for (auto it = countryCodes.constBegin(); it != countryCodes.constEnd(); ++it) {
        if (it.key().compare(normalizedName, Qt::CaseInsensitive) == 0) {
            return it.value();
        }
    }
    return "";
}

void MapWidget::setScale(qreal newScale, const QPointF &focusPoint) {
    if (newScale < getMinScale() || newScale > getMaxScale()) {
        qDebug() << "Scale out of bounds:" << newScale << " (min:" << getMinScale() << ", max:" << getMaxScale() << ")";
        return;
    }

    if (!focusPoint.isNull()) {
        QPointF worldPosBefore = (focusPoint - panOffset) / scale;
        scale = newScale;
        panOffset = focusPoint - worldPosBefore * scale;
    } else {
        scale = newScale;
    }

    clampOffset();
    if (showBubble) {
        for (const auto &country : countries) {
            if (country.name == selectedCountry) {
                QPointF bubblePos = QPointF(country.centroid.x() * scale + 50, country.centroid.y() * scale - 50);
                bubblePos += panOffset;
                infoBubble->setPosition(bubblePos);
                break;
            }
        }
    }
    emit scaleChanged(scale);
    update();
}

QString MapWidget::normalizeCountryName(const QString &name) const {
    QString normalized = name.toLower();
//    qDebug() << "Normalizing country name:" << name << "to" << normalized;

    // Унифицируем названия стран для соответствия ключам в countryCodes
    if (normalized == "united states of america" || normalized == "usa" || normalized == "united states") return "united states";
    if (normalized == "central african rep.") return "central african republic";
    if (normalized == "dem. rep. congo" || normalized == "democratic republic of the congo") return "dr congo";
    if (normalized == "congo" || normalized == "republic of the congo") return "congo";
    if (normalized == "united kingdom of great britain and northern ireland" || normalized == "uk") return "united kingdom";
    if (normalized == "south korea" || normalized == "korea, republic of") return "south korea";
    if (normalized == "north korea" || normalized == "korea, democratic people's republic of") return "north korea";
    if (normalized == "russian federation" || normalized == "russia") return "russia";
    if (normalized == "iran (islamic republic of)") return "iran";
    if (normalized == "syrian arab republic") return "syria";
    if (normalized == "venezuela (bolivarian republic of)") return "venezuela";
    if (normalized == "bolivia (plurinational state of)") return "bolivia";
    if (normalized == "tanzania, united republic of") return "tanzania";
    if (normalized == "viet nam") return "vietnam";
    if (normalized == "eq. guinea") return "equatorial guinea";
    if (normalized == "w. sahara") return "western sahara";
    if (normalized == "eswatini") return "eswatini";
    if (normalized == "n. cyprus") return "n. cyprus";
    if (normalized == "lao people's democratic republic") return "laos";
    if (normalized == "palestinian territory") return "palestine";
    if (normalized == "holy see (vatican city state)") return "vatican city";
    if (normalized == "micronesia, federated states of") return "micronesia";
    if (normalized == "brunei darussalam") return "brunei";
    if (normalized == "timor-leste") return "east timor";
    // Добавляем новые правила для стран из отладки
    if (normalized == "papua new guinea") return "papua new guinea";
    if (normalized == "australia") return "australia";
    if (normalized == "fiji") return "fiji";
    if (normalized == "new zealand") return "new zealand";
    if (normalized == "new caledonia") return "new caledonia";
    if (normalized == "solomon is.") return "solomon islands";
    if (normalized == "vanuatu") return "vanuatu";
    if (normalized == "antarctica") return "antarctica";
    if (normalized == "fr. s. antarctic lands") return "french southern territories";

    return normalized;
}

qreal MapWidget::calculateCountryWidth(const Country &country) const {
    return country.landWidth * scale;
}

qreal MapWidget::calculatePolygonArea(const QVector<QPointF> &points) const {
    qreal area = 0.0;
    int n = points.size();
    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n;
        area += points[i].x() * points[j].y();
        area -= points[j].x() * points[i].y();
    }
    return qAbs(area) / 2.0;
}

qreal MapWidget::calculatePolygonWidth(const QVector<QPointF> &points) const {
    qreal minX = 180, maxX = -180;
    for (const QPointF &point : points) {
        minX = qMin(minX, point.x());
        maxX = qMax(maxX, point.x());
    }
    return maxX - minX;
}

bool MapWidget::isPointInsidePolygon(const QPointF &point, const QVector<QPointF> &polygon) const {
    QPolygonF poly;
    for (const QPointF &p : polygon) {
        poly << p;
    }
    return poly.containsPoint(point, Qt::OddEvenFill);
}

bool MapWidget::isTextInsidePolygon(const QString &text, const QPointF &textPos, const QFontMetrics &fm, const QVector<QPointF> &polygon, qreal scale) const {
    if (scale <= 0) {
        qDebug() << "Invalid scale in isTextInsidePolygon:" << scale;
        return false;
    }

    int textWidth = fm.horizontalAdvance(text);
    int textHeight = fm.height();

    qreal left = (textPos.x() - textWidth / 2.0) / scale;
    qreal right = (textPos.x() + textWidth / 2.0) / scale;
    qreal top = (textPos.y() - textHeight / 2.0) / scale;
    qreal bottom = (textPos.y() + textHeight / 2.0) / scale;

    QVector<QPointF> textCorners = {
        QPointF(left, top),
        QPointF(right, top),
        QPointF(left, bottom),
        QPointF(right, bottom)
    };

    for (const QPointF &corner : textCorners) {
        if (!isPointInsidePolygon(corner, polygon)) {
            return false;
        }
    }
    return true;
}

QPointF MapWidget::findPoleOfInaccessibility(const QVector<QPointF> &points) const {
    qreal minX = 180, maxX = -180;
    qreal minY = 90, maxY = -90;
    for (const QPointF &p : points) {
        minX = qMin(minX, p.x());
        maxX = qMax(maxX, p.x());
        minY = qMin(minY, p.y());
        maxY = qMax(maxY, p.y());
    }

    const int gridSize = 10;
    qreal stepX = (maxX - minX) / gridSize;
    qreal stepY = (maxY - minY) / gridSize;

    QPointF bestPoint = points[0];
    qreal maxDistance = -1.0;

    for (int i = 0; i <= gridSize; ++i) {
        for (int j = 0; j <= gridSize; ++j) {
            QPointF testPoint(minX + i * stepX, minY + j * stepY);
            if (!isPointInsidePolygon(testPoint, points)) {
                continue;
            }

            qreal minDistance = std::numeric_limits<qreal>::max();
            for (int k = 0; k < points.size(); ++k) {
                int l = (k + 1) % points.size();
                QPointF p1 = points[k];
                QPointF p2 = points[l];
                qreal dx = p2.x() - p1.x();
                qreal dy = p2.y() - p1.y();
                qreal len = std::sqrt(dx * dx + dy * dy);
                if (len == 0) continue;

                qreal t = ((testPoint.x() - p1.x()) * dx + (testPoint.y() - p1.y()) * dy) / (len * len);
                t = qBound(0.0, t, 1.0);
                QPointF closest(p1.x() + t * dx, p1.y() + t * dy);
                qreal dist = std::sqrt(std::pow(testPoint.x() - closest.x(), 2) + std::pow(testPoint.y() - closest.y(), 2));
                minDistance = qMin(minDistance, dist);
            }

            if (minDistance > maxDistance) {
                maxDistance = minDistance;
                bestPoint = testPoint;
            }
        }
    }

    return bestPoint;
}

QPointF MapWidget::calculateCentroid(const QVector<QPointF> &points) const {
    return findPoleOfInaccessibility(points);
}

void MapWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Qt::blue);

    qreal mapWidth = getMapWidth() * scale;
    qreal xOffset = fmod(panOffset.x(), mapWidth);
    painter.translate(xOffset, panOffset.y());

    for (int i = -1; i <= 1; i++) {
        painter.save();
        painter.translate(i * mapWidth, 0);
        for (const auto &country : countries) {
            if (isCompareMode && selectedCountries.contains(country.name)) continue;
            if (!isCompareMode && country.name == selectedCountry) continue;

            bool isHovered = (country.name == hoveredCountry);
            qreal countryScale = countryScales.value(country.name, 1.0);

            for (const auto &points : country.polygons) {
                painter.save();
                painter.translate(country.centroid.x() * scale, country.centroid.y() * scale);
                painter.scale(countryScale, countryScale);
                painter.translate(-country.centroid.x() * scale, -country.centroid.y() * scale);

                QPolygonF polygon;
                for (const QPointF &point : points) {
                    polygon << QPointF(point.x() * scale, point.y() * scale);
                }
                painter.setBrush(isHovered ? Qt::red : Qt::gray); // Восстанавливаем исходный цвет Qt::gray
                painter.drawPolygon(polygon);
                painter.restore();
            }

            painter.save();
            painter.setPen(Qt::black);
            QFont font("Arial");
            qreal baseFontSize = 8.0 + std::sqrt(country.area) / 50.0;
            font.setPointSizeF(baseFontSize);
            painter.setFont(font);
            QPointF textPos(country.centroid.x() * scale, country.centroid.y() * scale);
            QFontMetrics fm(font);

            QString fullName = country.name;
            QString abbrName = getCountryCode(country.name);

            QString displayText;
            if (isTextInsidePolygon(fullName, textPos, fm, country.largestPolygon, scale)) {
                displayText = fullName;
            } else if (!abbrName.isEmpty() && isTextInsidePolygon(abbrName, textPos, fm, country.largestPolygon, scale)) {
                displayText = abbrName;
            } else {
                displayText = "";
            }

            if (!displayText.isEmpty()) {
                int textWidth = fm.horizontalAdvance(displayText);
                painter.drawText(textPos.x() - textWidth / 2, textPos.y(), displayText);
            }
            painter.restore();
        }

        if (isCompareMode) {
            for (const auto &country : countries) {
                if (!selectedCountries.contains(country.name)) continue;

                for (const auto &points : country.polygons) {
                    painter.save();
                    painter.translate(country.centroid.x() * scale, country.centroid.y() * scale);
                    painter.translate(-country.centroid.x() * scale, -country.centroid.y() * scale);

                    QPolygonF polygon;
                    for (const QPointF &point : points) {
                        polygon << QPointF(point.x() * scale, point.y() * scale);
                    }
                    painter.setPen(Qt::black);
                    painter.setBrush(Qt::red);
                    painter.drawPolygon(polygon.translated(2, 2));
                    painter.setPen(Qt::NoPen);
                    painter.setBrush(Qt::red);
                    painter.drawPolygon(polygon);
                    painter.restore();
                }

                painter.save();
                painter.setPen(Qt::black);
                QFont font("Arial");
                qreal baseFontSize = 8.0 + std::sqrt(country.area) / 50.0;
                font.setPointSizeF(baseFontSize);
                painter.setFont(font);
                QPointF textPos(country.centroid.x() * scale, country.centroid.y() * scale);
                QFontMetrics fm(font);

                QString fullName = country.name;
                QString abbrName = getCountryCode(country.name);

                QString displayText;
                if (isTextInsidePolygon(fullName, textPos, fm, country.largestPolygon, scale)) {
                    displayText = fullName;
                } else if (!abbrName.isEmpty() && isTextInsidePolygon(abbrName, textPos, fm, country.largestPolygon, scale)) {
                    displayText = abbrName;
                } else {
                    displayText = "";
                }

                if (!displayText.isEmpty()) {
                    int textWidth = fm.horizontalAdvance(displayText);
                    painter.translate(textPos.x(), textPos.y());
                    painter.translate(-textPos.x(), -textPos.y());
                    painter.drawText(textPos.x() - textWidth / 2, textPos.y(), displayText);
                }
                painter.restore();
            }
        } else if (!selectedCountry.isEmpty()) {
            for (const auto &country : countries) {
                if (country.name != selectedCountry) continue;

                qreal countryScale = countryScales.value(country.name, 1.0);
                qDebug() << "Selected country:" << country.name << "Country scale:" << countryScale;

                for (const auto &points : country.polygons) {
                    painter.save();
                    painter.translate(country.centroid.x() * scale, country.centroid.y() * scale);
                    painter.scale(countryScale, countryScale);
                    painter.translate(-country.centroid.x() * scale, -country.centroid.y() * scale);

                    QPolygonF polygon;
                    for (const QPointF &point : points) {
                        polygon << QPointF(point.x() * scale, point.y() * scale);
                    }
                    painter.setPen(Qt::black);
                    painter.setBrush(Qt::red);
                    painter.drawPolygon(polygon.translated(2, 2));
                    painter.setPen(Qt::NoPen);
                    painter.setBrush(Qt::red);
                    painter.drawPolygon(polygon);
                    painter.restore();
                }

                painter.save();
                painter.setPen(Qt::black);
                QFont font("Arial");
                qreal baseFontSize = 8.0 + std::sqrt(country.area) / 50.0;
                font.setPointSizeF(baseFontSize);
                painter.setFont(font);
                QPointF textPos(country.centroid.x() * scale, country.centroid.y() * scale);
                QFontMetrics fm(font);

                QString fullName = country.name;
                QString abbrName = getCountryCode(country.name);

                QString displayText;
                if (isTextInsidePolygon(fullName, textPos, fm, country.largestPolygon, scale)) {
                    displayText = fullName;
                } else if (!abbrName.isEmpty() && isTextInsidePolygon(abbrName, textPos, fm, country.largestPolygon, scale)) {
                    displayText = abbrName;
                } else {
                    displayText = "";
                }

                if (!displayText.isEmpty()) {
                    int textWidth = fm.horizontalAdvance(displayText);
                    painter.translate(textPos.x(), textPos.y());
                    painter.scale(countryScale, countryScale);
                    painter.translate(-textPos.x(), -textPos.y());
                    painter.drawText(textPos.x() - textWidth / 2, textPos.y(), displayText);
                }
                painter.restore();
            }
        }
        painter.restore();
    }
}

void MapWidget::clampOffset() {
    qreal mapHeight = (maxLat - minLat) * scale;
    qreal minY, maxY;

    if (mapHeight <= height()) {
        minY = (height() - mapHeight) / 2 - minLat * scale;
        maxY = minY;
    } else {
        minY = -maxLat * scale;
        maxY = height() - minLat * scale;
    }

    if (maxY < minY) {
        qDebug() << "Error: maxY < minY in clampOffset! minY:" << minY << "maxY:" << maxY;
        maxY = minY;
    }

    panOffset.setY(qBound(minY, panOffset.y(), maxY));
}

void MapWidget::wheelEvent(QWheelEvent *event) {
    QPointF mousePos = event->position();
    qreal newScale = scale + (event->angleDelta().y() > 0 ? 0.1 : -0.1);
    newScale = qBound(getMinScale(), newScale, getMaxScale());
    if (newScale != scale) {
        setScale(newScale, mousePos);
    }
}

void MapWidget::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        panOffset += event->pos() - lastMousePos;
        lastMousePos = event->pos();

        clampOffset();

        if (qAbs(scale - getMinScale()) <= 0.001) {
            qreal worldHeight = (maxLat - minLat) * scale;
            panOffset.setY((height() - worldHeight) / 2 - minLat * scale);
            clampOffset();
        }

        if (showBubble) {
            for (const auto &country : countries) {
                if (country.name == selectedCountry) {
                    QPointF bubblePos = QPointF(country.centroid.x() * scale + 50, country.centroid.y() * scale - 50);
                    bubblePos += panOffset;
                    infoBubble->setPosition(bubblePos);
                    break;
                }
            }
        }

        update();
    }

    // Преобразуем экранные координаты в мировые
    QPointF mousePos = event->pos();
    qreal mapWidth = getMapWidth() * scale;
    qreal xOffset = fmod(panOffset.x(), mapWidth);

    // Учитываем смещение карты
    mousePos.setX(mousePos.x() - xOffset);
    mousePos.setY(mousePos.y() - panOffset.y());
    mousePos /= scale;

    QString newHoveredCountry = "";
    for (int i = -1; i <= 1; ++i) {
        QPointF adjustedMousePos = mousePos;
        adjustedMousePos.setX(mousePos.x() - i * getMapWidth());

        for (const auto &country : countries) {
            for (const auto &points : country.polygons) {
                QPolygonF polygon;
                for (const QPointF &point : points) {
                    polygon << point;
                }
                if (polygon.containsPoint(adjustedMousePos, Qt::OddEvenFill)) {
                    newHoveredCountry = country.name;
                    break;
                }
            }
            if (!newHoveredCountry.isEmpty()) break;
        }
        if (!newHoveredCountry.isEmpty()) break;
    }

    if (newHoveredCountry != hoveredCountry) {
        hoveredCountry = newHoveredCountry;
        update();
    }
}

void MapWidget::mousePressEvent(QMouseEvent *event) {
    lastMousePos = event->pos();
}

void MapWidget::clearSelection() {
    selectedCountry = "";
    selectedCountries.clear();
    countryScales.clear();
    hideInfoBubble();
    update();
}

void MapWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (showBubble) {
        QRect bubbleRect = infoBubble->geometry();
        if (bubbleRect.contains(event->pos())) {
            return;
        }
    }

    QPointF clickPos = event->pos();
    qreal mapWidth = getMapWidth() * scale;
    qreal xOffset = fmod(panOffset.x(), mapWidth);

    // Учитываем смещение карты
    clickPos.setX(clickPos.x() - xOffset);
    clickPos.setY(clickPos.y() - panOffset.y());
    clickPos /= scale;

    QString newSelectedCountry = "";
    for (int i = -1; i <= 1; ++i) {
        QPointF adjustedClickPos = clickPos;
        adjustedClickPos.setX(clickPos.x() - i * getMapWidth());

        for (const auto &country : countries) {
            bool clicked = false;
            for (const auto &points : country.polygons) {
                QPolygonF polygon;
                for (const QPointF &point : points) {
                    polygon << point;
                }
                if (polygon.containsPoint(adjustedClickPos, Qt::OddEvenFill)) {
                    newSelectedCountry = country.name;
                    clicked = true;
                    break;
                }
            }
            if (clicked) break;
        }
        if (!newSelectedCountry.isEmpty()) break;
    }

    if (isCompareMode) {
        if (!newSelectedCountry.isEmpty()) {
            if (selectedCountries.contains(newSelectedCountry)) {
                selectedCountries.removeAll(newSelectedCountry);
                countryScales.remove(newSelectedCountry);
            } else {
                selectedCountries.append(newSelectedCountry);
                countryScales[newSelectedCountry] = 1.0;
            }
            QString countryCode = getCountryCode(newSelectedCountry);
            qint64 population = demographicsManager->getPopulation(countryCode);
            emit countryClicked(newSelectedCountry, population);
        }
    } else {
        if (newSelectedCountry == selectedCountry) {
            countryScales.remove(selectedCountry);
            selectedCountry = "";
            hoveredCountry = "";
            hideInfoBubble();
        } else {
            if (!selectedCountry.isEmpty()) {
                countryScales.remove(selectedCountry);
                selectedCountry = "";
                hoveredCountry = "";
                hideInfoBubble();
            }
            if (!newSelectedCountry.isEmpty()) {
                currentCountry = newSelectedCountry;
                selectedCountry = currentCountry;
                countryScales[currentCountry] = 1.33;
                qDebug() << "Clicked country:" << currentCountry;
                QString countryCode = getCountryCode(newSelectedCountry);
                qint64 population = demographicsManager->getPopulation(countryCode);
                emit countryClicked(currentCountry, population);
            }
        }
    }

    update();
}

void MapWidget::showInfoBubble(const QString &countryName, const QPointF &centroid, qint64 population) {
    infoBubble->setCountryInfo(countryName, population);
    QPointF bubblePos = QPointF(centroid.x() * scale + 50, centroid.y() * scale - 50);
    bubblePos += panOffset;
    infoBubble->setPosition(bubblePos);
    infoBubble->show();
    showBubble = true;
}

void MapWidget::hideInfoBubble() {
    infoBubble->hide();
    showBubble = false;
}

void MapWidget::onMoreStatsClicked() {
    emit showStatsRequested();
}

void MapWidget::loadGeoJson(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        throw CustomException(CustomException::ErrorType::FileNotFound,
                              QString("Failed to open GeoJSON file: %1").arg(filePath));
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        throw CustomException(CustomException::ErrorType::InvalidJsonFormat,
                              QString("Failed to parse GeoJSON file: %1").arg(filePath));
    }

    QJsonArray features = doc["features"].toArray();
    if (features.isEmpty()) {
        throw CustomException(CustomException::ErrorType::InvalidJsonFormat,
                              QString("GeoJSON file %1 contains no features").arg(filePath));
    }

    for (const QJsonValue &feature : features) {
        QJsonObject obj = feature.toObject();
        QString name = obj["properties"].toObject()["name"].toString();
        QJsonArray coordsArray = obj["geometry"].toObject()["coordinates"].toArray();
        Country country;
        country.name = name;
        country.area = 0.0;

        qreal maxArea = 0.0;
        QPointF largestCentroid;
        qreal largestWidth = 0.0;
        QVector<QPointF> largestPolygon;

        if (obj["geometry"].toObject()["type"].toString() == "MultiPolygon") {
            for (const QJsonValue &polygon : coordsArray) {
                QJsonArray coords = polygon.toArray();
                if (coords.isEmpty()) continue;
                QJsonArray firstRing = coords[0].toArray();
                if (firstRing.isEmpty()) continue;

                QVector<QPointF> points;
                for (int i = 0; i < firstRing.size(); ++i) {
                    QJsonArray coord = firstRing[i].toArray();
                    if (coord.size() < 2) continue;

                    qreal lon = coord[0].toDouble();
                    qreal lat = -coord[1].toDouble();

                    // Проверка корректности координат
                    if (lon < -180 || lon > 180 || lat < -90 || lat > 90) {
                        throw CustomException(CustomException::ErrorType::InvalidCoordinates,
                                              QString("Invalid coordinates for %1: lon=%2, lat=%3").arg(name).arg(lon).arg(lat));
                    }

                    while (lon > 180) lon -= 360;
                    while (lon < -180) lon += 360;

                    points << QPointF(lon, lat);

                    minLat = qMin(minLat, lat);
                    maxLat = qMax(maxLat, lat);

                    if (i > 0) {
                        qreal prevLon = points[i-1].x();
                        qreal deltaLon = qAbs(lon - prevLon);
                        if (deltaLon > 180) {
                            qreal sign = (lon > prevLon) ? -1 : 1;
                            qreal interLon = (lon > prevLon) ? 180 : -180;
                            qreal interLat = points[i-1].y() + (interLon - prevLon) * (lat - points[i-1].y()) / (lon - prevLon);
                            points << QPointF(interLon, interLat);
                            country.polygons << points;
                            points.clear();
                            points << QPointF(interLon + sign * 0.001, interLat);
                            points << QPointF(lon, lat);
                        }
                    }
                }
                if (!points.isEmpty()) {
                    country.polygons << points;
                    qreal area = calculatePolygonArea(points);
                    country.area += area;
                    if (area > maxArea) {
                        maxArea = area;
                        largestCentroid = calculateCentroid(points);
                        largestWidth = calculatePolygonWidth(points);
                        largestPolygon = points;
                    }
                }
            }
        } else {
            QJsonArray coords = coordsArray[0].toArray();
            if (coords.isEmpty()) continue;

            QVector<QPointF> points;
            for (int i = 0; i < coords.size(); ++i) {
                QJsonArray coord = coords[i].toArray();
                if (coord.size() < 2) continue;

                qreal lon = coord[0].toDouble();
                qreal lat = -coord[1].toDouble();

                // Проверка корректности координат
                if (lon < -180 || lon > 180 || lat < -90 || lat > 90) {
                    throw CustomException(CustomException::ErrorType::InvalidCoordinates,
                                          QString("Invalid coordinates for %1: lon=%2, lat=%3").arg(name).arg(lon).arg(lat));
                }

                while (lon > 180) lon -= 360;
                while (lon < -180) lon += 360;

                points << QPointF(lon, lat);

                minLat = qMin(minLat, lat);
                maxLat = qMax(maxLat, lat);

                if (i > 0) {
                    qreal prevLon = points[i-1].x();
                    qreal deltaLon = qAbs(lon - prevLon);
                    if (deltaLon > 180) {
                        qreal sign = (lon > prevLon) ? -1 : 1;
                        qreal interLon = (lon > prevLon) ? 180 : -180;
                        qreal interLat = points[i-1].y() + (interLon - prevLon) * (lat - points[i-1].y()) / (lon - prevLon);
                        points << QPointF(interLon, interLat);
                        country.polygons << points;
                        points.clear();
                        points << QPointF(interLon + sign * 0.001, interLat);
                        points << QPointF(lon, lat);
                    }
                }
            }
            if (!points.isEmpty()) {
                country.polygons << points;
                qreal area = calculatePolygonArea(points);
                country.area = area;
                largestCentroid = calculateCentroid(points);
                largestWidth = calculatePolygonWidth(points);
                largestPolygon = points;
            }
        }

        country.centroid = largestCentroid;
        country.landWidth = largestWidth;
        country.largestPolygon = largestPolygon;
        countries << country;
    }
}

void MapWidget::fitToWindow() {
    if (countries.isEmpty()) {
        return;
    }

    qreal minX = 180, maxX = -180;
    for (const auto &country : countries) {
        for (const auto &points : country.polygons) {
            for (const QPointF &point : points) {
                minX = qMin(minX, point.x());
                maxX = qMax(maxX, point.x());
            }
        }
    }

    qreal worldWidth = maxX - minX;
    qreal worldHeight = maxLat - minLat;

    if (worldWidth <= 0 || worldHeight <= 0) {
        return;
    }

    qreal windowWidth = width();
    qreal windowHeight = height();

    if (windowWidth <= 0 || windowHeight <= 0) {
        return;
    }

    qreal fitScale = qMin(windowWidth / worldWidth, windowHeight / worldHeight);
    scale = qBound(5.5, fitScale, getMaxScale()); // Минимальный масштаб 5.5
    qDebug() << "Initial scale set to:" << scale << "(fitScale:" << fitScale << ", minScale:" << getMinScale() << ", maxScale:" << getMaxScale() << ")";

    panOffset.setX((windowWidth - worldWidth * scale) / 2 - minX * scale);
    panOffset.setY((windowHeight - worldHeight * scale) / 2 - minLat * scale);

    clampOffset();
    update();
}

QPointF MapWidget::getCountryCentroid(const QString &countryName) const {
    for (const auto &country : countries) {
        if (country.name == countryName) {
            return country.centroid;
        }
    }
    return QPointF();
}

void MapWidget::highlightCountry(const QString &countryName, bool zoom) {
    QString countryCode = getCountryCode(countryName);
    if (!countryCode.isEmpty()) {
        selectedCountries.clear(); // Очищаем текущие выделения
        selectedCountries.append(countryName); // Добавляем новую страну в список
        selectedCountry = countryName; // Устанавливаем как текущую выбранную страну
        countryScales[countryName] = 1.33; // Устанавливаем увеличение, как при клике

        if (zoom) {
            QPointF centroid = getCountryCentroid(countryName);
            if (!centroid.isNull()) {
                // Увеличиваем масштаб
                qreal currentScale = getScale();
                qreal newScale = currentScale * 1.33; // Увеличиваем масштаб на 33%, как при клике
                newScale = qBound(getMinScale(), newScale, getMaxScale());

                // Центрируем камеру на страну
                QPointF screenCenter(width() / 2.0, height() / 2.0);
                panOffset = screenCenter - centroid * newScale;
                scale = newScale;

                clampOffset(); // Убеждаемся, что смещение в пределах допустимого
            }
        }

        update(); // Перерисовываем карту
    }
}

void MapWidget::setCompareMode(bool enabled) {
    isCompareMode = enabled;
    update();
}
