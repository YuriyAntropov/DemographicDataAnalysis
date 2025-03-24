#include "DocumentationDialog.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

DocumentationDialog::DocumentationDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Documentation");
    setModal(true); // Делаем окно модальным
    setMinimumSize(600, 400); // Задаём минимальный размер окна

    layout = new QVBoxLayout(this);

    textEdit = new QTextEdit(this);
    textEdit->setReadOnly(true); // Только для чтения
    layout->addWidget(textEdit);

    closeButton = new QPushButton("Close", this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept); // Закрытие по кнопке
    layout->addWidget(closeButton);

    setLayout(layout);

    loadDocumentation(); // Загружаем текст документации
}

void DocumentationDialog::loadDocumentation() {
    QFile file(":/docs/documentation.txt"); // Предполагаем, что файл будет в ресурсах
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        textEdit->setPlainText("Error: Could not load documentation file.");
        qDebug() << "Failed to open documentation file:" << file.errorString();
        return;
    }

    QTextStream in(&file);
    QString content = in.readAll();
    textEdit->setPlainText(content);
    file.close();
}
