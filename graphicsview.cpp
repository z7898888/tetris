#include "graphicsview.h"
#include "itemfactory.h"

GraphicsView::GraphicsView() : scene(20,10)
{
    scit.setScene(&scene);
    //scit.setNewItem(ItemFactory::createGrapicsItem());
    const Map* sceneMap = scene.getSceneMap();
    viewMap = new Map(sceneMap->getRow(), sceneMap->getColumn());
    updateViewMap();

}

GraphicsView::~GraphicsView(){
    if(viewMap!=NULL) delete viewMap;
}

void GraphicsView::updateViewMap()
{
    const Map* sceneMap = scene.getSceneMap();

    for(int i=0; i<viewMap->getRow(); i++)
         for(int j=0; j<viewMap->getColumn(); j++)
            viewMap->setMapValue(i,j,sceneMap->getMapValue(i,j));
    scit.updateSceneFromItem(viewMap);
}

Map* GraphicsView::getViewMap()
{
    updateViewMap();
    return viewMap;
}

SceneItem* GraphicsView::getViewItem()
{
    return &scit;
}

GraphicsScene* GraphicsView::getViewScene()
{
    return &scene;
}

bool GraphicsView::isGameOver(){
    return scene.isGameOver();
}
