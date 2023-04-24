#include "clicktoswipewidget.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QDebug>
#include <QPushButton>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QWidget w;
    auto btn = new ClickToSwipeWidget;
    auto hb = new QHBoxLayout(&w);
    hb->setSizeConstraint(QLayout::SetMinimumSize);
    hb->addStretch();
    hb->addWidget(btn);
    hb->addStretch();

    w.resize(800,400);
    w.show();
    QWidget::connect(btn,&ClickToSwipeWidget::clicked,[](int index)
    {
        if(index == 0)
            qDebug()<<"QQ";
        else if(index == 1)
            qDebug()<<"WX";
        else if(index == 2)
            qDebug()<<"WB";
    });

    return a.exec();
}
