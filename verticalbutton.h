#ifndef VERTICALBUTTON_H
#define VERTICALBUTTON_H

#include <QPushButton>
#include <QColor>

class VerticalButton : public QPushButton {
    Q_OBJECT
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)

public:
    explicit VerticalButton(QWidget *parent = nullptr);

    QColor backgroundColor() const { return m_backgroundColor; }
    void setBackgroundColor(const QColor &color);

protected:
    void paintEvent(QPaintEvent *event) override;
    bool event(QEvent *event) override;

private:
    QColor m_backgroundColor;
    bool m_isHovered = false;
};

#endif // VERTICALBUTTON_H
