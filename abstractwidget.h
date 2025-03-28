#ifndef ABSTRACTWIDGET_H
#define ABSTRACTWIDGET_H

#include <QWidget>

class AbstractWidget : public QWidget {
    Q_OBJECT
public:
    explicit AbstractWidget(QWidget *parent = nullptr) : QWidget(parent) {}
    virtual ~AbstractWidget() = default;

    // Виртуальный метод, который должны реализовать наследники
    virtual void updateDisplay() = 0;

signals:

};

#endif // ABSTRACTWIDGET_H
