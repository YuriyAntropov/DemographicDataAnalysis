#ifndef CUSTOMEXCEPTION_H
#define CUSTOMEXCEPTION_H

#include <stdexcept>
#include <QString>

using namespace std;

class CustomException : public std::exception {
public:
    // Перечисление типов ошибок
    enum class ErrorType {
        FileNotFound,       // Ошибка: файл не найден
        InvalidJsonFormat,  // Ошибка: некорректный формат JSON
        InvalidCoordinates, // Ошибка: некорректные координаты
        NullPointer,        // Ошибка: нулевой указатель
        UnknownError        // Неизвестная ошибка
    };

    // Конструктор с типом ошибки и сообщением
    CustomException(ErrorType type, const QString& message)
        : errorType(type), errorMessage(message.toStdString()) {}

    // Переопределение метода what() из std::exception
    const char* what() const noexcept override {
        return errorMessage.c_str();
    }

    // Получение типа ошибки
    ErrorType getErrorType() const { return errorType; }

    // Получение сообщения об ошибке в виде QString
    QString getErrorMessage() const { return QString::fromStdString(errorMessage); }

private:
    ErrorType errorType;
    string errorMessage;
};

#endif // CUSTOMEXCEPTION_H
