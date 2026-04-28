#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPainter>
#include <QRandomGenerator>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setAutoFillBackground(false);
    setAttribute(Qt::WA_OpaquePaintEvent);//⭐粒子运动尾迹
    for(int i = 0; i < 40; i++)
    {
        int x = QRandomGenerator::global()->bounded(width());
        int y = QRandomGenerator::global()->bounded(height());

        Civilization c;
        c.pos = QPoint(x, y);
        c.speed = QRandomGenerator::global()->bounded(1, 6); // 1~5
        c.life = QRandomGenerator::global()->bounded(50, 150);// ⭐⭐⭐ 就是加这一行（生命值初始化）
        c.level = QRandomGenerator::global()->bounded(1, 4);// ⭐ 就是这一行（文明等级初始化）

        civilizations.append(c);
    }
    timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, [=]()
            {
                // 每一帧更新文明
        for (int i = 0; i < civilizations.size(); i++)
        {
            Civilization &c = civilizations[i];

            // 1️⃣ 移动
            int dx = QRandomGenerator::global()->bounded(2 * c.speed + 1) - c.speed;
            int dy = QRandomGenerator::global()->bounded(2 * c.speed + 1) - c.speed;
            c.pos += QPoint(dx, dy);

            // 2️⃣ 边界
            if (c.pos.x() < 0) c.pos.setX(0);
            if (c.pos.y() < 0) c.pos.setY(0);
            if (c.pos.x() > width()) c.pos.setX(width());
            if (c.pos.y() > height()) c.pos.setY(height());

            // 3️⃣ 生命衰减（必须在演化前）
            c.life--;

            // 4️⃣ 演化逻辑
            if (c.life > 100 && c.level < 3)
            {
                c.level++;
                c.life -= 30;
            }

            if (c.life < 30 && c.level > 1)
            {
                c.level--;
            }

            // 5️⃣ 死亡处理（先标记，不删除）
            if (c.life <= 0)
            {
                c.life = 0;
            }

            if (c.pos.x() < 0) c.pos.setX(0);
            if (c.pos.y() < 0) c.pos.setY(0);
            if (c.pos.x() > width()) c.pos.setX(width());
            if (c.pos.y() > height()) c.pos.setY(height());
        }

        for (int i = 0; i < civilizations.size(); i++)
        {
            Civilization &c = civilizations[i];  // ⭐⭐⭐ 关键：定义 c
            for (int j = i + 1; j < civilizations.size(); j++)
            {
                Civilization &a = civilizations[i];
                Civilization &b = civilizations[j];

                if (a.life <= 0 || b.life <= 0) continue;

                int dx = a.pos.x() - b.pos.x();
                int dy = a.pos.y() - b.pos.y();

                int dist2 = dx * dx + dy * dy;

                // ⭐ 距离阈值（30像素）
                if (dist2 < 900)
                {
                    int event = QRandomGenerator::global()->bounded(100);

                    // 🌌 黑暗森林简化规则
                    if (event < 40)
                    {
                        // a 攻击 b
                        b.life -= 30;
                    }
                    else if (event < 80)
                    {
                        // b 攻击 a
                        a.life -= 30;
                    }
                    else
                    {
                        // 双方互相损伤
                        a.life -= 15;
                        b.life -= 15;
                    }
                }
            }

            if (c.life > 120 && QRandomGenerator::global()->bounded(1000) < 5)
            {
                Civilization child;

                // 🌍 在父文明附近出生
                child.pos = c.pos + QPoint(
                                QRandomGenerator::global()->bounded(21) - 10,
                                QRandomGenerator::global()->bounded(21) - 10
                                );

                child.speed = QRandomGenerator::global()->bounded(1, 6);
                child.life = 80;
                child.level = c.level;

                civilizations.append(child);
            }
        }

        // ⭐⭐⭐ 关键：倒序删除死亡文明
        for (int i = civilizations.size() - 1; i >= 0; i--)
        {
            if (civilizations[i].life <= 0)
            {
                civilizations.remove(i);
            }
        }

        update();
            });

    timer->start(100); // 每100毫秒更新一次
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.setBrush(QColor(0, 0, 0,60)); // 黑色 + 透明度
    painter.setPen(Qt::NoPen);
    painter.drawRect(rect());

    for (int i = 0; i < civilizations.size(); i++)
    {
        const Civilization &c = civilizations[i];

        if (c.life <= 0) continue;

        // 🌈 根据等级设置颜色
        switch (c.level)
        {
        case 1: painter.setBrush(Qt::gray); break;
        case 2: painter.setBrush(Qt::green); break;
        case 3: painter.setBrush(Qt::red); break;
        default: painter.setBrush(Qt::white);
        }

        // 🔵 根据生命值决定大小
        int radius = c.life / 12;
        if (radius < 4) radius = 4;
        if (radius > 12) radius = 12;

        painter.drawEllipse(c.pos, radius, radius);
    }
}