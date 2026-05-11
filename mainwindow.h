#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPaintEvent>
#include <QVector>
#include <QPoint>
#include <QTimer>
#include <QPixmap>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


  class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    Ui::MainWindow *ui;
    struct Civilization
    {
        QPoint pos;

        // 基础属性
        int speed;
        int life;
        int level;

        // 🌍 新文明属性
        int population;     // 人口
        int technology;     // 科技等级
        int energy;         // 能源
        int age;            // 文明年龄
        int stability;      // 社会稳定度
        int visibility;     // 暴露度
        int aggression;     // 扩张欲 / 侵略性
        int military;       // 军事实力
    };

    struct ResourcePlanet
    {
        QPoint pos;

        int energy;      // 资源储量
    };

    QVector<Civilization> civilizations;
    QVector<int> history;   // ⭐ 文明数量历史
    QVector<ResourcePlanet> planets;
    int selectedIndex = -1;  // ⭐ 当前选中的文明
    QTimer *timer;
    QPixmap background;
};
#endif // MAINWINDOW_H