#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include "map.h"

class GraphicsView
{
    Map* viewMap;
    GraphicsScene scene;
    SceneItem scit;

public:
    GraphicsView();
    ~GraphicsView();
    Map* getViewMap();

    void updateViewMap();
    SceneItem* getViewItem();
    GraphicsScene* getViewScene();
    bool isGameOver();
};

#endif // GRAPHICSVIEW_H
