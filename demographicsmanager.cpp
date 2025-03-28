#include "DemographicsManager.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QFile>

DemographicsManager::DemographicsManager(QObject *parent) : QObject(parent) {
    addStaticData();
}

void DemographicsManager::updateCountryData(const QString &countryCode, const CountryData &data) {
    if (demographics.contains(countryCode)) {
        demographics[countryCode] = data;
        qDebug() << "Updated data for" << countryCode;
    } else {
        qDebug() << "Country" << countryCode << "not found for update";
    }
}

void DemographicsManager::saveToJson(const QString &filePath) const {
    QJsonArray countriesArray;
    for (auto it = demographics.constBegin(); it != demographics.constEnd(); ++it) {
        const QString &countryCode = it.key();
        const CountryData &data = it.value();

        QJsonObject countryObj;
        countryObj["cca3"] = countryCode;
        countryObj["population"] = QJsonValue::fromVariant(data.population);

        QJsonArray capitalArray;
        if (!data.capital.isEmpty() && data.capital != "N/A") {
            capitalArray.append(data.capital);
        }
        countryObj["capital"] = capitalArray;

        countryObj["region"] = data.region;
        countryObj["subregion"] = data.subregion;
        countryObj["area"] = data.area;

        QJsonObject currenciesObj;
        for (const QString &currency : data.currencies) {
            currenciesObj[currency] = QJsonObject{{"name", currency}};
        }
        countryObj["currencies"] = currenciesObj;

        QJsonObject languagesObj;
        for (const QString &lang : data.languages) {
            languagesObj[lang] = lang;
        }
        countryObj["languages"] = languagesObj;

        QJsonArray timezonesArray;
        for (const QString &tz : data.timezones) {
            timezonesArray.append(tz);
        }
        countryObj["timezones"] = timezonesArray;

        QJsonObject iddObj;
        iddObj["root"] = data.phoneCode.left(2); // Примерное разделение
        iddObj["suffixes"] = QJsonArray{data.phoneCode.mid(2)};
        countryObj["idd"] = iddObj;

        QJsonArray tldArray;
        for (const QString &t : data.tld) {
            tldArray.append(t);
        }
        countryObj["tld"] = tldArray;

        QJsonArray latlngArray = {data.countryCoordinates.x(), data.countryCoordinates.y()};
        countryObj["latlng"] = latlngArray;

        QJsonObject capitalInfo;
        capitalInfo["latlng"] = QJsonArray{data.capitalCoordinates.x(), data.capitalCoordinates.y()};
        countryObj["capitalInfo"] = capitalInfo;

        QJsonObject flags;
        flags["png"] = data.flagUrl;
        countryObj["flags"] = flags;

        QJsonObject coatOfArms;
        coatOfArms["png"] = data.coatOfArmsUrl;
        countryObj["coatOfArms"] = coatOfArms;

        QJsonObject car;
        car["side"] = data.drivingSide.toLower();
        countryObj["car"] = car;

        countriesArray.append(countryObj);
    }

    QJsonDocument doc(countriesArray);
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
        qDebug() << "Data saved to" << filePath;
    } else {
        qDebug() << "Failed to save data to" << filePath;
    }
}

