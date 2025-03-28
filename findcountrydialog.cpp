#include "FindCountryDialog.h"

FindCountryDialog::FindCountryDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Find Country");
    setModal(true); // Модальное окно

    layout = new QVBoxLayout(this);

    countryInput = new QLineEdit(this);
    countryInput->setPlaceholderText("Enter country name (e.g., Russia)");
    layout->addWidget(countryInput);

    findButton = new QPushButton("Find", this);
    layout->addWidget(findButton);

    setLayout(layout);

    // Настраиваю минимальный размер окна
    int width = 300; // Оставляю ширину как есть
    int margin = layout->contentsMargins().top() + layout->contentsMargins().bottom(); // Учитываю отступы layout
    int spacing = layout->spacing(); // Расстояние между виджетами
    int inputHeight = countryInput->sizeHint().height(); // Рекомендуемая высота строки ввода
    int buttonHeight = findButton->sizeHint().height(); // Рекомендуемая высота кнопки

    // Вычисляем минимальную высоту окна
    int totalHeight = inputHeight + spacing + buttonHeight + margin + 10; // Добавляю небольшой запас (10 пикселей)
    setFixedSize(width, totalHeight);

    connect(findButton, &QPushButton::clicked, this, &FindCountryDialog::onFindButtonClicked);
}

QString FindCountryDialog::getCountryName() const {
    return countryInput->text().trimmed(); // Возвращаю введённое название без лишних пробелов
}

void FindCountryDialog::onFindButtonClicked() {
    accept(); // Закрываю диалог с кодом принятия
}
