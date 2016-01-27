#include "map.h"
#include "itemfactory.h"

GraphicsItemMap::GraphicsItemMap(int size) : map(NULL)
{
    map = createMap(size);
    pix[0] = map;
    for(int i=1; i< maxMapCount; i++)
        pix[i] = createNextMap(pix[i-1]);
    index = 0;
}

GraphicsItemMap::~GraphicsItemMap()
{
    freeMap(map);
    for(int i=0; i<maxMapCount; i++)
         freeMap(pix[i]);
}

Map* GraphicsItemMap::createMap(int size)
{
    if(size<=0) return NULL;
    Map* ret = new Map(size,size);
    return ret;
}

Map* GraphicsItemMap::createNextMap(const Map *cmap)
{
    if( cmap == NULL) return NULL;
    int size = cmap->getSize();

    Map* temp = createMap(size);
    for(int i=0; i<size; i++)
         for(int j=0; j<size; j++){
              int x=i,y=j;
              translate(x,y);
              temp->setMapValue(i,j,cmap->getMapValue(x,y));
         }
    return temp;
}

void GraphicsItemMap::freeMap(Map* &map)
{
    if(map == NULL) return;
    delete map;
    map = NULL;
}

void GraphicsItemMap::translate(int &x, int &y)
{
    int length = map->getSize() - 1;
    double x0 = length/2.0, y0=x0;
    double x1=x,y1=y;
    x1-=x0;y1-=y0;
    double temp = x1;
    x1 = y1;
    y1 = -temp;
    x1+=x0, y1+=y0;
    x = (int)x1; y = (int)y1;
}

int GraphicsItemMap::getSize() const {
    if(map) return map->getSize();
    return 0;
}

const Map* GraphicsItemMap::getMap(int index) {
    if(index >=0 && index<maxMapCount) return pix[index];
    return NULL;
}

const Map* GraphicsItemMap::getCurrentMap(){
    return pix[index];
}

void GraphicsItemMap::setMap(int x, int y, bool flag){
    map->setMapValue(x,y,flag);
    int tx=x,ty=y;
    for(int i=1; i<maxMapCount; i++){
            translate(tx,ty);
            pix[i]->setMapValue(tx,ty,flag);
    }
}

void GraphicsItemMap::setCurrentIndex(int index)
{
    if(index >= 0 && index < maxMapCount) this->index = index;
}

int GraphicsItemMap::getCurrentIndex()
{
    return index;
}

void GraphicsItemMap::next()
{
    index++;
    if(index>=maxMapCount) index=0;
}

//---------------------------------------------SceneItem--------------------------------

SceneItem::SceneItem() : x(0),y(0),scene(NULL),sceneMap(NULL),grit(NULL)
{
    //isGameOver = false;
    next=ItemFactory::createGraphicsItem();
}

SceneItem::~SceneItem()
{
    if(scene!=NULL) delete scene;
    if(sceneMap!=NULL) delete sceneMap;
    if(grit!=NULL) delete grit;
    if(next!=NULL) delete next;
}

const Map* SceneItem::getItemMap()
{
      if(grit!=NULL) return grit->getMap();
      return NULL;
}

const Map* SceneItem::getNextItemMap()
{
    if(next!=NULL) return next->getMap();
    return NULL;
}

void SceneItem::changeMove()
{
     int index = grit->getIndex();
     grit->next();
     if(checkOut(x,y)) return;
     if(y<0 && checkOut(x,y+1)) {y+=1;return;}
     if(y>0 && checkOut(x,y-1)) {y-=1;return;}
     grit->setIndex(index);
}

void SceneItem::upMove()
{
     if(checkOut(x-1,y)) x-=1;
}

void SceneItem::leftMove()
{
     if(checkOut(x,y-1)) y-=1;
}

void SceneItem::rightMove()
{
     if(checkOut(x,y+1)) y+=1;
}

void SceneItem::downMove()
{
     if(checkOut(x+1, y)) x+=1;
     else {
          updateSceneFromItem(scene->getSceneMap());
          setNewItem(next);
     }
}

bool SceneItem::checkOut(int x, int y)
{
     if(sceneMap == NULL || scene==NULL) return false;

     const Map* itemMap = grit->getMap();
     int width = scene->getSceneMap()->getColumn();
     if(x < 0 && x>-itemMap->getRow() && y>0 && y+itemMap->getColumn() < width) return true;

     updateFromScene(x,y);
     for(int i=0; i<size; i++)
          for(int j=0; j<size; j++)
               if(sceneMap->getMapValue(i,j)&&itemMap->getMapValue(i,j)) return false;
     return true;
}

void SceneItem::setPosition(int x, int y)
{
     this->x = x;this->y = y;
}

void SceneItem::setScene(GraphicsScene* scene)
{
     if(this->scene == NULL) {
         this->scene=scene;
         //isGameOver = scene->isGameOver();
     }
}

void SceneItem::setNewItem(GraphicsItem* &item)
{
     if(item == NULL) return;

     if(sceneMap != NULL) delete sceneMap;
     if(grit!=NULL) delete grit;

     const Map* smap = scene->getSceneMap();
     int width  = smap->getColumn();

     grit = item;

     size = grit->getSize();
     sceneMap = new Map(size, size);

     next=ItemFactory::createGraphicsItem();

     x = -size; y=(width-size)/2;
}

void SceneItem::updateFromScene(int x, int y)
{
     if( scene==NULL || sceneMap==NULL) return;
     for(int i=0; i<size; i++)
          for(int j=0; j<size; j++){
               sceneMap->setMapValue(i,j, scene->getScene(x+i,y+j));
          }
}

void SceneItem::updateSceneFromItem(Map* sceneMap)
{
     if(sceneMap == NULL) return;
     if( grit==NULL) return;

     const Map* itemMap = grit->getMap();
     for(int i=0; i<size; i++)
          for(int j=0; j<size; j++){
                  bool value = itemMap->getMapValue(i,j);
                  if(value) sceneMap->setMapValue(x+i,y+j, value);
          }
     if(sceneMap == scene->getSceneMap()) scene->checkOut();
}