void DemographicsManager::loadFromJson(const QJsonDocument &doc) {
    demographics.clear();
    QJsonArray countries = doc.array();

    qDebug() << "Loading demographics for" << countries.size() << "countries";

    for (const QJsonValue &value : countries) {
        QJsonObject country = value.toObject();
        QString countryCode = country["cca3"].toString();
        qint64 population = country["population"].toVariant().toLongLong();

        QString capital = country["capital"].toArray().isEmpty() ? "N/A" : country["capital"].toArray().first().toString();
        QString region = country["region"].toString("N/A");
        QString subregion = country["subregion"].toString("N/A");
        qreal area = country["area"].toDouble(-1.0);

        // Обработка валют
        QStringList currencies;
        QJsonObject currenciesObj = country["currencies"].toObject();
        for (const QString &cyCode : currenciesObj.keys()) {
            QJsonObject currency = currenciesObj[cyCode].toObject();
            QString name = currency["name"].toString();
            QString symbol = currency["symbol"].toString();
            if (!name.isEmpty()) {
                currencies << (symbol.isEmpty() ? name : QString("%1 (%2)").arg(name, symbol));
            }
        }
        if (currencies.isEmpty()) currencies << "N/A";

        QStringList languages;
        QJsonObject languagesObj = country["languages"].toObject();
        for (const QString &lang : languagesObj.keys()) {
            languages << languagesObj[lang].toString();
        }
        if (languages.isEmpty()) languages << "N/A";

        QStringList timezones;
        QJsonArray timezonesArray = country["timezones"].toArray();
        for (const QJsonValue &tz : timezonesArray) {
            timezones << tz.toString();
        }
        if (timezones.isEmpty()) timezones << "N/A";

        QString phoneCode = "N/A";
        QJsonObject idd = country["idd"].toObject();
        QString root = idd["root"].toString();
        QJsonArray suffixes = idd["suffixes"].toArray();
        if (!root.isEmpty() && !suffixes.isEmpty()) {
            phoneCode = root + suffixes.first().toString();
        }

        QStringList tld;
        QJsonArray tldArray = country["tld"].toArray();
        for (const QJsonValue &domain : tldArray) {
            tld << domain.toString();
        }
        if (tld.isEmpty()) tld << "N/A";

        QPointF countryCoordinates(0, 0);
        QJsonArray latlng = country["latlng"].toArray();
        if (latlng.size() >= 2) {
            countryCoordinates.setX(latlng[0].toDouble());
            countryCoordinates.setY(latlng[1].toDouble());
        }

        QPointF capitalCoordinates(0, 0);
        QJsonObject capitalInfo = country["capitalInfo"].toObject();
        QJsonArray capitalLatlng = capitalInfo["latlng"].toArray();
        if (capitalLatlng.size() >= 2) {
            capitalCoordinates.setX(capitalLatlng[0].toDouble());
            capitalCoordinates.setY(capitalLatlng[1].toDouble());
        }

        QString flagUrl = "N/A";
        QJsonObject flags = country["flags"].toObject();
        if (flags.contains("png")) {
            flagUrl = flags["png"].toString();
        }

        QString coatOfArmsUrl = "N/A";
        QJsonObject coatOfArms = country["coatOfArms"].toObject();
        if (coatOfArms.contains("png")) {
            coatOfArmsUrl = coatOfArms["png"].toString();
        }

        QString drivingSide = "N/A";
        QJsonObject car = country["car"].toObject();
        if (car.contains("side")) {
            drivingSide = car["side"].toString();
            if (!drivingSide.isEmpty()) {
                drivingSide[0] = drivingSide[0].toUpper();
            }
        }

        QJsonObject ageDistribution = {{"0-14", 15}, {"15-64", 70}, {"65+", 15}};

        CountryData &data = demographics[countryCode]; // Используем ссылку для обновления
        data.population = country["population"].toVariant().toLongLong();
        data.ageDistribution = ageDistribution;
        data.capital = country["capital"].toArray().isEmpty() ? "N/A" : country["capital"].toArray().first().toString();
        data.region = country["region"].toString("N/A");
        data.subregion = country["subregion"].toString("N/A");
        data.area = country["area"].toDouble(-1.0);
        data.currencies = currencies;
        data.languages = languages;
        data.timezones = timezones;
        data.phoneCode = phoneCode;
        data.tld = tld;
        data.countryCoordinates = countryCoordinates;
        data.capitalCoordinates = capitalCoordinates;
        data.flagUrl = flagUrl;
        data.coatOfArmsUrl = coatOfArmsUrl;
        data.drivingSide = drivingSide;

        demographics[countryCode] = data;
        qDebug() << "Loaded country:" << countryCode << "Population:" << population << "Area:" << area;
    }
}

