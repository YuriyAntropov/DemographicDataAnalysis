#include "DataLoader.h"
#include <QNetworkReply>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QXmlStreamReader>

DataLoader::DataLoader(QObject *parent) : QObject(parent) {
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &DataLoader::onReplyFinished);
}

void DataLoader::loadData() {
    if (useRestCountries) {
        QUrl url("https://restcountries.com/v3.1/all");
        QNetworkRequest request(url);
        request.setRawHeader("Accept", "application/json");
        manager->get(request);
    } else {
        QUrl url("http://data.un.org/ws/rest/data/DF_UNDATA_WDI/SERIES=SP_POP_TOTL+TIME_PERIOD=2022");
        QNetworkRequest request(url);
        request.setRawHeader("Accept", "text/json");
        manager->get(request);
    }
}

void DataLoader::loadCountryCodes() {
    if (useRestCountries) {
        loadData();
    } else {
        QUrl url("http://data.un.org/ws/rest/codelist/WB/CL_REF_AREA_WDI");
        QNetworkRequest request(url);
        request.setRawHeader("Accept", "application/xml");
        manager->get(request);
    }
}

void DataLoader::onReplyFinished(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Data load error:" << reply->errorString();
        emit debugMessage("Data load error: " + reply->errorString());
        QFile file("data.json");
        if (file.open(QIODevice::ReadOnly)) {
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
            emit dataLoaded(doc);
            file.close();
        }
        reply->deleteLater();
        return;
    }

    QByteArray rawData = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(rawData);
    if (doc.isNull()) {
        qDebug() << "Failed to parse JSON response for data";
        emit debugMessage("Failed to parse JSON response for data");
        reply->deleteLater();
        return;
    }

    QFile file("data.json");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(rawData);
        file.close();
    }

    emit dataLoaded(doc);

    if (useRestCountries) {
        QMap<QString, QString> countryCodes;
        QJsonArray countries = doc.array();
        for (const QJsonValue &value : countries) {
            QJsonObject country = value.toObject();
            QJsonObject nameObj = country["name"].toObject();
            QString officialName = nameObj["official"].toString();
            QString commonName = nameObj["common"].toString();
            QString code = country["cca3"].toString();

            if (!code.isEmpty()) {
                // Добавляем официальное и общее название
                if (!officialName.isEmpty()) {
                    countryCodes[officialName.toLower()] = code;
                }
                if (!commonName.isEmpty() && commonName != officialName) {
                    countryCodes[commonName.toLower()] = code;
                }
                // Добавляем альтернативные названия
                QJsonArray altSpellings = country["altSpellings"].toArray();
                for (const QJsonValue &alt : altSpellings) {
                    QString altName = alt.toString();
                    if (!altName.isEmpty() && !countryCodes.contains(altName.toLower())) {
                        countryCodes[altName.toLower()] = code;
                    }
                }
                // Добавляем переводы на английский
                QJsonObject translations = country["translations"].toObject();
                if (translations.contains("eng")) {
                    QJsonObject engTranslation = translations["eng"].toObject();
                    QString engOfficial = engTranslation["official"].toString();
                    QString engCommon = engTranslation["common"].toString();
                    if (!engOfficial.isEmpty() && engOfficial != officialName && engOfficial != commonName) {
                        countryCodes[engOfficial.toLower()] = code;
                    }
                    if (!engCommon.isEmpty() && engCommon != officialName && engCommon != commonName && engCommon != engOfficial) {
                        countryCodes[engCommon.toLower()] = code;
                    }
                }
            }
        }
        emit countryCodesLoaded(countryCodes);
        emit debugMessage(QString("Loaded %1 country codes").arg(countryCodes.size()));
    }

    reply->deleteLater();
}

void DataLoader::onCountryCodesReplyFinished(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Country codes load error:" << reply->errorString();
        emit debugMessage("Country codes load error: " + reply->errorString());
        reply->deleteLater();
        return;
    }

    QMap<QString, QString> countryCodes;
    QXmlStreamReader xml(reply->readAll());
    QString currentCode, currentName;

    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token == QXmlStreamReader::StartElement) {
            if (xml.name() == "Code") {
                currentCode = xml.attributes().value("id").toString();
            } else if (xml.name() == "Name" && xml.attributes().value("xml:lang").toString() == "en") {
                xml.readNext();
                if (xml.tokenType() == QXmlStreamReader::Characters) {
                    currentName = xml.text().toString().trimmed();
                    if (!currentCode.isEmpty() && !currentName.isEmpty()) {
                        countryCodes[currentName.toLower()] = currentCode;
                    }
                }
            }
        }
    }

    if (xml.hasError()) {
        qDebug() << "XML parse error:" << xml.errorString();
        emit debugMessage("XML parse error: " + xml.errorString());
    } else {
        qDebug() << "Loaded" << countryCodes.size() << "country codes";
        emit debugMessage(QString("Loaded %1 country codes").arg(countryCodes.size()));
    }

    emit countryCodesLoaded(countryCodes);
    reply->deleteLater();
}
