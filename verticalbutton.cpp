#include "VerticalButton.h"
#include <QPainter>
#include <QStyleOptionButton>
#include <QEvent>

VerticalButton::VerticalButton(QWidget *parent) : QPushButton(parent), m_backgroundColor(Qt::black) {
    setBackgroundColor(QColor(Qt::black));
    setAttribute(Qt::WA_Hover, true); // Включаем hover-события
    setMouseTracking(false); // Убедимся, что кнопка не перехватывает события мыши
}

void VerticalButton::setBackgroundColor(const QColor &color) {
    if (m_backgroundColor != color) { // Вызываем update() только при изменении цвета
        m_backgroundColor = color;
        update();
    }
}

void VerticalButton::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), m_backgroundColor);

    painter.save();
    painter.translate(width() / 2, height() / 2);
    painter.rotate(-90);
    painter.translate(-height() / 2, -width() / 2);

    QFontMetrics fm(font());
    int textWidth = fm.horizontalAdvance(text());
    int textHeight = fm.height();

    int x = (height() - textWidth) / 2;
    int y = (width() + textHeight) / 2;

    painter.setPen(Qt::white);
    painter.drawText(x, y, text());

    painter.restore();
}

bool VerticalButton::event(QEvent *event) {
    if (event->type() == QEvent::HoverEnter && !m_isHovered) {
        m_isHovered = true;
        setBackgroundColor(QColor(51, 51, 51)); // Серый цвет (#333333)
    } else if (event->type() == QEvent::HoverLeave && m_isHovered) {
        m_isHovered = false;
        setBackgroundColor(QColor(Qt::black)); // Чёрный цвет
    }
    return QPushButton::event(event);
}
