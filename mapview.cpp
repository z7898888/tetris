#include "mapview.h"
#include "QPainter"
#include "map.h"

MapView::MapView(QFrame *parent) :
    QFrame(parent)
{
    map=NULL;
}

void MapView::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setBrush(Qt::black);
    painter.drawRect(rect());

    if(map==NULL) return;

    int row = map->getRow();
    int col = map->getColumn();

    if(row<5 && col<5){
        row =4;col=4;
    }

    int h = height();
    int w = width();

    int dh = h/row;
    int dw = w/col;


    painter.setBrush(Qt::red);
    for(int i=0; i<map->getRow(); i++)
        for(int j=0; j<map->getColumn(); j++){
            if(map->getMapValue(i,j)) painter.drawRect(j*dw,i*dh, dw,dh);
        }
}

void MapView::paintMap(const Map *map)
{
    if(map == NULL) return;
    this->map = map;
    update();
}

