#include "mainwindow.h"
#include "QLayout"
#include "QStackedWidget"
#include "QGroupBox"
#include "QListWidget"
#include "QMessageBox"
#include "networkwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    game_view = new MapView;
    mainLayout->addWidget(game_view);
    game_view->setFrameShape(QFrame::Box);
    game_view->setFixedSize(250,500);

    middleFrame = new QFrame;
    middleFrame->setFrameShape(QFrame::Box);
    mainLayout->addWidget(middleFrame);
    CreateMiddlePanel();


    net_game_view = new MapView;
    mainLayout->addWidget(net_game_view);
    net_game_view->setFrameShape(QFrame::Box);
    net_game_view->setMinimumSize(250,500);
    net_game_view->hide();
    flag = false;

    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    view = new GraphicsView;

    game_state = Ready;
    end_flag = false;
}

MainWindow::~MainWindow(){}

void MainWindow::ShowOpponentView(const Map *map, int socre, int level, int state){
    net_game_view->paintMap(map);
    other_score->setText(QString::number(socre));
    other_level->setText(QString::number(level));
    if(state == 1) other_state->setText("game");
    else if(state == 2){
        other_state->setText("over");
        CheckWinOrLose();
    }
}

void MainWindow::StartGame()
{
    setFocus();
    if(game_state == Ready){
        game_state=Gaming;
        state->setText("游戏中");
        selectButton->setEnabled(false);
        int level = view->getViewScene()->getLevel();
        time_id = startTimer(1000-level*100);
        if(view->getViewScene()->isGameOver() || end_flag) {
            view->getViewScene()->start();
            end_flag = false;
        }
    }
    else if(game_state == Gaming){
        game_state = Ready;
        state->setText("暂停");
        killTimer(time_id);
    }
}

void MainWindow::IsNetGame()
{
    flag = net_game_view->isVisible();

    if(flag) {
        self->hide();
        other->hide();
        net_game_view->hide();
    }
    else {
        self->show();
        other->show();
        net_game_view->show();
    }
    selectStackedWidget->setCurrentIndex(!flag);

    flag = !flag;
}

void MainWindow::CreateMiddlePanel()
{
    QVBoxLayout*  panelLayout = new QVBoxLayout(middleFrame);

    nextItemFrame = new MapView;
    nextItemFrame->setFixedSize(100,100);
    nextItemFrame->setFrameShape(QFrame::Box);

    self = new QWidget;
    QVBoxLayout* selfLayout = new QVBoxLayout(self);
    selfLayout->addWidget(self_score = new QLabel("0"));
    selfLayout->addWidget(self_level = new QLabel("0"));
    selfLayout->addWidget(self_state = new QLabel("free"));

    other = new QWidget;
    QVBoxLayout* otherLayout = new QVBoxLayout(other);
    otherLayout->addWidget(other_score = new QLabel("0"));
    otherLayout->addWidget(other_level = new QLabel("0"));
    otherLayout->addWidget(other_state = new QLabel("free"));

    QWidget* middle_top_widget = new QWidget;
    QHBoxLayout* itemLayout = new QHBoxLayout(middle_top_widget);
    itemLayout->addWidget(self);
    itemLayout->addStretch();
    itemLayout->addWidget(nextItemFrame);
    itemLayout->addStretch();
    itemLayout->addWidget(other);

    panelLayout->addWidget(middle_top_widget);

    selectStackedWidget = new QStackedWidget;
    panelLayout->addWidget(selectStackedWidget);
    selectStackedWidget->setFrameShape(QFrame::Box);
    selectStackedWidget->setFixedSize(220,220);
    CreateSelectPanel();

    selectButton = new QPushButton("单机/网络");
    panelLayout->addWidget(selectButton);
    QLabel* label = new QLabel("彭鹏程 个人作品");
    label->setAlignment(Qt::AlignCenter);
    panelLayout->addWidget(label);
    connect(selectButton,SIGNAL(clicked()), this, SLOT(IsNetGame()));

    self->hide();
    other->hide();
}

