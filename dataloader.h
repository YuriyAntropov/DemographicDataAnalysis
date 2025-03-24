#ifndef DATALOADER_H
#define DATALOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QMap>

class DataLoader : public QObject {
    Q_OBJECT
public:
    explicit DataLoader(QObject *parent = nullptr);
    void loadData();
    void loadCountryCodes();

signals:
    void dataLoaded(const QJsonDocument &data);
    void countryCodesLoaded(const QMap<QString, QString> &countryCodes);
    void debugMessage(const QString &message); // Для отладки

private slots:
    void onReplyFinished(QNetworkReply *reply);
    void onCountryCodesReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *manager;
    bool useRestCountries = true;
};

#endif // DATALOADER_H
