#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPaintEvent>
#include <QVector>
#include <QPoint>
#include <QTimer>
#include <QPixmap>
#include <QCheckBox>
#include <QPushButton>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>

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
    void resizeEvent(QResizeEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

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
        // 🌍 文明名称
        QString name;

        // 🌌 文明类型
        QString type;
        // 🌍 政治制度
        QString politics;
        QString faction;   // 阵营
        QVector<int> relations;   // 与其它文明关系值
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
    int universeYear = 0;
    QString currentEra = "Ancient Era";
    // 🌌 纪元修正值
    float warModifier = 1.0f;
    float techModifier = 1.0f;
    float resourceModifier = 1.0f;
    QTimer *timer;
    QTimer *uiTimer;
    QPixmap background;
    QPixmap coverImage;
    // =========================
    // 🌌 封面动态粒子
    // =========================

    struct MenuParticle
    {
        QPointF pos;

        float speedX;
        float speedY;

        int size;

        int alpha;
        int alphaDirection;
    };

    QVector<MenuParticle> menuParticles;
    // =========================
    // 🌠 流星系统
    // =========================

    struct Meteor
    {
        QPointF pos;

        float speedX;
        float speedY;

        int life;
    };

    QVector<Meteor> meteors;
    // 🌌 时间速度控制
    float timeSpeed = 1.0f;

    // 🌌 星云动态偏移
    float nebulaOffset = 0;

    // 🌌 滑块比例位置（0.0 ~ 1.0）
    float sliderRatio = 0.5f;

    // 🌌 是否正在拖动滑块
    bool draggingSlider = false;
    // 🌌 游戏是否正式开始
    bool gameStarted = false;
    // 🌌 鼠标当前位置
    QPoint mousePos;
    // 🌌 启动渐变动画
    bool startingGame = false;

    // 🌌 黑幕透明度
    int fadeAlpha = 0;
    // 🌌 Start按钮呼吸光效
    int startGlow = 120;

    int glowDirection = 1;
    // 🌌 宇宙淡入动画
    bool fadingIntoUniverse = false;
    // 🌌 外交线控制按钮
    QCheckBox *allianceBox;
    QCheckBox *enemyBox;
    QCheckBox *neutralBox;
    // ⏸️ 暂停按钮
    QPushButton *pauseButton;

    // 🌌 是否暂停
    bool isPaused = false;
    bool showAlliance = true;
    bool showEnemy = true;
    bool showNeutral = false;

    bool focusSelectedRelations = true;
    // =========================
    // ⚔️ 战争激光
    // =========================

    struct Laser
    {
        QPoint start;
        QPoint end;

        int life;   // 激光剩余时间
    };

    QVector<Laser> lasers;
    // =========================
    // 🌌 文明事件日志
    // =========================

    QStringList eventLogs;
    // 🌌 聚焦模式（只显示选中文明关系）
    bool focusMode = false;
    // 🌌 是否处于启动菜单
    bool inMenu = true;
    // 🌌 导出文明记录
    void exportCivilizationReport();
};
#endif // MAINWINDOW_H