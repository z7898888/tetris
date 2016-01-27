#include "networkwidget.h"
#include "network.h"
#include "map.h"
#include "mapview.h"

#include "QGroupBox"
#include "QLayout"
#include "QPushButton"
#include "QListWidget"
#include "QMessageBox"
#include "QLCDNumber"
#include "QTime"
#include "QTimer"
#include "QComboBox"

class game_info{
public:
    int level;
    int score;
    int state;
};

class PortControl{
protected:
    CtrlCenter ctrl;
    Network net;
public:
    void SendMap(const Map *map,int score, int level, int state, QString ip){
        if(map == NULL) return;
        QByteArray map_msg;
        map_msg.append(4);

        char row = (char)map->getRow();
        char col = (char)map->getColumn();
        map_msg.append(row);
        map_msg.append(col);

        for(int i=0; i<map->getRow(); i++)
            for(int j=0; j<map->getColumn(); j++){
                map_msg.append(map->getMapValue(i,j));
            }
        game_info a;
        a.score = score;
        a.level = level;
        a.state = state;

        map_msg.append((char*)&a, sizeof(game_info));

        net.sendMessage(ip, map_msg);
    }

    void Open(){
        net.openPort();
    }

    void Close(){
        net.closePort();
    }
};

class MyHandler : public Handler{
protected:
    NetworkWidget::GameState& state;
public:
    MyHandler(NetworkWidget::GameState& state): state(state){}
};

class Opponent_Map_Handle: public MyHandler{
    NetworkWidget* widget;
    bool IsMe(const QByteArray &data, const QString &ip){
        if(state != NetworkWidget::Gaming) return false;
        if(data[0]==4 && ip == widget->opponent_ip) return true;
        return false;
    }

    void Action(const QByteArray &data, const QString &){
        unsigned char row = data[1];
        unsigned char col = data[2];
        static Map map(row, col);
        for(int i=0; i<row; i++)
            for(int j=0; j<col; j++){
                map.setMapValue(i,j,data[3+i*col+j]);
            }
        int base = 3+row*col;

        game_info *b =(game_info*)(data.data()+base);

        widget->ShowView(&map, b->score, b->level, b->state);
    }
public:
    Opponent_Map_Handle(NetworkWidget& widget) : MyHandler(widget.game_state){
        this->widget = &widget;
    }
};

class C_Handle_Host_Rs :public MyHandler{
    NetworkWidget* widget;
    bool IsMe(const QByteArray &data, const QString &){
        if(data[0]==1 && data[1]==2) return true;
        return false;
    }

    void Action(const QByteArray &, const QString &ip){
        widget->JoinClientList(ip);
    }
public:
    C_Handle_Host_Rs(NetworkWidget& widget): MyHandler(widget.game_state){
        this->widget = &widget;
    }
};

class C_Handle_Join_Rs :public MyHandler{
    NetworkWidget* widget;
    bool IsMe(const QByteArray &data, const QString &ip){
        if(state!=NetworkWidget::Free) return false;
        if(data[0]==2 && widget->opponent_ip == ip) return true;
        return false;
    }

    void Action(const QByteArray &data, const QString &){
        if(data[1]==2){
            widget->DetermineOpponent();
        }
        else if(data[1]==3){
            QMessageBox::about(0,"拒绝","主机拒绝您的加入");
        }
    }
public:
    C_Handle_Join_Rs(NetworkWidget& widget): MyHandler(widget.game_state){
        this->widget = &widget;
    }
};

class C_Handle_Start_Rs : public MyHandler{
    NetworkWidget* widget;
    bool IsMe(const QByteArray &data, const QString &ip){
        if(state!=NetworkWidget::Ready) return false;
        if(data[0]==3 && widget->opponent_ip == ip) return true;
        return false;
    }

    void Action(const QByteArray &data, const QString &){
        if(data[1]==1){
            widget->start_time = data[2];
            widget->StartGame();
        }
    }
public:
    C_Handle_Start_Rs(NetworkWidget& widget): MyHandler(widget.game_state){
        this->widget = &widget;
    }
};