void MainWindow::CreateSelectPanel()
{
    QWidget* stand_alone = new QWidget;
    QVBoxLayout* stand_alone_layout = new QVBoxLayout(stand_alone);

    QGroupBox* box = new QGroupBox("游戏信息(单机)");
    QGridLayout* layout = new QGridLayout(box);
    layout->addWidget(new QLabel("分数："), 0,0);layout->addWidget(score = new QLabel("0"),0,1);
    layout->addWidget(new QLabel("等级："), 1,0);layout->addWidget(level = new QLabel("0"),1,1);
    layout->addWidget(new QLabel("状态："), 2,0);layout->addWidget(state = new QLabel("未开始"),2,1);
    QPushButton* start_button = new QPushButton("开始/暂停");
    stand_alone_layout->addWidget(start_button);
    stand_alone_layout->addWidget(box);
    connect(start_button, SIGNAL(clicked()), this, SLOT(StartGame()));
    start_button->setFocusPolicy(Qt::NoFocus);

    online = new NetworkWidget;
    connect(online, SIGNAL(ShowOpponentView(const Map*,int,int,int)), this, SLOT(ShowOpponentView(const Map*,int,int,int)));
    connect(online, SIGNAL(Notify_Game_Start()), this, SLOT(StartGame()));
    connect(online, SIGNAL(Notify_Game_End()), this, SLOT(EndGame()));

    selectStackedWidget->addWidget(stand_alone);
    selectStackedWidget->addWidget(online);
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    if(game_state != Gaming ) return;
    if(view->isGameOver()) return;

    if(event->timerId() == time_id){
        SceneItem* item = view->getViewItem();
        item->downMove();

        if(!flag) UpdatePrint();
        else UpdateNetPrint();

        int level = view->getViewScene()->getLevel();
        killTimer(time_id);
        time_id = startTimer(1000-level*100);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(game_state != Gaming) return;
    if(view->isGameOver()) return;

    SceneItem* item = view->getViewItem();
    if(item == NULL) return;
    switch(event->key()){
    case Qt::Key_Down : item->downMove();break;
    //case Qt::Key_Up: item->upMove();break;
    case Qt::Key_Left: item->leftMove();break;
    case Qt::Key_Right: item->rightMove();break;
    case Qt::Key_Space: item->changeMove();break;
    }

    if(!flag) UpdatePrint();
    else UpdateNetPrint();
}

void MainWindow::UpdatePrint()
{
    SceneItem* item = view->getViewItem();

    if(view->getViewScene()->isGameOver()) {
        game_state = Ready;
        selectButton->setEnabled(true);
    }

    game_view->paintMap(view->getViewMap());
    nextItemFrame->paintMap(item->getNextItemMap());

    score->setText(QString::number(view->getViewScene()->getSocre()));
    level->setText(QString::number(view->getViewScene()->getLevel()));
    switch (game_state) {
    case Ready:
        state->setText("准备");break;
    case Gaming:
        state->setText("游戏中");break;
    }
}

void MainWindow::UpdateNetPrint()
{
    SceneItem* item = view->getViewItem();

    self_state->setText("game");
    bool flag = view->getViewScene()->isGameOver();
    if(flag){
        self_state->setText("over");
        CheckWinOrLose();
    }
    game_view->paintMap(view->getViewMap());
    nextItemFrame->paintMap(item->getNextItemMap());

    int score = view->getViewScene()->getSocre();
    int level = view->getViewScene()->getLevel();

    self_score->setText(QString::number(score));
    self_level->setText(QString::number(level));

    online->SendMap(view->getViewMap(), score, level, flag+1);
}

void MainWindow::CheckWinOrLose()
{
    QString me = self_state->text();
    QString he = other_state->text();

    if(me!="over" || he!="over") return;
    EndGame();
}

void MainWindow::EndGame()
{
    int me_s = self_score->text().toInt();
    int he_s = other_score->text().toInt();

    if(me_s > he_s){
        self_state->setText("win");
        other_state->setText("lose");
    }
    else if(me_s < he_s){
        self_state->setText("lose");
        other_state->setText("win");
    }
    else {
        self_state->setText("^_^");
        other_state->setText("^_^");
    }

    game_state = Ready;
    selectButton->setEnabled(true);
    end_flag = true;

    if(flag) online->EndGame();
}
