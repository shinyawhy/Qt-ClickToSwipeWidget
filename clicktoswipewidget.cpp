#include "clicktoswipewidget.h"
#include "ui_clicktoswipewidget.h"

const int normalStatusSize = 60;
const int iconSize = 28; // 图标宽度
const int iconShowDstY = (normalStatusSize - iconSize) / 2;// 图标显示最终位置的Y值 16 = （60 - 图标高度28）/ 2
const int iconHideY = normalStatusSize + 10;// 图标隐藏最终位置的Y值

ClickToSwipeWidget::ClickToSwipeWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ClickToSwipeWidget)
{
    ui->setupUi(this);

    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    connect(&timer, &QTimer::timeout, this, &ClickToSwipeWidget::onTimer);
    timer.setInterval(40);

    imgList << QPixmap(":/img/qq.png");
    imgList << QPixmap(":/img/qq_p.png");
    imgList << QPixmap(":/img/wechat.png");
    imgList << QPixmap(":/img/wechat_p.png");
    imgList << QPixmap(":/img/weibo.png");
    imgList << QPixmap(":/img/weibo_p.png");

    assert((!imgList.isEmpty()) && (imgList.size() % 2 == 0));
    for(int i = 0; i < imgList.size() / 2; i++)
    {
        imgRect << QRect(normalStatusSize + i * (normalStatusSize - 10) + (normalStatusSize - 10 - iconSize) / 2, iconHideY, iconSize, iconSize);
    }
}

ClickToSwipeWidget::~ClickToSwipeWidget()
{
    delete ui;
}

void ClickToSwipeWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
       painter.setRenderHint(QPainter::Antialiasing,true);
       painter.setRenderHint(QPainter::SmoothPixmapTransform,true);

       auto rect = event->rect();
       QPainterPath path;
       path.addRoundedRect(rect.adjusted(5,5,-5,-5), normalStatusSize / 2,normalStatusSize / 2);
       painter.setClipPath(path);
       painter.fillRect(rect,Qt::white);

       QPen pen(QColor(0x22a74c),10,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin);
       painter.setPen(pen);

       painter.drawRoundedRect(rect.adjusted(5,5,-5,-5),normalStatusSize / 2,normalStatusSize / 2);

       auto radiu = normalStatusSize / 2 - 6;
       painter.save();
       painter.drawRoundedRect(rect.adjusted(5,5,-5,-5),normalStatusSize / 2,normalStatusSize / 2);
       painter.translate(QPoint(normalStatusSize / 2,normalStatusSize / 2));
       auto lineWidth = radiu/3.0f;

       if(widgetState == state::StretchingWidth_addWidth ||
          widgetState == state::iconShowing ||
          widgetState == state::iconShowed ||
          widgetState == state::iconHiding ||
          widgetState == state::StretchingWidth_reduce)
       {
           painter.rotate(angle);
       }

       painter.drawLine(QPoint(-lineWidth,0),QPoint(lineWidth,0));
       painter.drawLine(QPoint(0,-lineWidth),QPoint(0,lineWidth));

       painter.restore();

       if(widgetState == state::iconShowing ||
          widgetState == state::iconShowed ||
          widgetState == state::iconHiding)
       {
           for(int i = 0;i < imgRect.size();++i)
           {
               painter.drawPixmap(imgRect.at(i),imgList.at(i*2));
           }
       }

       if(widgetState == state::iconShowed && pressIconIndex != -1)
       {
           painter.drawPixmap(imgRect.at(pressIconIndex),imgList.at(pressIconIndex*2+1));
       }

}

QSize ClickToSwipeWidget::sizeHint() const
{
    if(widgetState == state::UnPressed)
    {
        return QSize(normalStatusSize, normalStatusSize);
    }
    else
    {
        return QSize(normalStatusSize + widthChangeValue, normalStatusSize);
    }
}

