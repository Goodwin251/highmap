#ifndef MAP_H
#define MAP_H

#include <QWidget>
#include <QImage>
#include <QPixmap>
#include <QLabel>
#include <QMouseEvent>
#include <QColor>
#include <QVector>

class Map : public QWidget
{
    Q_OBJECT
public:
    explicit Map(QWidget *parent = nullptr);

    const int OBSERVER_HEIGHT = 2;
    const int PEAK_HEIGHT = 500;
    const QRgb VISIBLE = QColor::fromRgbF(208,0,255).rgb();
    const QRgb INVISIBLE = QColor::fromRgbF(0,255,0).rgb();

private:
    struct Ray
    {
    public:
        Ray(QPoint begin, Map *parent);
        QPoint begin;
        QPoint end;
        int length;
        QVector<QPoint> points;

        int build(QPoint end);
        int findLength(QPoint end);
        int findLength(QPoint begin, QPoint end);

    private:
        Map *parent;
        QPoint shortestPath(QPoint last, int xsign, int ysign);


    };
    void mousePressEvent(QMouseEvent *event);
    void fillMap(QPoint point);
    bool checkPixel(QPoint point, int increment);
    bool inRange(QPoint point);
    int findHeight(QPoint pixel);
    void scanTerrain(QVector<QPoint> points);
    bool isHigher(int endx, int endh, int maxx, int maxh);
    QImage *highmap;
    QImage *image;
    QPixmap *pixmap;
    QLabel *label;

    int observerpoint = -1;
    QPoint observercoords;
signals:
};

#endif // MAP_H
