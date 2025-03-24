#ifndef INFOBUBBLE_H
#define INFOBUBBLE_H

#include <QWidget>
#include <QPainterPath> // Добавляем подключение QPainterPath

class InfoBubble : public QWidget {
    Q_OBJECT
public:
    explicit InfoBubble(QWidget *parent = nullptr);
    void setCountryInfo(const QString &name, qint64 pop);
    void setPosition(const QPointF &pos);

signals:
    void moreStatsClicked();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QString countryName;
    qint64 population;

};

#endif // INFOBUBBLE_H
