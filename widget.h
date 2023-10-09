#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include<QMediaPlayer>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_pushButtonAdd_clicked();
    void on_horizontalSliderVolume_valueChanged(int value);
    void on_pushButtonPlay_clicked();
    void on_pushButtonPause_clicked();
    void on_pushButtonStop_clicked();

    void on_position_changed(qint64 position);
    void on_duration_changed(qint64 duration);



    void on_horizontalSliderProgress_valueChanged(qint64 position);






    void on_pushButtonMute_clicked();

private:
    Ui::Widget *ui;
    QMediaPlayer* m_player;
    bool muted;
};
#endif // WIDGET_H
