#include "InfoBubble.h"
#include <QPainter>
#include <QMouseEvent>
#include <QFontMetrics>

InfoBubble::InfoBubble(QWidget *parent) : QWidget(parent), population(0) {
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setStyleSheet("InfoBubble { background-color: #424242; border-radius: 10px; }");
}

void InfoBubble::setCountryInfo(const QString &name, qint64 pop) {
    countryName = name;
    population = pop;

    // Вычисляем размер облачка на основе текста
    QFont fontTitle("Roboto", 14, QFont::Bold);
    QFont fontPopulation("Roboto", 12);
    QFont fontMoreStats("Roboto", 10);
    fontMoreStats.setUnderline(true);

    QFontMetrics fmTitle(fontTitle);
    QFontMetrics fmPopulation(fontPopulation);
    QFontMetrics fmMoreStats(fontMoreStats);

    QString populationText = QString("Population: %1").arg(population);
    int titleWidth = fmTitle.horizontalAdvance(countryName);
    int populationWidth = fmPopulation.horizontalAdvance(populationText);
    int moreStatsWidth = fmMoreStats.horizontalAdvance("More stats");

    int maxWidth = qMax(qMax(titleWidth, populationWidth), moreStatsWidth) + 20; // Отступы слева и справа по 10
    int height = fmTitle.height() + fmPopulation.height() + fmMoreStats.height() + 20; // Отступы между строками и сверху/снизу

    setFixedSize(maxWidth, height);
    update();
}

void InfoBubble::setPosition(const QPointF &pos) {
    move(pos.toPoint());
}

void InfoBubble::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.addRoundedRect(0, 0, width() - 1, height() - 1, 10, 10);
    painter.fillPath(path, QColor("#424242"));
    painter.setPen(Qt::NoPen);
    painter.drawPath(path);

    QFont fontTitle("Roboto", 14, QFont::Bold);
    painter.setFont(fontTitle);
    painter.setPen(QColor("#E0E0E0"));
    painter.drawText(10, 10 + fontTitle.pointSize(), countryName); // Смещение на высоту шрифта + отступ сверху

    QFont fontPopulation("Roboto", 12);
    painter.setFont(fontPopulation);
    QString populationText = QString("Population: %1").arg(population);
    painter.drawText(10, 10 + fontTitle.pointSize() + fontPopulation.pointSize() + 5, populationText); // Отступ между строками

    QFont fontMoreStats("Roboto", 10);
    fontMoreStats.setUnderline(true);
    painter.setFont(fontMoreStats);
    painter.setPen(QColor("#42A5F5"));
    painter.drawText(10, height() - 10, "More stats"); // "More stats" внизу с отступом
}

void InfoBubble::mousePressEvent(QMouseEvent *event) {
    QFont fontMoreStats("Roboto", 10);
    QFontMetrics fm(fontMoreStats);
    QRect moreStatsRect(10, height() - 10 - fm.height(), fm.horizontalAdvance("More stats"), fm.height());
    if (moreStatsRect.contains(event->pos())) {
        emit moreStatsClicked();
    }
    QWidget::mousePressEvent(event);
}