class Client: public PortControl{
    QByteArray broad;
    QByteArray join;
public:
    Client(NetworkWidget* widget){
        net.setCtrlCenter(&ctrl);
        net.setListeningPort(3333);
        net.setBroadcastPort(2222);

        broad.append(1);broad.append(1);
        join.append(2);join.append(1);

        if(widget!=NULL){
            ctrl.Register(new C_Handle_Host_Rs(*widget));
            ctrl.Register(new C_Handle_Join_Rs(*widget));
            ctrl.Register(new C_Handle_Start_Rs(*widget));
            ctrl.Register(new Opponent_Map_Handle(*widget));
        }
    }

    void Online_Broadcast(){
        net.sendMessage(broad);
    }

    void Join_Host(QString ip){
        net.sendMessage(ip, join);
    }
};

class S_Handle_Client_Broadcast : public Handler{
    Network& net;
    QByteArray rs;

    bool IsMe(const QByteArray &data, const QString &){
        if(data[0]==1 && data[1]==1) return true;
        return false;
    }

    void Action(const QByteArray &, const QString &ip){
        net.sendMessage(ip,rs);
    }
public:
    S_Handle_Client_Broadcast(Network& net):net(net){
        rs.append(1);
        rs.append(2);
    }
};

class S_Handle_Client_Join : public MyHandler{

    NetworkWidget* widget;
    Network& net;
    QByteArray rs_yes;
    QByteArray rs_no;

    bool IsMe(const QByteArray &data, const QString &){
        if(data[0]==2 && data[1]==1) return true;
        return false;
    }

    void Action(const QByteArray &, const QString &ip){
        if(state == NetworkWidget::Free){
            net.sendMessage(ip,rs_yes);
            widget->opponent_ip = ip;
            widget->DetermineOpponent();
        }else{
            net.sendMessage(rs_no);
        }
    }
public:
    S_Handle_Client_Join(NetworkWidget& widget, Network &net) :
        net(net),
        MyHandler(widget.game_state)
    {
        rs_yes.append(2);rs_yes.append(2);
        rs_no.append(2);rs_no.append(3);
        this->widget = & widget;
    }
};

class Server : public PortControl{
    QByteArray startgame;
public:
    Server(NetworkWidget* widget){
        net.setCtrlCenter(&ctrl);
        net.setBroadcastPort(3333);
        net.setListeningPort(2222);

        startgame.append(3);
        startgame.append(1);
        startgame.append(3);

        if(widget != NULL){
            ctrl.Register(new S_Handle_Client_Broadcast(net));
            ctrl.Register(new S_Handle_Client_Join(*widget, net));
            ctrl.Register(new Opponent_Map_Handle(*widget));
        }
    }
    void StartGame(int s_time, QString ip){
        startgame[2] = s_time;
        net.sendMessage(ip, startgame);
    }
};

NetworkWidget::NetworkWidget(QWidget* parent):QWidget(parent)
{
    QGroupBox* box = new QGroupBox("游戏选择(网络)");

    stackWidget = new QStackedWidget;
    CreateSelectWidget();

    QVBoxLayout* boxLayout = new QVBoxLayout(box);
    boxLayout->addWidget(stackWidget);

    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget(box,0,0,1,2);

    mainLayout->addWidget(new QLabel("对手IP："),1,0);mainLayout->addWidget(opponent = new QLabel);
    mainLayout->addWidget(new QLabel("本地IP："),2,0);mainLayout->addWidget(new QLabel(Network::getLocalIp()));
    mainLayout->addWidget(new QLabel("本地状态："),3,0);mainLayout->addWidget(state = new QLabel("自由"));

    game_state = Free;

    client = new Client(this);
    server = new Server(this);

    view = NULL;

    index = 0;
    start_time = 0;
}

