#ifndef DEMOGRAPHICSMANAGER_H
#define DEMOGRAPHICSMANAGER_H

#include <QObject>
#include <QJsonDocument>
#include <QMap>
#include <QPointF>
#include <QJsonObject>

class DemographicsManager : public QObject {
    Q_OBJECT
public:
    explicit DemographicsManager(QObject *parent = nullptr);
    void loadFromJson(const QJsonDocument &doc);
    qint64 getPopulation(const QString &countryCode) const;
    QString getCapital(const QString &countryCode) const;
    QString getRegion(const QString &countryCode) const;
    QString getSubregion(const QString &countryCode) const;
    qreal getArea(const QString &countryCode) const;
    QStringList getCurrencies(const QString &countryCode) const;
    QStringList getLanguages(const QString &countryCode) const;
    QStringList getTimezones(const QString &countryCode) const;
    QString getPhoneCode(const QString &countryCode) const;
    QStringList getTLD(const QString &countryCode) const;
    QPointF getCountryCoordinates(const QString &countryCode) const;
    QPointF getCapitalCoordinates(const QString &countryCode) const;
    QString getFlagUrl(const QString &countryCode) const;
    QString getCoatOfArmsUrl(const QString &countryCode) const;
    QString getDrivingSide(const QString &countryCode) const;
    QJsonObject getAgeDistribution(const QString &country) const;

    struct CountryData {
        qint64 population = -1;
        QJsonObject ageDistribution;
        QString capital;
        QString region;
        QString subregion;
        qreal area = -1.0;
        QStringList currencies;
        QStringList languages;
        QStringList timezones;
        QString phoneCode;
        QStringList tld;
        QPointF countryCoordinates;
        QPointF capitalCoordinates;
        QString flagUrl;
        QString coatOfArmsUrl;
        QString drivingSide;
    };

    const QMap<QString, CountryData>& getDemographics() const { return demographics; }
    void updateCountryData(const QString &countryCode, const CountryData &data); // Новый метод
    void saveToJson(const QString &filePath) const; // Новый метод для сохранения

private:
    void addStaticData();
    QMap<QString, CountryData> demographics;
};

#endif // DEMOGRAPHICSMANAGER_H