void DemographicsManager::addStaticData() {
    QJsonObject ageDistribution = {{"0-14", 15}, {"15-64", 70}, {"65+", 15}};

    CountryData data;
    data.population = 382836;
    data.ageDistribution = ageDistribution;
    data.capital = "N/A";
    data.region = "Europe";
    data.subregion = "Southern Europe";
    data.area = -1.0;
    data.currencies = {"N/A"};
    data.languages = {"N/A"};
    data.timezones = {"N/A"};
    data.phoneCode = "N/A";
    data.tld = {"N/A"};
    data.countryCoordinates = QPointF(0, 0);
    data.capitalCoordinates = QPointF(0, 0);
    data.flagUrl = "N/A";
    data.coatOfArmsUrl = "N/A";
    data.drivingSide = "N/A";
    demographics["N_CYP"] = data;

    data.population = 331002651;
    data.ageDistribution = ageDistribution;
    data.capital = "Washington, D.C.";
    data.region = "Americas";
    data.subregion = "Northern America";
    data.area = 9833517;
    data.currencies = {"US Dollar ($)"};
    data.languages = {"English"};
    data.timezones = {"UTC-05:00", "UTC-06:00"};
    data.phoneCode = "+1";
    data.tld = {".us"};
    data.countryCoordinates = QPointF(38, -97);
    data.capitalCoordinates = QPointF(38.9, -77);
    data.flagUrl = "https://flagcdn.com/w320/us.png";
    data.coatOfArmsUrl = "https://mainfacts.com/media/images/coats_of_arms/us.png";
    data.drivingSide = "Right";
    demographics["USA"] = data;

    data.population = 1439323776;
    data.ageDistribution = ageDistribution;
    data.capital = "Beijing";
    data.region = "Asia";
    data.subregion = "Eastern Asia";
    data.area = 9596961;
    data.currencies = {"Chinese Yuan (¥)"};
    data.languages = {"Chinese"};
    data.timezones = {"UTC+08:00"};
    data.phoneCode = "+86";
    data.tld = {".cn"};
    data.countryCoordinates = QPointF(35, 105);
    data.capitalCoordinates = QPointF(39.9, 116.4);
    data.flagUrl = "https://flagcdn.com/w320/cn.png";
    data.coatOfArmsUrl = "https://mainfacts.com/media/images/coats_of_arms/cn.png";
    data.drivingSide = "Right";
    demographics["CHN"] = data;

    data.population = 1380004385;
    data.ageDistribution = ageDistribution;
    data.capital = "New Delhi";
    data.region = "Asia";
    data.subregion = "Southern Asia";
    data.area = 3287263;
    data.currencies = {"Indian Rupee (₹)"};
    data.languages = {"Hindi", "English"};
    data.timezones = {"UTC+05:30"};
    data.phoneCode = "+91";
    data.tld = {".in"};
    data.countryCoordinates = QPointF(20, 77);
    data.capitalCoordinates = QPointF(28.6, 77.2);
    data.flagUrl = "https://flagcdn.com/w320/in.png";
    data.coatOfArmsUrl = "https://mainfacts.com/media/images/coats_of_arms/in.png";
    data.drivingSide = "Left";
    demographics["IND"] = data;

    data.population = 145912025;
    data.ageDistribution = ageDistribution;
    data.capital = "Moscow";
    data.region = "Europe";
    data.subregion = "Eastern Europe";
    data.area = 17098242;
    data.currencies = {"Russian Ruble (₽)"};
    data.languages = {"Russian"};
    data.timezones = {"UTC+03:00", "UTC+04:00"};
    data.phoneCode = "+7";
    data.tld = {".ru"};
    data.countryCoordinates = QPointF(60, 100);
    data.capitalCoordinates = QPointF(55.8, 37.6);
    data.flagUrl = "https://flagcdn.com/w320/ru.png";
    data.coatOfArmsUrl = "https://mainfacts.com/media/images/coats_of_arms/ru.png";
    data.drivingSide = "Right";
    demographics["RUS"] = data;
}

qint64 DemographicsManager::getPopulation(const QString &country) const {
    return demographics.contains(country) ? demographics[country].population : 0;
}

QJsonObject DemographicsManager::getAgeDistribution(const QString &country) const {
    return demographics.contains(country) ? demographics[country].ageDistribution : QJsonObject();
}

QString DemographicsManager::getCapital(const QString &country) const {
    return demographics.contains(country) ? demographics[country].capital : "N/A";
}

QString DemographicsManager::getRegion(const QString &country) const {
    return demographics.contains(country) ? demographics[country].region : "N/A";
}

QString DemographicsManager::getSubregion(const QString &country) const {
    return demographics.contains(country) ? demographics[country].subregion : "N/A";
}

qreal DemographicsManager::getArea(const QString &country) const {
    return demographics.contains(country) ? demographics[country].area : -1.0;
}

QStringList DemographicsManager::getCurrencies(const QString &country) const {
    return demographics.contains(country) ? demographics[country].currencies : QStringList{"N/A"};
}

QStringList DemographicsManager::getLanguages(const QString &country) const {
    return demographics.contains(country) ? demographics[country].languages : QStringList{"N/A"};
}

QStringList DemographicsManager::getTimezones(const QString &country) const {
    return demographics.contains(country) ? demographics[country].timezones : QStringList{"N/A"};
}

QString DemographicsManager::getPhoneCode(const QString &country) const {
    return demographics.contains(country) ? demographics[country].phoneCode : "N/A";
}

QStringList DemographicsManager::getTLD(const QString &country) const {
    return demographics.contains(country) ? demographics[country].tld : QStringList{"N/A"};
}

QPointF DemographicsManager::getCountryCoordinates(const QString &country) const {
    return demographics.contains(country) ? demographics[country].countryCoordinates : QPointF(0, 0);
}

QPointF DemographicsManager::getCapitalCoordinates(const QString &country) const {
    return demographics.contains(country) ? demographics[country].capitalCoordinates : QPointF(0, 0);
}

QString DemographicsManager::getFlagUrl(const QString &country) const {
    return demographics.contains(country) ? demographics[country].flagUrl : "N/A";
}

QString DemographicsManager::getCoatOfArmsUrl(const QString &country) const {
    return demographics.contains(country) ? demographics[country].coatOfArmsUrl : "N/A";
}

QString DemographicsManager::getDrivingSide(const QString &country) const {
    return demographics.contains(country) ? demographics[country].drivingSide : "N/A";
}
