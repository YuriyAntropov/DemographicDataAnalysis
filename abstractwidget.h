#ifndef ABSTRACTWIDGET_H
#define ABSTRACTWIDGET_H

#include <QWidget>

class AbstractWidget : public QWidget {
    Q_OBJECT
public:
    explicit AbstractWidget(QWidget *parent = nullptr) : QWidget(parent) {}
    virtual ~AbstractWidget() = default;

    // Чисто виртуальный метод, который должны реализовать наследники
    virtual void updateDisplay() = 0;

signals:
         // Можно добавить общие сигналы, если нужно, но для минимального вмешательства оставим пустым
};

#endif // ABSTRACTWIDGET_H