void ClickToSwipeWidget::mousePressEvent(QMouseEvent *event)
{
    if(widgetState == state::UnPressed)
    {
        widgetState = state::StretchingWidth_addWidth;
        widthChangeValue = 0;
        angle = 0;
        timer.start();
    }
    else if(widgetState == state::iconShowed)
    {
        auto pos = event->pos();
        for (int var = 0; var < imgRect.size(); ++var)
        {
            if(imgRect.at(var).contains(pos))
            {
                pressIconIndex = var;
                break;
            }
        }
        if(pressIconIndex != -1)
        {
            update();
            emit clicked(pressIconIndex);
        }
        else
        {
            if(QRect(10,10,normalStatusSize - 20,normalStatusSize - 20).contains(pos))
            {
                widgetState = state::iconHiding;
                timer.start();
            }
        }
    }
}

void ClickToSwipeWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(widgetState == state::iconShowed)
    {
        if(pressIconIndex != -1)
        {
            pressIconIndex = -1;
            update();
        }
    }
}



void ClickToSwipeWidget::onTimer()
{
    if(widgetState == state::StretchingWidth_addWidth)
    {
        auto thisIsInLayout = testAttribute(Qt::WA_LaidOut);//控件是否在布局中
        if(thisIsInLayout)
            setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);//Minimum：sizeHint() 提供的大小为最小大小，可拉伸（布局中）

       angle += (90.0 / kRotateDuration) * timer.interval(); //修改角度变化量
        auto temp = (imgList.size() / 2 * (normalStatusSize - 10));

        widthChangeValue += (temp / 10);
        if(widthChangeValue > temp)
        {
            widgetState = state::iconShowing;
            angle = 90; //旋转角度调整为90度
        }

        if(!thisIsInLayout)
            adjustSize();//控件不在布局中时调整大小（不在布局中）
        update();

        if(thisIsInLayout)
            setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    }
    else if(widgetState == state::iconShowing)
    {
        bool allVCenter{true};
        for(int i = 0;i < imgRect.size();++i)
        {
            auto rect = imgRect[i];

            auto y = rect.y();
            if(y == iconShowDstY)
            {
                continue;
            }
            allVCenter = false;
            y -= 7;

            if(y < iconShowDstY)
            {
                rect.moveTopLeft(QPoint(rect.x(),iconShowDstY));
            }
            else
            {
                rect.moveTop(y);
            }
            imgRect[i] = rect;

            break;
        }
        if(allVCenter)
        {
            widgetState = state::iconShowed;
            angle = 90; //旋转角度调整为90度
            timer.stop();
        }
        update();
    }
    else if(widgetState == state::iconHiding)
    {
        bool alliconhide{true};
        for(int i = imgRect.size() - 1;i >= 0;--i)
        {
            auto rect = imgRect[i];

            auto y = rect.y();
            if(y == iconHideY)
            {
                continue;
            }
            alliconhide = false;
            y += 7;

            if(y >= iconHideY)
            {
                rect.moveTopLeft(QPoint(rect.x(),iconHideY));
            }
            else
            {
                rect.moveTop(y);
            }
            imgRect[i] = rect;

            break;
        }
        if(alliconhide)
        {
            widgetState = state::StretchingWidth_reduce;
        }
        update();
    }
    else if(widgetState == state::StretchingWidth_reduce)
    {
        auto thisIsInLayout = testAttribute(Qt::WA_LaidOut);
        if(thisIsInLayout)
            setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);

        angle -= 5;
        auto temp = (imgList.size() / 2 * (normalStatusSize - 10));

        widthChangeValue -= (temp / 10);
        if(widthChangeValue <= 0)
        {
            widgetState = state::UnPressed;
            widthChangeValue = 0;
                        angle = 0;
                        update();
            timer.stop();
        }

        if(!thisIsInLayout)
            adjustSize();//控件不在布局中时调整大小（不在布局中）
        update();

        if(thisIsInLayout)
            setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    }
}


