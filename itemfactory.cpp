#include "itemfactory.h"
#include "map.h"
#include "cstdlib"

class GraphicsItem1 : public GraphicsItem{
public:
       GraphicsItem1() : GraphicsItem(3)
       {
            makeItem();
       }

       void makeItem(){
            map.setMap(1,0,true);
            map.setMap(1,1,true);
            map.setMap(1,2,true);
            map.setMap(2,2,true);
       }
};

class GraphicsItem2 : public GraphicsItem{
      unsigned int index;
public:
       GraphicsItem2() : GraphicsItem(4),index(0) { makeItem();}
       void makeItem(){
            map.setMap(1,0,true);
            map.setMap(1,1,true);
            map.setMap(1,2,true);
            map.setMap(1,3,true);
       }

       void next(){
            index = !index;
            map.setCurrentIndex(index);
       }
};

class GraphicsItem3 : public GraphicsItem
{
public:
    GraphicsItem3() : GraphicsItem(3){
        makeItem();
    }
    void makeItem(){
        map.setMap(1,1,true);
        map.setMap(2,0,true);
        map.setMap(2,1,true);
        map.setMap(2,2,true);
    }
};

class GraphicsItem4 : public GraphicsItem
{
    int index;
public:
    GraphicsItem4() : GraphicsItem(3){
        makeItem();
    }
    void makeItem(){
        map.setMap(0,0,true);
        map.setMap(1,0,true);
        map.setMap(1,1,true);
        map.setMap(2,1,true);
    }

    void next(){
        index = !index;
        map.setCurrentIndex(index);
    }
};

class GraphicsItem5 : public GraphicsItem
{
public:
    GraphicsItem5() : GraphicsItem(2){
        makeItem();
    }

    void makeItem(){
        map.setMap(0,0,true);
        map.setMap(0,1,true);
        map.setMap(1,0,true);
        map.setMap(1,1,true);
    }
};

class GraphicsItem6 : public GraphicsItem
{
public:
    GraphicsItem6() : GraphicsItem(3){
        makeItem();
    }

    void makeItem(){
         map.setMap(1,0,true);
         map.setMap(1,1,true);
         map.setMap(1,2,true);
         map.setMap(2,0,true);
    }
};

class GraphicsItem7 : public GraphicsItem
{
public:
    GraphicsItem7() : GraphicsItem(3){
        makeItem();
    }

    void makeItem(){
        map.setMap(0,2,true);
        map.setMap(1,2,true);
        map.setMap(1,1,true);
        map.setMap(2,1,true);
    }
};


ItemFactory::ItemFactory()
{
}

GraphicsItem* ItemFactory::item[7] = {
  new GraphicsItem1,
  new GraphicsItem2,
  new GraphicsItem3,
  new GraphicsItem4,
  new GraphicsItem5,
  new GraphicsItem6,
  new GraphicsItem7
};

GraphicsItem* ItemFactory::getGrapicsItem()
{
    int num =  rand()%7;
    GraphicsItem* ret = NULL;
    ret = item[num];
    int state = rand()%4;
    while(state--) ret->next();
    return ret;
}

#include "time.h"
GraphicsItem* ItemFactory::createGraphicsItem()
{
    srand((int)time(0));
    int num = rand()%7;
    GraphicsItem* ret = NULL;
    switch (num) {
    case 0:ret = new GraphicsItem1;break;
    case 1:ret = new GraphicsItem2;break;
    case 2:ret = new GraphicsItem3;break;
    case 3:ret = new GraphicsItem4;break;
    case 4:ret = new GraphicsItem5;break;
    case 5:ret = new GraphicsItem6;break;
    case 6:ret = new GraphicsItem7;break;
    default:
        break;
    }
    return ret;
}
