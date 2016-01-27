#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QFrame>
#include <QPaintEvent>
#include <QPixmap>

class Map;

class MapView : public QFrame
{
    Q_OBJECT
private:
    QPixmap* pix;
    const Map* map;
public:
    explicit MapView(QFrame *parent = 0);

    void paintEvent(QPaintEvent *);
    void paintMap(const Map* map);
};

#endif // MAPVIEW_H
