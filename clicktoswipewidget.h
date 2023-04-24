#ifndef CLICKTOSWIPEWIDGET_H
#define CLICKTOSWIPEWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QPainter>
#include <QPaintEvent>
QT_BEGIN_NAMESPACE
namespace Ui { class ClickToSwipeWidget; }
QT_END_NAMESPACE

class ClickToSwipeWidget : public QWidget
{
    Q_OBJECT

public:
    ClickToSwipeWidget(QWidget *parent = nullptr);
    ~ClickToSwipeWidget();

private:
    Ui::ClickToSwipeWidget *ui;

signals:
    void clicked(int index);

protected:
    void paintEvent(QPaintEvent *event) override;
    QSize sizeHint() const override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;


private:
    enum class state
    {
      UnPressed, // 未按下
      StretchingWidth_addWidth, // 正在伸长长度
      iconShowing, // 正在显示图标
      iconShowed, // 图标已显示
      iconHiding, // 图标正在隐藏
      StretchingWidth_reduce // 缩减宽度
    };
    state widgetState{state::UnPressed};
    QList<QPixmap> imgList;
    QList<QRect> imgRect;
    int widthChangeValue{0}; // 宽度变化值
    QTimer timer;
    int angle{0};
    int pressIconIndex{-1}; // 按下的图标

private:
    void onTimer();
private:
    static const int kRotateDuration = 200; //旋转动画时长，单位：毫秒


};
#endif // CLICKTOSWIPEWIDGET_H
