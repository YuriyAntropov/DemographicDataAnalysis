#include "DataEditDialog.h"
#include <QDebug>

DataEditDialog::DataEditDialog(const QString &countryCode, DemographicsManager *manager, QWidget *parent)
    : QDialog(parent), countryCode(countryCode), demographicsManager(manager) {
    setWindowTitle("Edit Data for " + countryCode);
    setModal(true);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    originalData = demographicsManager->getDemographics()[countryCode];

    populationInput = new QLineEdit(QString::number(originalData.population), this);
    capitalInput = new QLineEdit(originalData.capital, this);
    regionInput = new QLineEdit(originalData.region, this);
    subregionInput = new QLineEdit(originalData.subregion, this);
    areaInput = new QLineEdit(QString::number(originalData.area), this);
    currenciesInput = new QLineEdit(originalData.currencies.join(", "), this);
    languagesInput = new QLineEdit(originalData.languages.join(", "), this);
    timezonesInput = new QLineEdit(originalData.timezones.join(", "), this);
    phoneCodeInput = new QLineEdit(originalData.phoneCode, this);
    tldInput = new QLineEdit(originalData.tld.join(", "), this);
    countryLatInput = new QLineEdit(QString::number(originalData.countryCoordinates.x()), this);
    countryLonInput = new QLineEdit(QString::number(originalData.countryCoordinates.y()), this);
    capitalLatInput = new QLineEdit(QString::number(originalData.capitalCoordinates.x()), this);
    capitalLonInput = new QLineEdit(QString::number(originalData.capitalCoordinates.y()), this);
    flagUrlInput = new QLineEdit(originalData.flagUrl, this);
    coatOfArmsUrlInput = new QLineEdit(originalData.coatOfArmsUrl, this);
    drivingSideInput = new QLineEdit(originalData.drivingSide, this);

    formLayout->addRow("Population:", populationInput);
    formLayout->addRow("Capital:", capitalInput);
    formLayout->addRow("Region:", regionInput);
    formLayout->addRow("Subregion:", subregionInput);
    formLayout->addRow("Area (km²):", areaInput);
    formLayout->addRow("Currencies:", currenciesInput);
    formLayout->addRow("Languages:", languagesInput);
    formLayout->addRow("Timezones:", timezonesInput);
    formLayout->addRow("Phone Code:", phoneCodeInput);
    formLayout->addRow("TLD:", tldInput);
    formLayout->addRow("Country Lat:", countryLatInput);
    formLayout->addRow("Country Lon:", countryLonInput);
    formLayout->addRow("Capital Lat:", capitalLatInput);
    formLayout->addRow("Capital Lon:", capitalLonInput);
    formLayout->addRow("Flag URL:", flagUrlInput);
    formLayout->addRow("Coat of Arms URL:", coatOfArmsUrlInput);
    formLayout->addRow("Driving Side:", drivingSideInput);

    mainLayout->addLayout(formLayout);

    saveButton = new QPushButton("Save", this);
    mainLayout->addWidget(saveButton);

    connect(saveButton, &QPushButton::clicked, this, &DataEditDialog::onSaveButtonClicked);

    setLayout(mainLayout);
}

DemographicsManager::CountryData DataEditDialog::getUpdatedData() const {
    DemographicsManager::CountryData updatedData = originalData;

    updatedData.population = populationInput->text().toLongLong();
    updatedData.capital = capitalInput->text();
    updatedData.region = regionInput->text();
    updatedData.subregion = subregionInput->text();
    updatedData.area = areaInput->text().toDouble();
    updatedData.currencies = currenciesInput->text().split(", ", Qt::SkipEmptyParts);
    updatedData.languages = languagesInput->text().split(", ", Qt::SkipEmptyParts);
    updatedData.timezones = timezonesInput->text().split(", ", Qt::SkipEmptyParts);
    updatedData.phoneCode = phoneCodeInput->text();
    updatedData.tld = tldInput->text().split(", ", Qt::SkipEmptyParts);
    updatedData.countryCoordinates = QPointF(countryLatInput->text().toDouble(), countryLonInput->text().toDouble());
    updatedData.capitalCoordinates = QPointF(capitalLatInput->text().toDouble(), capitalLonInput->text().toDouble());
    updatedData.flagUrl = flagUrlInput->text();
    updatedData.coatOfArmsUrl = coatOfArmsUrlInput->text();
    updatedData.drivingSide = drivingSideInput->text();

    return updatedData;
}

void DataEditDialog::onSaveButtonClicked() {
    accept();
}
