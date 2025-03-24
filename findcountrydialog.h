#ifndef FINDCOUNTRYDIALOG_H
#define FINDCOUNTRYDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

class FindCountryDialog : public QDialog {
    Q_OBJECT
public:
    explicit FindCountryDialog(QWidget *parent = nullptr);

    QString getCountryName() const; // Геттер для получения введённого названия страны

private slots:
    void onFindButtonClicked();

private:
    QLineEdit *countryInput;
    QPushButton *findButton;
    QVBoxLayout *layout;
};

#endif // FINDCOUNTRYDIALOG_H
