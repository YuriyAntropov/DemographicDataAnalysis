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

    // Убираем layout->addStretch(), чтобы не было пустого пространства

    setLayout(layout);

    // Настраиваем минимальный размер окна
    int width = 300; // Оставляем ширину как есть
    int margin = layout->contentsMargins().top() + layout->contentsMargins().bottom(); // Учитываем отступы layout
    int spacing = layout->spacing(); // Расстояние между виджетами
    int inputHeight = countryInput->sizeHint().height(); // Рекомендуемая высота строки ввода
    int buttonHeight = findButton->sizeHint().height(); // Рекомендуемая высота кнопки

    // Вычисляем минимальную высоту окна
    int totalHeight = inputHeight + spacing + buttonHeight + margin + 10; // Добавляем небольшой запас (10 пикселей)
    setFixedSize(width, totalHeight);

    connect(findButton, &QPushButton::clicked, this, &FindCountryDialog::onFindButtonClicked);
}

QString FindCountryDialog::getCountryName() const {
    return countryInput->text().trimmed(); // Возвращаем введённое название без лишних пробелов
}

void FindCountryDialog::onFindButtonClicked() {
    accept(); // Закрываем диалог с кодом принятия
}
