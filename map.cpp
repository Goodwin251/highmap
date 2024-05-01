#include "map.h"
#include <QDebug>


Map::Map(QWidget *parent)
    : QWidget{parent}
{
    setFixedSize(1024,1024);
    qInfo("Constructor started");

    image = new QImage;
    image->load("C:/Users/Goodwin/Documents/Highmap/Iraq_mountains.png");
    *image = image->scaled(1024,1024);

    highmap = new QImage;
    *highmap = *image;

    pixmap = new QPixmap;
    *pixmap = pixmap->fromImage(*highmap);

    label = new QLabel("Image", this);
    label->setPixmap(*pixmap);
}

void Map::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        observerpoint = -1;
        *highmap = *image;
        fillMap(event->pos());
    }
    else if (event->button() == Qt::RightButton)
    {
        observerpoint = -1;
        *highmap = *image;
        *pixmap = pixmap->fromImage(*highmap);
        label->setPixmap(*pixmap);
    }

}

bool Map::inRange(QPoint point)
{
    if((0 <= point.x() && point.x() <= image->width() - 1 ) && (0 <= point.y() && point.y() <= image->height() - 1 ))
        return true;
    else
        return false;
}

int Map::findHeight(QPoint pixel)
{
    QColor color = highmap->pixel(pixel);

    return int(float(color.red() + color.blue() + color.green())  / (765 / float(PEAK_HEIGHT)));
}

bool Map::isHigher(int endx, int endh, int maxx, int maxh)
{
    if(maxh < 0)
        return false;
    double t = double (maxx) / double(endx);
    int calch = int(observerpoint + t*(endh-observerpoint));
    return (calch > maxh);
}

void Map::scanTerrain(QVector<QPoint> points)
{
    int localmaxh = observerpoint,
        globalmaxh = -1,
        localmaxc = 0,
        globalmaxc = 0,
        step = 1;
    bool slope = true;
    for (auto p : points)
    {
        if(highmap->pixelColor(p) == VISIBLE)
        {
            step++;
            continue;//skip painted pixel
        }

        if(highmap->pixelColor(p) == INVISIBLE)
        {
            //globalmaxh = -2;
            step++;
            continue;
        }

        bool isVisible = false;
        int h = findHeight(p);

        bool isgreater = h >= observerpoint;

        if(isgreater && globalmaxh == -1) //first peak
        {
            globalmaxh = h;
            isVisible = true;
            globalmaxc = step;
        }
        else if(!isgreater && globalmaxh == -1 && h <= localmaxh && slope)
        {
            localmaxh = h;
            localmaxc = step;
            isVisible = true;
        }
        else if(!isgreater && globalmaxh == -1 && h > localmaxh)
        {
            if(isHigher(step, h, localmaxc, localmaxh))
            {
                localmaxc = step;
                localmaxh = h;
                isVisible = true;
                slope = false;
            }
            else
                isVisible = false;
        }
        else if(!isgreater && globalmaxh == -1 && h <= localmaxh && !slope)
        {
            if(isHigher(step, h, localmaxc, localmaxh))
                isVisible = true;
            else
                isVisible = false;
        }
        else if(isgreater && h > globalmaxh && globalmaxh > 0) //heighter peaks
        {
            if(isHigher(step, h, globalmaxc, globalmaxh))
            {
                globalmaxc = step;
                globalmaxh = h;
                isVisible = true;

            }
            else
                isVisible = false;
        }

        if(isVisible)
            highmap->setPixelColor(p, VISIBLE);
        else
            highmap->setPixelColor(p, INVISIBLE);
        step++;
    }
}


void Map::fillMap(QPoint point)
{
    observercoords = point;
    qDebug() << "Coords:" << observercoords.x() << " " << observercoords.y();
    int height = findHeight(observercoords);
    qDebug()<< "START Height:" << height;

    observerpoint = height + 2;

    highmap->setPixelColor(observercoords, QColor(0,0,0));

    Ray ray(observercoords, this);
    for(int i = 0; i < highmap->size().width(); i++)
    {
        ray.build(QPoint(i,0));
        scanTerrain(ray.points);
    }
//    qDebug() << ray.points.at(0);
    for(int i = 0; i < highmap->size().width(); i++)
    {
        ray.build(QPoint(0,i));
        scanTerrain(ray.points);
    }
//    qDebug() << ray.points.at(0);
    for(int i = 0; i < highmap->size().width(); i++)
    {
        ray.build(QPoint(highmap->size().width() - 1,i));
        scanTerrain(ray.points);
    }
//    qDebug() << ray.points.at(0);
    for(int i = 0; i < highmap->size().width(); i++)
    {
        ray.build(QPoint(i,highmap->size().height() - 1));
        scanTerrain(ray.points);
    }
//    qDebug() << ray.points.at(0);

    *pixmap = pixmap->fromImage(*highmap);
    label->setPixmap(*pixmap);
}


Map::Ray::Ray(QPoint b, Map *parent)
{
    begin = b;
    end = QPoint();
    points = QVector<QPoint>();
    Ray::parent = parent;
}
int Map::Ray::build(QPoint e)
{
    points.clear();

    int x1 = begin.x(),
        y1 = begin.y(),
        x2 = e.x(),
        y2 = e.y();


    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    bool slope = dy>dx;
    if(slope)
    {
        int temp = x1;
        x1 = y1;
        y1 = temp;

        temp = x2;
        x2 = y2;
        y2 = temp;
    }
    bool reversed = x1 > x2;
    if(reversed)
    {
        int temp = x1;
        x1 = x2;
        x2 = temp;

        temp = y1;
        y1 = y2;
        y2 = temp;
    }

    dx = abs(x2 - x1);
    dy = abs(y2 - y1);

    int error = dx % 2;

    int y = y1;
    int ystep;
    y1 < y2 ? ystep = 1: ystep = -1;

    for(int x = x1; x < x2 + 1; x++)
    {
        if(reversed)
            slope ? points.push_front(QPoint(y,x)): points.push_front(QPoint(x,y));
        else
            slope ? points.push_back(QPoint(y,x)): points.push_back(QPoint(x,y));
        error -= dy;
        if(error < 0)
        {
            y+= ystep;
            error += dx;
        }
    }
    return points.size();
}

int Map::Ray::findLength(QPoint end)
{
    if(parent->inRange(end))
        return sqrt(pow(end.x() - begin.x(),2) + pow(end.y() - begin.y(), 2));
    else
        return -1;
}
int Map::Ray::findLength(QPoint begin, QPoint end)
{
    if(parent->inRange(end))
        return sqrt(pow(end.x() - begin.x(),2) + pow(end.y() - begin.y(), 2));
    else
        return -1;
}
