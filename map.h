#ifndef MAP_H
#define MAP_H

#include "iostream"

class Map
{
      const int size;
      const int row,col;
      bool* map;
private:
        int translate(int x, int y) const {
            return (x%row)*col+(y%col);
        }
public:
       Map(int row, int column) : size(row),row(row),col(column),map(NULL)
       {
            map = new bool[row*column];
            for(int i=0; i<row*column; i++)
                 map[i]=0;
       }
       ~Map() { if(map!=NULL) delete map; }

       void setMapValue(int x, int y, bool flag){
            if(isInMap(x,y)) map[translate(x,y)]=flag;
       }
       bool getMapValue(int x, int y) const{
            if(isInMap(x,y)) return map[translate(x,y)];
            return false;
       }

       int  getSize() const{
            return size;
       }

       int  getRow() const{
            return row;
       }

       int  getColumn() const {
            return col;
       }

       bool isInMap(int x, int y) const{
            if(x>=0 && x<row && y>=0 && y<col) return true;
            return false;
       }
};

class GraphicsItemMap{
      static const int maxMapCount = 4;

      Map* map;
      Map* pix[maxMapCount];
      int index;
private:
      Map* createMap(int size);
      Map* createNextMap(const Map* currentMap);
      void freeMap(Map* &map);
      void translate(int &x, int &y);
public:
      GraphicsItemMap(int size);
      ~GraphicsItemMap();

      int getSize() const;
      const Map* getMap(int index);
      const Map* getCurrentMap();
      void setMap(int x, int y, bool flag);
      void setCurrentIndex(int index);
      int getCurrentIndex();
      void next();
};

class GraphicsItem{
protected:
      GraphicsItemMap map;
      GraphicsItem(int size) : map(size){}
public:
      virtual ~GraphicsItem(){}
      virtual void makeItem() = 0;

      int getSize() {
          return map.getSize();
      }

      const Map* getMap(){
            return map.getCurrentMap();
      }

      int getIndex() {
          return map.getCurrentIndex();
      }

      virtual void setIndex(unsigned int index){
              index = index % 4;
              map.setCurrentIndex(index);
      }
      virtual void next(){
           map.next();
      }
};

class GraphicsScene
{
      Map map;
      int socre;
      int level;
      bool IsGameOver;

      bool isFull(int row)
      {
           int column = map.getColumn();
           for(int i=0; i<column; i++)
                if(map.getMapValue(row, i) == false) return false;
           return true;
      }

      void delRow(int row)
      {
           int column = map.getColumn();
           for(int i=row; i>0; i--)
                for(int j=0; j<column; j++)
                     map.setMapValue(i,j, map.getMapValue(i-1,j));
      }
public:
      GraphicsScene(int row, int height) : map(row,height)
      {
          level = 0;
          socre = 0;
          IsGameOver = false;
      }

      bool isGameOver(){
          return IsGameOver;
      }

      Map* getSceneMap(){
            return &map;
      }

      bool getScene(int x,int y)
      {
           if(map.isInMap(x,y))
                return map.getMapValue(x,y);
           return true;
      }

      void setScene(int x, int y, bool flag)
      {
           map.setMapValue(x,y,flag);
      }

      bool isInScene(int x,int y)
      {
           return map.isInMap(x,y);
      }

      bool checkOut()
      {
           int count = 0;
           int row = map.getRow();
           int column = map.getColumn();

           for(int i=0; i<column; i++)
               if(map.getMapValue(0,i)) {IsGameOver = true;return true;}

           for(int i=1; i<row; i++)
                if(isFull(i)) {delRow(i);count++;}
           socre+=10*count;
           if(count>1) socre+=(1<<(count-1))*5;

           level = socre/100;
           if(level > 9) level=9;
           return false;
      }

      int getSocre(){
          return socre;
      }
      int getLevel(){
          return level;
      }

      void start(){
          level = 0;
          socre = 0;
          IsGameOver = false;

          int row = map.getRow();
          int column = map.getColumn();
          for(int i=0; i<row; i++)
              for(int j=0; j<column; j++)
                  map.setMapValue(i,j,false);
      }
};

class SceneItem
{
      int x, y;
      GraphicsScene* scene;
      Map* sceneMap;
      GraphicsItem* grit;
      GraphicsItem* next;
      int size;
      //bool isGameOver;
private:
       bool checkOut(int x, int y);
public:
       SceneItem();
       ~SceneItem();

       void setNewItem(GraphicsItem* &item);
       void setScene(GraphicsScene* scene);
       void setPosition(int x, int y);
       void updateFromScene(int x,int y);
       void updateSceneFromItem(Map* sceneMap);

       const Map* getItemMap();
       const Map* getNextItemMap();

       void leftMove();
       void rightMove();
       void downMove();
       void upMove();
       void changeMove();
};

#endif // MAP_H