void NetworkWidget::CreateSelectWidget()
{
    QPushButton* button = NULL;
    QWidget* widget1 = new QWidget;//游戏选择的主界面--------------------------
    QVBoxLayout* layout1 = new QVBoxLayout(widget1);
    layout1->addWidget(button = new QPushButton("建立主机"));
    connect(button, SIGNAL(clicked()), this, SLOT(GotoServerGUI()));
    layout1->addWidget(button = new QPushButton("寻找主机"));
    connect(button, SIGNAL(clicked()), this, SLOT(GotoClientGUI()));
    stackWidget->addWidget(widget1);

    QWidget* widget2 = new QWidget;  //主机的建立界面---------------------------
    QGridLayout* layout2 = new QGridLayout(widget2);
    layout2->addWidget(button = new QPushButton("开始"),0,0,1,1);
    connect(button, SIGNAL(clicked()), this, SLOT(StartingGame()));
    layout2->addWidget(button = new QPushButton("离开"),0,1,1,1);
    connect(button, SIGNAL(clicked()), this, SLOT(GotoMainGUI()));
    layout2->addWidget(new QLabel("请选择游戏时间（分钟）"),1,0,1,2);
    select_time = new QComboBox;
    for(int i=0; i<=20; i+=5){
        select_time->addItem(QString::number(i));
    }
    layout2->addWidget(select_time,2,0,1,2);
    stackWidget->addWidget(widget2);

    QWidget* widget3 = new QWidget;//客户机的建立界面---------------------------
    QGridLayout* layout3 = new QGridLayout(widget3);
    layout3->addWidget(button = new QPushButton("加入"),0,0,1,1);
    connect(button, SIGNAL(clicked()), this, SLOT(JoiningHost()));
    layout3->addWidget(button = new QPushButton("离开"),0,1,1,1);
    connect(button, SIGNAL(clicked()), this, SLOT(GotoMainGUI()));
    list = new QListWidget;
    layout3->addWidget(list,1,0,1,2);
    stackWidget->addWidget(widget3);


    time = new QTime;
    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), this, SLOT(ShowTime()));
    QWidget* widget4 = new QWidget;//游戏倒计时的界面----------------------------
    QVBoxLayout* layout4 = new QVBoxLayout(widget4);
    layout4->addWidget(new QLabel("游戏结束倒计时"));
    number = new QLCDNumber;
    layout4->addWidget(number);
    stackWidget->addWidget(widget4);
}

void NetworkWidget::GotoMainGUI()
{
    stackWidget->setCurrentIndex(index = 0);
    list->clear();
    opponent_ip="";
    opponent->setText("");
    game_state = Free;
    state->setText("自由");
    client->Close();
    server->Close();
}

void NetworkWidget::GotoClientGUI()
{
    stackWidget->setCurrentIndex(index = 2);
    client->Open();
    client->Online_Broadcast();
}

void NetworkWidget::GotoServerGUI()
{
    stackWidget->setCurrentIndex(index = 1);
    server->Open();
}

void NetworkWidget::JoiningHost()
{
    QListWidgetItem* item = list->currentItem();
    if(item!=NULL){
        opponent_ip = item->text();
    }

    if(!opponent_ip.isEmpty()){
        client->Join_Host(opponent_ip);
    }
}

void NetworkWidget::StartingGame(){
    start_time = select_time->currentText().toInt();
    if(game_state == Ready){
        StartGame();
        server->StartGame(start_time,opponent_ip);
    }
}

void NetworkWidget::JoinClientList(const QString& ip)
{
    for(int i=0; i<list->count(); i++){
        if(ip == list->item(i)->text()) return;
    }
    list->addItem(ip);
}

void NetworkWidget::DetermineOpponent(){
    if(opponent_ip.isEmpty()) return;
    opponent->setText(opponent_ip);
    game_state=Ready;
    state->setText("准备");
}

void NetworkWidget::StartGame(){
    if(game_state!=Ready) return;
    time->setHMS(0,start_time,0);
    if(start_time!=0) timer->start(1000);
    index = stackWidget->currentIndex();
    stackWidget->setCurrentIndex(3);
    number->display(time->toString("mm:ss"));
    game_state = Gaming;
    state->setText("游戏中");
    Notify_Game_Start();
}

void NetworkWidget::EndGame()
{
    stackWidget->setCurrentIndex(index);
    game_state = Ready;
    timer->stop();
}

void NetworkWidget::SendMap(const Map *map,int score, int level, int state)
{
    if(index == 1) server->SendMap(map, score, level, state, opponent_ip);
    if(index == 2) client->SendMap(map, score, level, state, opponent_ip);
}

void NetworkWidget::ShowView(const Map* map, int score, int level, int state)
{
    if(map == NULL) return;
    ShowOpponentView(map,score,level,state);
}

void NetworkWidget::ShowTime()
{
    int min = time->minute();
    int sec = time->second();
    sec--;
    if(sec < 0 ) {
        if(min == 0) {
            sec = 0;
            Notify_Game_End();
        }
        else {
            min--;
            sec = 59;
        }
    }
    time->setHMS(0,min,sec);
    number->display(time->toString("mm:ss"));
}
