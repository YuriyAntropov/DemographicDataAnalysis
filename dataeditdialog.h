#ifndef DATAEDITDIALOG_H
#define DATAEDITDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include "DemographicsManager.h"

class DataEditDialog : public QDialog {
    Q_OBJECT
public:
    explicit DataEditDialog(const QString &countryCode, DemographicsManager *manager, QWidget *parent = nullptr);

    DemographicsManager::CountryData getUpdatedData() const; // Геттер для получения изменённых данных

private slots:
    void onSaveButtonClicked();

private:
    QString countryCode;
    DemographicsManager *demographicsManager;
    DemographicsManager::CountryData originalData;

    QLineEdit *populationInput;
    QLineEdit *capitalInput;
    QLineEdit *regionInput;
    QLineEdit *subregionInput;
    QLineEdit *areaInput;
    QLineEdit *currenciesInput;
    QLineEdit *languagesInput;
    QLineEdit *timezonesInput;
    QLineEdit *phoneCodeInput;
    QLineEdit *tldInput;
    QLineEdit *countryLatInput;
    QLineEdit *countryLonInput;
    QLineEdit *capitalLatInput;
    QLineEdit *capitalLonInput;
    QLineEdit *flagUrlInput;
    QLineEdit *coatOfArmsUrlInput;
    QLineEdit *drivingSideInput;

    QPushButton *saveButton;
};

#endif // DATAEDITDIALOG_H
