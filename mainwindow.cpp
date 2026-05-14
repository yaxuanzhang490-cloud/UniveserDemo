#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPainter>
#include <QRandomGenerator>
#include <QTimer>
#include <QDebug>
#include <QKeyEvent>
#include <cmath>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setMouseTracking(true);
    centralWidget()->setMouseTracking(true);
    // =========================
    // 🌌 初始化默认速度系统（修复关键）
    // =========================
    sliderRatio = 0.5f;   // 默认在中间 → 1x速度附近
    timeSpeed   = 1.0f;   // 默认正常速度
    // =========================
    // 🌌 外交线控制按钮
    // =========================

    allianceBox =
        new QCheckBox("Alliance", this);

    enemyBox =
        new QCheckBox("Enemy", this);

    neutralBox =
        new QCheckBox("Neutral", this);
    connect(allianceBox, &QCheckBox::toggled,
            this,
            [=](bool checked)
            {
                showAlliance = checked;
                update();
            });

    connect(enemyBox, &QCheckBox::toggled,
            this,
            [=](bool checked)
            {
                showEnemy = checked;
                update();
            });

    connect(neutralBox, &QCheckBox::toggled,
            this,
            [=](bool checked)
            {
                showNeutral = checked;
                update();
            });

    // 默认状态
    allianceBox->setChecked(false);
    enemyBox->setChecked(false);
    neutralBox->setChecked(false);


    // =========================
    // 🌌 按钮样式
    // =========================

    allianceBox->setStyleSheet(
        "QCheckBox { color: white; }"
        "QCheckBox::indicator { width: 15px; height: 15px; }"
        );

    enemyBox->setStyleSheet(
        "QCheckBox { color: white; }"
        "QCheckBox::indicator { width: 15px; height: 15px; }"
        );

    neutralBox->setStyleSheet(
        "QCheckBox { color: white; }"
        "QCheckBox::indicator { width: 15px; height: 15px; }"
        );
    // =========================
    // ⏸️ 暂停按钮
    // =========================

    pauseButton =
        new QPushButton("Pause", this);
    // 🌌 初始隐藏UI按钮
    allianceBox->hide();
    enemyBox->hide();
    neutralBox->hide();
    pauseButton->hide();


    pauseButton->setGeometry(width() - 140,
                             height() - 50,
                             100,
                             32);

    pauseButton->setStyleSheet(
        "QPushButton {"
        "background-color: rgba(40,40,60,180);"
        "color: white;"
        "border: 1px solid gray;"
        "border-radius: 8px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background-color: rgba(80,80,120,200);"
        "}"
        );

    // =========================
    // 🌌 初始禁用按钮
    // =========================

    allianceBox->setEnabled(false);

    enemyBox->setEnabled(false);

    neutralBox->setEnabled(false);

    pauseButton->setEnabled(false);
    connect(pauseButton,
            &QPushButton::clicked,
            this,
            [=]()
            {
                isPaused = !isPaused;

                if (isPaused)
                {
                    timer->stop();



                    // 🌌 自动导出文明报告
                    exportCivilizationReport();

                    pauseButton->setText("Resume");
                }
                else
                {
                    timer->start(100);

                    pauseButton->setText("Pause");
                }
            });
    background.load("D:/Qt_code/UniverseDemo/star.png");
    coverImage.load( "D:/Qt_code/UniverseDemo/cover.png");
    // =========================
    // 🌌 创建封面粒子
    // =========================

    for (int i = 0; i < 100; i++)
    {
        MenuParticle p;

        p.pos = QPointF(
            QRandomGenerator::global()->bounded(width()),
            QRandomGenerator::global()->bounded(height())
            );

        p.speedX =
            QRandomGenerator::global()
                ->bounded(-10, 11) / 50.0f;

        p.speedY =
            QRandomGenerator::global()
                ->bounded(-10, 11) / 50.0f;

        p.size =
            QRandomGenerator::global()
                ->bounded(1, 4);

        p.alpha =
            QRandomGenerator::global()
                ->bounded(40, 140);
        // 🌌 随机决定变亮还是变暗
        if (QRandomGenerator::global()->bounded(2) == 0)
        {
            p.alphaDirection = 1;
        }
        else
        {
            p.alphaDirection = -1;
        }

        menuParticles.append(p);
    }
    qDebug() << background.isNull();
    resize(1200, 700);    //窗口变宽
    setAutoFillBackground(false);
    setAttribute(Qt::WA_OpaquePaintEvent);//⭐粒子运动尾迹
    for(int i = 0; i < 40; i++)
    {
        int x = QRandomGenerator::global()->bounded(width());
        int y = QRandomGenerator::global()->bounded(height());

        Civilization c;
        c.pos = QPoint(x, y);
        c.speed = QRandomGenerator::global()->bounded(1, 6); // 1~5
        c.life = QRandomGenerator::global()->bounded(50, 150);// ⭐生命值初始化
        c.level = QRandomGenerator::global()->bounded(1, 4);// ⭐ 文明等级初始化
        // 🌍 文明属性初始化
        c.population = QRandomGenerator::global()->bounded(500, 5000);

        c.technology = QRandomGenerator::global()->bounded(1, 6);

        c.energy = QRandomGenerator::global()->bounded(50, 150);

        c.age = 0;

        c.stability = QRandomGenerator::global()->bounded(40, 100);

        c.visibility = QRandomGenerator::global()->bounded(10, 80);

        c.aggression = QRandomGenerator::global()->bounded(0, 100);

        c.military = QRandomGenerator::global()->bounded(20, 100);

        // =========================
        // 🌍 文明名称生成
        // =========================

        QString prefixes[] =
            {
                "Nova",
                "Eden",
                "Orion",
                "Titan",
                "Astra",
                "Zenith",
                "Solar",
                "Nebula"
            };

        QString suffixes[] =
            {
                "Union",
                "Empire",
                "Alliance",
                "Federation",
                "Collective",
                "Dominion"
            };

        QString prefix =
            prefixes[
                QRandomGenerator::global()->bounded(8)
        ];

        QString suffix =
            suffixes[
                QRandomGenerator::global()->bounded(6)
        ];

        c.name = prefix + " " + suffix;

        // =========================
        // 🌌 文明类型
        // =========================

        if (c.aggression > 70)
        {
            c.type = "Aggressive";
        }
        else if (c.visibility < 30)
        {
            c.type = "Hidden";
        }
        else if (c.stability > 80)
        {
            c.type = "Peaceful";
        }
        else
        {
            c.type = "Balanced";
        }
        // =========================
        // 🏛️ 政治制度生成
        // =========================

        if (c.military > 120 &&
            c.aggression > 70)
        {
            c.politics =
                "Military Dictatorship";
        }
        else if (c.technology > 7)
        {
            c.politics =
                "Technocracy";
        }
        else if (c.stability > 80 &&
                 c.aggression < 30)
        {
            c.politics =
                "Peace Republic";
        }
        else if (c.visibility < 25)
        {
            c.politics =
                "Hidden Civilization";
        }
        else if (c.energy > 130)
        {
            c.politics =
                "Resource Empire";
        }
        else if (c.stability < 30)
        {
            c.politics =
                "Chaos Tribe";
        }
        else
        {
            c.politics =
                "Balanced Federation";
        }
        // =========================
        // 🌌 文明阵营
        // =========================

        QString factions[] =
            {
                "Blue Alliance",
                "Red Empire",
                "Green Union",
                "Golden Federation"
            };

        c.faction =
            factions[
                QRandomGenerator::global()->bounded(4)
        ];

        civilizations.append(c);
        // =========================
        // 🌌 初始化外交关系
        // =========================

        for (int i = 0; i < civilizations.size(); i++)
        {
            civilizations[i].relations.resize(
                civilizations.size());

            for (int j = 0; j < civilizations.size(); j++)
            {
                if (i == j)
                {
                    civilizations[i].relations[j] = 100;
                }
                else
                {
                    civilizations[i].relations[j] =
                        QRandomGenerator::global()
                            ->bounded(-30, 31);
                }
            }
        }
    }

    // =========================
    // 🌍 创建资源星球
    // =========================

    for (int i = 0; i < 8; i++)
    {
        ResourcePlanet p;

        p.pos = QPoint(
            QRandomGenerator::global()->bounded(width() - 300),
            QRandomGenerator::global()->bounded(height())
            );

        p.energy =
            QRandomGenerator::global()->bounded(100, 300);

        planets.append(p);
    }

    timer = new QTimer(this);
    uiTimer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, [=]()
    {



        // =========================
        // 🌌 启动渐变动画
        // =========================

        if (startingGame)
        {
            fadeAlpha += 12;



            if (fadeAlpha >= 255)
            {
                fadeAlpha = 255;

                startingGame = false;

                inMenu = false;

                // 🌌 开启宇宙淡入

                fadingIntoUniverse = true;
                // 🌌 启用UI按钮
                allianceBox->setEnabled(true);

                enemyBox->setEnabled(true);

                neutralBox->setEnabled(true);

                pauseButton->setEnabled(true);

                fadeAlpha = 255;
                allianceBox->show();
                enemyBox->show();
                neutralBox->show();
                pauseButton->show();
                // 🌌 根据速度启动宇宙
                // 只在 START 时设置一次


            }

            update();
        }
        // =========================
        // 🌌 宇宙淡入动画
        // =========================

        if (fadingIntoUniverse)
        {
            fadeAlpha -= 8;

            if (fadeAlpha < 0)
                fadeAlpha = 0;
        }
// 🌌 游戏开始后才更新宇宙
// =========================
        if (gameStarted)
    {
        // 🌌 宇宙年份增长
        universeYear++;
        // 🌌 宇宙纪元系统

        // 🌌 宇宙纪元系统

        if (universeYear < 500)
        {
            currentEra = "Ancient Era";

            warModifier = 0.5f;
            techModifier = 0.7f;
            resourceModifier = 1.2f;
        }
        else if (universeYear < 1500)
        {
            currentEra = "Expansion Era";

            warModifier = 1.0f;
            techModifier = 1.2f;
            resourceModifier = 1.0f;
        }
        else if (universeYear < 3000)
        {
            currentEra = "Galactic War Era";

            warModifier = 1.8f;
            techModifier = 1.0f;
            resourceModifier = 0.8f;
        }
        else
        {
            currentEra = "Dark Age";

            warModifier = 0.7f;
            techModifier = 0.5f;
            resourceModifier = 0.6f;
        }
// 🌌 根据速度决定每帧更新次数
        int simulationSteps =
            qMax(1, int(timeSpeed));
        for (int step = 0; step < simulationSteps; step++)
        {
        // 每一帧更新文明
        for (int i = 0; i < civilizations.size(); i++)
        {
            Civilization &c = civilizations[i];

            // =========================
            // 🌍 文明AI移动系统
            // =========================

            // 默认随机移动
            int dx =
                QRandomGenerator::global()
                    ->bounded(2 * c.speed + 1)
                - c.speed;

            int dy =
                QRandomGenerator::global()
                    ->bounded(2 * c.speed + 1)
                - c.speed;



                // =========================
                // 🌌 寻找最近资源星球
                // =========================

                int nearestDist = 999999;

                QPoint targetPos = c.pos;

                for (const ResourcePlanet &p : planets)
                {
                    if (p.energy <= 0) continue;

                    int ddx = p.pos.x() - c.pos.x();
                    int ddy = p.pos.y() - c.pos.y();

                    int dist2 = ddx * ddx + ddy * ddy;

                    if (dist2 < nearestDist)
                    {
                        nearestDist = dist2;

                        targetPos = p.pos;
                    }
                }
                // =========================
                // 🌍 资源星球自动恢复
                // =========================

                for (ResourcePlanet &p : planets)
                {
                    // 小概率恢复资源
                    if (QRandomGenerator::global()->bounded(4) == 0)
                    {
                        p.energy += int(2 * resourceModifier);

                        // 资源上限
                        if (p.energy > 400)
                        {
                            p.energy = 400;
                        }
                    }
                }

                // =========================
                // ⭐ 能源不足时主动靠近资源
                // =========================

                if (c.energy < 60 ||
                    c.technology > 6)
                {
                    if (targetPos.x() > c.pos.x())
                    {
                        dx += 1;
                    }
                    else
                    {
                        dx -= 1;
                    }

                    if (targetPos.y() > c.pos.y())
                    {
                        dy += 1;
                    }
                    else
                    {
                        dy -= 1;
                    }
                }

                // =========================
                // 🚀 移动
                // =========================

                // =========================
                // ⚠️ 弱文明逃离强文明
                // =========================

                for (const Civilization &other : civilizations)
                {
                    if (&other == &c) continue;

                    if (other.life <= 0) continue;

                    // 对方比自己强很多
                    if (other.military > c.military + 50)
                    {
                        int ddx = other.pos.x() - c.pos.x();
                        int ddy = other.pos.y() - c.pos.y();

                        int dist2 = ddx * ddx + ddy * ddy;

                        // 距离较近
                        if (dist2 < 10000)
                        {
                            // 反方向逃跑
                            if (ddx > 0)
                            {
                                dx -= 2;
                            }
                            else
                            {
                                dx += 2;
                            }

                            if (ddy > 0)
                            {
                                dy -= 2;
                            }
                            else
                            {
                                dy += 2;
                            }
                        }
                    }
                }

                // =========================
                // ⚔️ 高侵略文明主动追击弱文明
                // =========================

                if (c.aggression > 70)
                {
                    for (const Civilization &other : civilizations)
                    {
                        if (&other == &c) continue;

                        if (other.life <= 0) continue;

                        // 对方比自己弱
                        if (other.military < c.military)
                        {
                            int ddx =
                                other.pos.x() - c.pos.x();

                            int ddy =
                                other.pos.y() - c.pos.y();

                            int dist2 =
                                ddx * ddx + ddy * ddy;

                            if (dist2 < 40000)
                            {
                                // 主动靠近
                                if (ddx > 0)
                                {
                                    dx += 1;
                                }
                                else
                                {
                                    dx -= 1;
                                }

                                if (ddy > 0)
                                {
                                    dy += 1;
                                }
                                else
                                {
                                    dy -= 1;
                                }
                            }
                        }
                    }
                }

                float moveX = dx * timeSpeed;
                float moveY = dy * timeSpeed;

                c.pos += QPoint(
                    round(moveX),
                    round(moveY)
                    );
                // 🌍 移动消耗能源
                if (QRandomGenerator::global()->bounded(3) == 0)
                {
                    c.energy--;
                }

                // 🌍 缓慢获取能源
                if (QRandomGenerator::global()->bounded(5) == 0)
                {
                    c.energy++;

                    if (c.energy > 150)
                    {
                        c.energy = 150;
                    }
                }

                // 2️⃣ 边界
                int universeWidth = width() - 300; // 右侧300像素留给侧边栏

                if (c.pos.x() < 0) c.pos.setX(0);
                if (c.pos.y() < 0) c.pos.setY(0);
                if (c.pos.x() > universeWidth) c.pos.setX(universeWidth);
                if (c.pos.y() > height()) c.pos.setY(height());

                // 3️⃣ 生命衰减（必须在演化前）
                 if (QRandomGenerator::global()->bounded(3) == 0)
                 {
                    // 🌍 能源不足会导致生命下降
                    if (c.energy <= 0)
                    {
                        c.life -= 2;
                    }
                    else
                    {
                        // 正常缓慢衰老
                        if (QRandomGenerator::global()->bounded(4) == 0)
                        {
                            c.life--;
                        }
                    }
                 }

                // 4️⃣ 演化逻辑
                if (c.life > 100 && c.level < 5)
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

                // 🌍 文明年龄增长
                c.age++;
                // =========================
                // 🌱 文明自然恢复系统
                // =========================

                if (c.energy > 80 &&
                    c.stability > 60)
                {
                    // 小概率恢复生命
                    if (QRandomGenerator::global()->bounded(8) == 0)
                    {
                        c.life++;

                        // 限制生命上限
                        if (c.life > 150)
                        {
                            c.life = 150;
                        }
                    }
                }

                // 🌟 科技发展
                if (c.age % int(200 / techModifier) == 0)
                {
                    c.technology++;

                    if (c.technology > 10)
                    {
                        c.technology = 10;
                    }
                }

                // ⚔️ 科技促进军事实力
                if (QRandomGenerator::global()->bounded(20) == 0)
                {
                    c.military += c.technology / 2;

                    if (c.military > 200)
                    {
                        c.military = 200;
                    }
                }

                // 🌑 隐藏文明更稳定
                if (c.visibility < 30)
                {
                    c.stability += 1;

                    if (c.stability > 100)
                    {
                        c.stability = 100;
                    }
                }
            }
            for (int i = 0; i < civilizations.size(); i++)
            {
                Civilization &c = civilizations[i];  // ⭐⭐⭐ 关键：定义 c
                // =========================
                // 🏛️ 政治制度效果
                // =========================

                // ⚔️ 军政府
                if (c.politics == "Military Dictatorship")
                {
                    // 更容易扩张
                    c.aggression += 1;

                    // 军事增长更快
                    if (QRandomGenerator::global()->bounded(8) == 0)
                    {
                        c.military += 1;
                    }

                    // 稳定度缓慢下降
                    if (QRandomGenerator::global()->bounded(10) == 0)
                    {
                        c.stability--;
                    }
                }

                // 🧠 技术治国
                else if (c.politics == "Technocracy")
                {
                    // 科技发展更快
                    if (QRandomGenerator::global()->bounded(20) == 0)
                    {
                        c.technology++;
                    }

                    // 更低侵略性
                    if (c.aggression > 20)
                    {
                        c.aggression--;
                    }
                }

                // ☮️ 和平共和国
                else if (c.politics == "Peace Republic")
                {
                    // 社会更稳定
                    if (QRandomGenerator::global()->bounded(5) == 0)
                    {
                        c.stability++;
                    }

                    // 缓慢恢复生命
                    if (QRandomGenerator::global()->bounded(8) == 0)
                    {
                        c.life++;
                    }

                    // 不喜欢战争
                    if (c.aggression > 10)
                    {
                        c.aggression--;
                    }
                }

                // 🌑 隐藏文明
                else if (c.politics == "Hidden Civilization")
                {
                    // 更难被发现
                    if (c.visibility > 5)
                    {
                        c.visibility--;
                    }

                    // 科技缓慢提升
                    if (QRandomGenerator::global()->bounded(30) == 0)
                    {
                        c.technology++;
                    }
                }
                // =========================
                // 🌌 属性限制
                // =========================

                if (c.aggression > 100) c.aggression = 100;
                if (c.aggression < 0) c.aggression = 0;

                if (c.stability > 100) c.stability = 100;
                if (c.stability < 0) c.stability = 0;

                if (c.visibility > 100) c.visibility = 100;
                if (c.visibility < 0) c.visibility = 0;

                if (c.technology > 10) c.technology = 10;

                if (c.life > 200) c.life = 200;

                if (c.military > 300) c.military = 300;
                for (int j = i + 1; j < civilizations.size(); j++)
                {
                    // ============================================================
                    // 🛡️ 核心修复：安全校验（第三处修改）
                    // ============================================================
                    // 将检查逻辑放在循环的最开始，防止任何对 relations 的越界访问
                    if (i >= civilizations.size() || j >= civilizations.size() ||
                        j >= civilizations[i].relations.size() ||
                        i >= civilizations[j].relations.size())
                    {
                        continue;
                    }

                    Civilization &a = civilizations[i];
                    Civilization &b = civilizations[j];

                    if (a.life <= 0 || b.life <= 0) continue;

                    int dx = a.pos.x() - b.pos.x();
                    int dy = a.pos.y() - b.pos.y();
                    int dist2 = dx * dx + dy * dy;

                    // 🌌 探测范围（科技越高越远）
                    int detectRangeA = 40 + a.technology * 12;
                    int detectRangeB = 40 + b.technology * 12;
                    int detectA2 = detectRangeA * detectRangeA;
                    int detectB2 = detectRangeB * detectRangeB;

                    bool aFindsB = false;
                    bool bFindsA = false;

                    // 🔭 是否发现对方
                    if (dist2 < detectA2) {
                        int chance = b.visibility + a.technology * 3;
                        if (QRandomGenerator::global()->bounded(100) < chance) aFindsB = true;
                    }
                    if (dist2 < detectB2) {
                        int chance = a.visibility + b.technology * 3;
                        if (QRandomGenerator::global()->bounded(100) < chance) bFindsA = true;
                    }

                    // ⚔️ 只有发现后才产生外交影响
                    if (aFindsB || bFindsA)
                    {
                        int relation = a.relations[j];

                        // 🌌 外交关系自然变化逻辑
                        if (a.faction == b.faction) {
                            a.relations[j] += 1;
                            b.relations[i] += 1;
                        }
                        else
                        {
                            // 🌌 不同阵营关系缓慢变化

                            int randomChange =
                                QRandomGenerator::global()->bounded(-1, 2);

                            a.relations[j] += randomChange;
                            b.relations[i] += randomChange;
                        }

                        // 和平/侵略倾向影响
                        if (a.type == "Peaceful" || b.type == "Peaceful") {
                            a.relations[j] += 1;
                            b.relations[i] += 1;
                        }
                        if (a.aggression > 70 || b.aggression > 70)
                        {
                            // 🌌 高侵略文明更容易恶化关系
                            if (QRandomGenerator::global()->bounded(3) == 0)
                            {
                                a.relations[j] -= 1;
                                b.relations[i] -= 1;
                            }
                        }

                        // 🌌 限制关系值范围 (-100 ~ 100)
                        a.relations[j] = std::clamp(a.relations[j], -100, 100);
                        b.relations[i] = std::clamp(b.relations[i], -100, 100);

                        // 🌌 长期和平会缓慢恢复关系
                        if (dist2 > 50000)
                        {
                            if (a.relations[j] < 20)
                            {
                                a.relations[j]++;
                            }

                            if (b.relations[i] < 20)
                            {
                                b.relations[i]++;
                            }
                        }

                        // 判定战争几率
                        int attackChance = (a.aggression + b.aggression - (a.stability + b.stability) / 3);
                        if (relation > 40) attackChance -= 40;
                        else if (relation < -40) attackChance += 40;

                        attackChance = std::clamp(attackChance, 5, 95);
                        attackChance *= warModifier;
                        if (QRandomGenerator::global()->bounded(100)
                            < attackChance)
                        {
                        // =========================
                        // ⚔️ 文明战力计算
                        // =========================

                        int powerA =
                            a.technology * 5 +
                            a.military * 3 +
                            a.energy +
                            a.stability;

                        int powerB =
                            b.technology * 5 +
                            b.military * 3 +
                            b.energy +
                            b.stability;

                        // =========================
                        // 🌌 战争结果
                        // =========================

                        if (powerA > powerB)
                        {
                            // a 获胜

                            int damage = (powerA - powerB) /25;

                            if (damage < 1) damage = 1;

                            b.life -= damage;
                            a.relations[j] -= 15;
                            b.relations[i] -= 15;

                            // =========================
                            // 🔴 创建战争激光
                            // =========================

                            Laser laser;

                            laser.start = a.pos;
                            laser.end = b.pos;

                            laser.life = 6;

                            lasers.append(laser);

                            // 🌍 战争消耗
                            a.energy -= 3;
                            a.military -= 1;
                            a.stability -= 2;

                            // 🌟 胜利收益
                            a.energy += 5;

                            // 🌌 战争日志
                            eventLogs.append(
                                QString("[Year %1] ")
                                    .arg(universeYear)
                                + a.name +
                                " attacked " +
                                b.name
                                );
                        }
                        else
                        {
                            // b 获胜

                            int damage = (powerB - powerA) / 10;

                            if (damage < 5) damage = 5;

                            a.life -= damage;
                            a.relations[j] -= 15;
                            b.relations[i] -= 15;

                            Laser laser;

                            laser.start = b.pos;
                            laser.end = a.pos;

                            laser.life = 6;

                            lasers.append(laser);

                            b.energy -= 3;
                            b.military -= 1;
                            b.stability -= 2;

                            b.energy += 5;

                            eventLogs.append(
                                QString("[Year %1] ")
                                    .arg(universeYear)
                                + b.name +
                                " attacked " +
                                a.name
                                );
                        }
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

                    // 继承/初始化属性 (防止由于未初始化导致的随机数值)
                    child.aggression = QRandomGenerator::global()->bounded(100);
                    child.visibility = QRandomGenerator::global()->bounded(100);
                    child.stability = QRandomGenerator::global()->bounded(100);
                    child.military = QRandomGenerator::global()->bounded(100);
                    child.energy = QRandomGenerator::global()->bounded(100);
                    child.technology = c.technology;

                    // =========================
                    // 🌍 子文明名称生成
                    // =========================
                    QString prefixes[] = {"Nova", "Eden", "Orion", "Titan", "Astra", "Zenith", "Solar", "Nebula"};
                    QString suffixes[] = {"Union", "Empire", "Alliance", "Federation", "Collective", "Dominion"};

                    child.name = prefixes[QRandomGenerator::global()->bounded(8)] + " " +
                                 suffixes[QRandomGenerator::global()->bounded(6)];

                    // =========================
                    // 🌌 子文明类型与政治制度判定
                    // =========================
                    if (child.aggression > 70) child.type = "Aggressive";
                    else if (child.visibility < 30) child.type = "Hidden";
                    else if (child.stability > 80) child.type = "Peaceful";
                    else child.type = "Balanced";

                    if (child.military > 120 && child.aggression > 70) child.politics = "Military Dictatorship";
                    else if (child.technology > 7) child.politics = "Technocracy";
                    else if (child.stability > 80 && child.aggression < 30) child.politics = "Peace Republic";
                    else child.politics = "Balanced Federation";

                    child.faction = c.faction; // 继承阵营

                    // ============================================================
                    // 🛡️ 核心修复：双向同步所有文明的外交关系表
                    // ============================================================

                    // 1. 让新文明的关系表长度等于：当前文明总数 + 1 (即包含它自己)
                    int currentTotal = civilizations.size();
                    child.relations.fill(0, currentTotal + 1);
                    for (int k = 0; k < child.relations.size(); k++) {
                        child.relations[k] = QRandomGenerator::global()->bounded(-20, 21);
                    }

                    // 2. 将新文明加入总列表
                    civilizations.append(child);

                    // 3. 重要！遍历所有“前辈”文明，给它们的关系表末尾增加一个位置
                    // 这样当老文明索引到最新这个文明时，就不会越界崩溃了
                    for (int k = 0; k < civilizations.size() - 1; k++)
                    {
                        civilizations[k].relations.append(QRandomGenerator::global()->bounded(-20, 21));
                    }

                    eventLogs.append(QString("[Year %1] %2 was born").arg(universeYear).arg(child.name));
                }
                // =========================
                // 🌍 文明采集资源
                // =========================

                for (ResourcePlanet &p : planets)
                {
                    int dx = c.pos.x() - p.pos.x();
                    int dy = c.pos.y() - p.pos.y();

                    int dist2 = dx * dx + dy * dy;

                    // ⭐ 接近资源星球
                    if (dist2 < 1600)
                    {
                        // 🌟 获取资源
                        if (p.energy > 0)
                        {
                            c.energy += 2;

                            p.energy -= 1;

                            // 科技缓慢提升
                            if (QRandomGenerator::global()->bounded(20) == 0)
                            {
                                c.technology++;
                            }

                            // 限制科技上限
                            if (c.technology > 10)
                            {
                                c.technology = 10;
                            }
                        }
                    }
                }
                // =========================
                // 🌍 资源星球恢复
                // =========================

                for (ResourcePlanet &p : planets)
                {
                    if (QRandomGenerator::global()->bounded(10) == 0)
                    {
                        p.energy++;

                        if (p.energy > 300)
                        {
                            p.energy = 300;
                        }
                    }
                }

            }

            // ⭐⭐⭐ 关键：倒序删除死亡文明
            // 💀 移除灭亡的文明
            for (int i = civilizations.size() - 1; i >= 0; i--)
            {
                if (civilizations[i].life <= 0)
                {
                    // 1. 🌌 记录文明灭亡日志 (在删除数据前先获取名字)
                    eventLogs.append(
                        QString("[Year %1] %2 collapsed")
                            .arg(universeYear)
                            .arg(civilizations[i].name)
                        );

                    // 2. ⭐ 维护选中索引的安全性
                    if (i == selectedIndex)
                    {
                        selectedIndex = -1;
                        focusMode = false; // 如果选中的文明灭亡了，关闭聚焦模式
                    }
                    else if (i < selectedIndex)
                    {
                        selectedIndex--; // 如果删掉的是前面的，选中的索引要减1
                    }

                    // 3. 🛡️ 核心修复：先从所有其他文明的备忘录里删掉这个灭亡文明的关系记录
                    // 注意：必须在 civilizations.remove(i) 之前操作，以确保索引对应准确
                    for (int k = 0; k < civilizations.size(); k++)
                    {
                        if (i < civilizations[k].relations.size())
                        {
                            civilizations[k].relations.removeAt(i);
                        }
                    }

                    // 4. 彻底从宇宙列表中移除该文明
                    civilizations.removeAt(i);
                }
            }
            // ⭐ 记录当前文明数量
            history.append(civilizations.size());

            // =========================
            // 🌌 宇宙自然生成文明
            // =========================

            if (civilizations.size() < 20)
            {
                if (QRandomGenerator::global()->bounded(100) < 8)
                {
                    Civilization c;

                    c.pos = QPoint(
                        QRandomGenerator::global()->bounded(width() - 300),
                        QRandomGenerator::global()->bounded(height())
                        );

                    c.speed = 2;
                    c.life = 100;
                    c.level = 1;

                    c.population = 1000;
                    c.technology = 1;
                    c.energy = 120;
                    c.age = 0;
                    c.stability = 80;
                    c.visibility = 20;
                    c.aggression = 20;
                    c.military = 40;
                    // =========================
                    // 🌍 文明名称生成
                    // =========================

                    QString prefixes[] =
                        {
                            "Nova",
                            "Eden",
                            "Orion",
                            "Titan",
                            "Astra",
                            "Zenith",
                            "Solar",
                            "Nebula"
                        };

                    QString suffixes[] =
                        {
                            "Union",
                            "Empire",
                            "Alliance",
                            "Federation",
                            "Collective",
                            "Dominion"
                        };

                    QString prefix =
                        prefixes[
                            QRandomGenerator::global()->bounded(8)
                    ];

                    QString suffix =
                        suffixes[
                            QRandomGenerator::global()->bounded(6)
                    ];

                    c.name = prefix + " " + suffix;

                    // =========================
                    // 🌌 文明类型
                    // =========================

                    if (c.aggression > 70)
                    {
                        c.type = "Aggressive";
                    }
                    else if (c.visibility < 30)
                    {
                        c.type = "Hidden";
                    }
                    else if (c.stability > 80)
                    {
                        c.type = "Peaceful";
                    }
                    else
                    {
                        c.type = "Balanced";
                    }

                    // =========================
                    // 🏛️ 政治制度
                    // =========================

                    if (c.military > 120 &&
                        c.aggression > 70)
                    {
                        c.politics =
                            "Military Dictatorship";
                    }
                    else if (c.technology > 7)
                    {
                        c.politics =
                            "Technocracy";
                    }
                    else if (c.stability > 80 &&
                             c.aggression < 30)
                    {
                        c.politics =
                            "Peace Republic";
                    }
                    else if (c.visibility < 25)
                    {
                        c.politics =
                            "Hidden Civilization";
                    }
                    else if (c.energy > 130)
                    {
                        c.politics =
                            "Resource Empire";
                    }
                    else if (c.stability < 30)
                    {
                        c.politics =
                            "Chaos Tribe";
                    }
                    else
                    {
                        c.politics =
                            "Balanced Federation";
                    }

                    civilizations.append(c);
                }
            }
            // ⭐ 防止无限增长（最多保存300帧）
            if (history.size() > 300)
            {
                history.remove(0);
            }
            // =========================
            // 🔴 更新激光寿命
            // =========================
            for (int i = lasers.size() - 1; i >= 0; i--)
            {
                lasers[i].life--;

                if (lasers[i].life <= 0)
                {
                    lasers.remove(i);
                }
            }
            // =========================
            // 🌌 同步外交关系数组大小
            // =========================

            int civCount = civilizations.size();

            for (int i = 0; i < civCount; i++)
            {
                // 🌌 记录扩容前大小
                int oldSize =
                    civilizations[i].relations.size();

                civilizations[i].relations.resize(civCount);

                for (int j = 0; j < civCount; j++)
                {
                    // 自己对自己
                    if (i == j)
                    {
                        civilizations[i].relations[j] = 100;
                    }

                    // 🌌 新扩展出的关系
                    else if (j >= oldSize)
                    {
                        civilizations[i].relations[j] =
                            QRandomGenerator::global()
                                ->bounded(-30, 31);
                    }
                }
            }
            // =========================
            // 🌌 限制日志数量
            // =========================

            while (eventLogs.size() > 12)
            {
                eventLogs.removeFirst();
            }
        }

}
        update();
            });



    timer->start(100); // 每100毫秒更新一次
    connect(uiTimer, &QTimer::timeout,
           this,
           [=]()
    {

    // =========================
    // 🌌 更新封面粒子
    // =========================
    if (inMenu)
    {
        for (MenuParticle &p : menuParticles)
        {
            p.pos.rx() += p.speedX;
            p.pos.ry() += p.speedY;
            // =========================
            // 🌌 粒子呼吸闪烁
            // =========================

            p.alpha += p.alphaDirection;

            // 到达上限后开始变暗
            if (p.alpha > 150)
            {
                p.alpha = 150;

                p.alphaDirection = -1;
            }

            // 到达下限后开始变亮
            if (p.alpha < 30)
            {
                p.alpha = 30;

                p.alphaDirection = 1;
            }

            // 超出边界重新循环
            if (p.pos.x() < 0)
            {
                p.pos.setX(width());
            }

            if (p.pos.x() > width())
            {
                p.pos.setX(0);
            }

            if (p.pos.y() < 0)
            {
                p.pos.setY(height());
            }

            if (p.pos.y() > height())
            {
                p.pos.setY(0);
            }
        }
        // =========================
        // 🌠 随机生成流星
        // =========================

        if (QRandomGenerator::global()->bounded(100) < 2)
        {
            Meteor m;

            m.pos = QPointF(
                QRandomGenerator::global()->bounded(width()),
                0
                );

            m.speedX =
                -4.0f
                - QRandomGenerator::global()
                      ->bounded(3);

            m.speedY =
                4.0f
                + QRandomGenerator::global()
                      ->bounded(3);

            m.life = 80;

            meteors.append(m);
        }
        // =========================
        // 🌠 更新流星
        // =========================

        for (int i = meteors.size() - 1;
             i >= 0;
             i--)
        {
            meteors[i].pos.rx() +=
                meteors[i].speedX;

            meteors[i].pos.ry() +=
                meteors[i].speedY;

            meteors[i].life--;

            if (meteors[i].life <= 0)
            {
                meteors.remove(i);
            }
        }
        nebulaOffset += 0.2f;

        if (nebulaOffset > width())
        {
            nebulaOffset = 0;
        }

        update();
    }

    });



    uiTimer->start(16);
}


MainWindow::~MainWindow()
{
    delete ui;
}
// =========================
// 🌌 导出文明报告
// =========================
void MainWindow::exportCivilizationReport()
{
    // 文件名（带时间）
    QString fileName =
        QString("D:/Qt_code/UniverseDemo/reports/civilization_report_%1.txt")
            .arg(QDateTime::currentDateTime()
                     .toString("yyyyMMdd_hhmmss"));

    QFile file(fileName);
    // 打开文件
    if (!file.open(QIODevice::WriteOnly |
                   QIODevice::Text))
    {
        return;
    }

    QTextStream out(&file);
    // =========================
    // 🌌 标题
    // =========================
    out << "==============================\n";
    out << " UNIVERSE CIVILIZATION REPORT \n";
    out << "==============================\n\n";

    out << "Generated Time: "
        << QDateTime::currentDateTime()
               .toString("yyyy-MM-dd hh:mm:ss")
        << "\n\n";
    // =========================
    // 🌍 文明统计
    // =========================
    out << "========== CIVILIZATIONS =========="
        << "\n\n";

    for (int i = 0;
         i < civilizations.size();
         i++)
    {
        const Civilization &c = civilizations[i];

        out << "Civilization #"
            << i + 1
            << "\n";

        out << "Name: "
            << c.name
            << "\n";

        out << "Type: "
            << c.type
            << "\n";

        out << "Politics: "
            << c.politics
            << "\n";

        out << "Faction: "
            << c.faction
            << "\n";

        out << "Population: "
            << c.population
            << "\n";

        out << "Technology: "
            << c.technology
            << "\n";

        out << "Energy: "
            << c.energy
            << "\n";

        out << "Military: "
            << c.military
            << "\n";

        out << "Stability: "
            << c.stability
            << "\n";

        out << "Aggression: "
            << c.aggression
            << "\n";

        out << "Life: "
            << c.life
            << "\n";

        out << "Level: "
            << c.level
            << "\n";

        out << "--------------------------------\n";
    }
    // =========================
    // 🌌 事件日志
    // =========================
    out << "\n========== EVENT LOG =========="
        << "\n\n";

    for (const QString &log : eventLogs)
    {
        out << log << "\n";
    }
    // =========================
    // 🌍 总结信息
    // =========================
    out << "\n========== SUMMARY =========="
        << "\n\n";

    out << "Total Civilizations: "
        << civilizations.size()
        << "\n";

    out << "Total Events: "
        << eventLogs.size()
        << "\n";

    file.close();
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    // =========================
    // 🌌 启动菜单界面
    // =========================

    if (inMenu)
    {
        // 绘制封面背景
        painter.drawPixmap(rect(), coverImage);
        // =========================
        // 🌌 动态星云
        // =========================

        painter.setPen(Qt::NoPen);

        // =========================
        // 🌌 星云1
        // =========================

        painter.setBrush(
            QColor(120, 180, 255, 40)
            );

        painter.setPen(Qt::NoPen);

        int nebula1X =
            width() * 0.25;

        int nebula1Y =
            height() * 0.30;

        int nebula1W =
            width() * 0.18;

        int nebula1H =
            height() * 0.12;

        painter.drawEllipse(
            QPointF(
                nebula1X + nebulaOffset * 0.3f,
                nebula1Y
                ),
            nebula1W,
            nebula1H
            );
        /// =========================
        // 🌌 星云2
        // =========================

        painter.setBrush(
            QColor(180, 120, 255, 35)
            );

        int nebula2X =
            width() * 0.75;

        int nebula2Y =
            height() * 0.72;

        int nebula2W =
            width() * 0.22;

        int nebula2H =
            height() * 0.14;

        painter.drawEllipse(
            QPointF(
                nebula2X - nebulaOffset * 0.2f,
                nebula2Y
                ),
            nebula2W,
            nebula2H
            );

        // =========================
        // 🌌 星云3
        // =========================

        painter.setBrush(
            QColor(100, 255, 220, 30)
            );

        int nebula3X =
            width() * 0.50;

        int nebula3Y =
            height() * 0.50;

        int nebula3W =
            width() * 0.15;

        int nebula3H =
            height() * 0.10;

        painter.drawEllipse(
            QPointF(
                nebula3X,
                nebula3Y
                    + sin(nebulaOffset * 0.01f) * 30
                ),
            nebula3W,
            nebula3H
            );

        // =========================
        // 🌌 绘制封面动态粒子
        // =========================

        painter.setPen(Qt::NoPen);

        for (const MenuParticle &p : menuParticles)
        {
            painter.setBrush(
                QColor(
                    255,
                    255,
                    255,
                    p.alpha
                    )
                );

            painter.drawEllipse(
                p.pos,
                p.size,
                p.size
                );
        }
        // =========================
        // 🌠 绘制流星
        // =========================

        for (const Meteor &m : meteors)
        {
            QColor meteorColor(
                255,
                255,
                255,
                180
                );

            QPen pen(meteorColor);

            pen.setWidth(2);

            painter.setPen(pen);

            QPointF tail =
                m.pos -
                QPointF(
                    m.speedX * 8,
                    m.speedY * 8
                    );

            painter.drawLine(
                m.pos,
                tail
                );
        }

        // =========================
        // 🌌 游戏标题
        // =========================

        painter.setPen(Qt::white);

        QFont titleFont;
        int titleSize = width() * 0.025;

        if (titleSize < 24)
        {
            titleSize = 24;
        }

        if (titleSize > 48)
        {
            titleSize = 48;
        }

        titleFont.setPointSize(titleSize);
        titleFont.setBold(true);

        painter.setFont(titleFont);

        // =========================
        // 🌌 标题区域
        // =========================

        // 标题区域高度按窗口比例
        int titleAreaHeight = height() * 0.2;

        // 创建标题区域
        QRect titleRect(
            0,
            height() * 0.05,
            width(),
            titleAreaHeight
            );

        // 绘制标题
        painter.drawText(
            titleRect,
            Qt::AlignCenter,
            "Civilization Simulator"
            );

        // =========================
        // 🌌 时间轴
        // =========================

        int lineY = height() / 2;

        painter.setPen(
            QPen(Qt::white, 3)
            );

        // =========================
        // 🌌 时间轴动态长度
        // =========================

        // 时间轴长度按窗口宽度变化
        int sliderWidth = width() * 0.35;

        // 限制最小/最大长度
        if (sliderWidth < 300)
        {
            sliderWidth = 300;
        }

        if (sliderWidth > 800)
        {
            sliderWidth = 800;
        }

        // 左右边界
        int sliderLeft =
            width()/2 - sliderWidth/2;

        int sliderRight =
            width()/2 + sliderWidth/2;

        // 绘制时间轴
        painter.drawLine(
            sliderLeft,
            lineY,
            sliderRight,
            lineY
            );

        // =========================
        // 🌌 根据比例计算滑块位置
        // =========================

        int sliderX =
            sliderLeft +
            (sliderRight - sliderLeft) * sliderRatio;

        // 绘制滑块
        painter.setBrush(
            QColor(180, 220, 255)
            );

        painter.setPen(
            QPen(Qt::white, 2)
            );

        painter.drawEllipse(
            QPointF(sliderX, lineY),
            12,
            12
            );

        // =========================
        // 🌌 速度文字
        // =========================
        painter.setPen(Qt::white);   //强制恢复画笔颜色

        QFont textFont;
        textFont.setPointSize(12);

        painter.setFont(textFont);

        painter.drawText(
            width()/2 - 240,
            lineY + 55,
            "Slow"
            );

        painter.drawText(
            width()/2 + 190,
            lineY + 55,
            "Fast"
            );

        QString speedText =
            QString("Speed: x%1")
                .arg(timeSpeed, 0, 'f', 1);

        painter.drawText(
            width()/2 - 60,
            lineY - 50,
            speedText
            );

        // =========================
        // 🌌 Start按钮
        // =========================

        // =========================
        // 🌌 Start按钮自适应尺寸
        // =========================

        // 按窗口比例决定按钮大小
        int buttonWidth = width() * 0.16;
        int buttonHeight = height() * 0.07;

        // 限制最小大小
        if (buttonWidth < 180)
        {
            buttonWidth = 180;
        }

        if (buttonHeight < 45)
        {
            buttonHeight = 45;
        }

        // 创建按钮矩形
        QRect startButton(
            width()/2 - buttonWidth/2,
            lineY + height() * 0.08,
            buttonWidth,
            buttonHeight
            );
        // 🌌 是否悬停START按钮
        bool hoverStart =
            startButton.contains(mousePos);

        // =========================
        // 🌌 Start按钮发光
        // =========================

        QColor glowColor(
            180,
            180,
            255,

            hoverStart
                ? 255
                : startGlow
            );

        painter.setBrush(glowColor);

        painter.setPen(Qt::NoPen);

        QRect drawButton = startButton;

        // 🌌 悬停时放大
        if (hoverStart)
        {
            drawButton.adjust(
                -12,
                -6,
                12,
                6
                );
        }

        painter.drawRoundedRect(
            drawButton,
            10,
            10
            );

        painter.setPen(Qt::white);

        painter.drawText(
            startButton,
            Qt::AlignCenter,
            "START"
            );
        // =========================
        // 🌌 黑幕渐变
        // =========================

        if (startingGame)
        {
            painter.setBrush(
                QColor(0, 0, 0, fadeAlpha)
                );

            painter.setPen(Qt::NoPen);

            painter.drawRect(rect());
        }

        return;
    }

    // 🌌 绘制星空背景
    painter.drawPixmap(rect(), background);

    // =========================
    // 🌍 绘制资源星球
    // =========================

    for (const ResourcePlanet &p : planets)
    {
        // ⭐ 资源星球颜色
        if (p.energy > 50)
        {
            painter.setBrush(QColor(0, 150, 255));
        }
        else
        {
            painter.setBrush(QColor(80, 80, 80));
        }

        // ⭐ 根据资源量决定大小
        int radius = p.energy / 40;

        if (radius < 6) radius = 6;
        if (radius > 15) radius = 15;

        painter.drawEllipse(p.pos, radius, radius);
    }

    //  // 背景拖尾
    // painter.setBrush(QColor(0, 0, 0,60)); // 黑色 + 透明度
    // painter.setPen(Qt::NoPen);
    // painter.drawRect(rect());

    // 🌌 绘制右侧信息栏背景
    painter.setBrush(QColor(20, 20, 30, 220));
    painter.setPen(Qt::NoPen);
    painter.drawRect(width() - 300, 0, 300, height());

    //绘制侧边栏标题
    painter.setPen(Qt::white);

    QFont titleFont;
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    painter.setFont(titleFont);

    painter.drawText(width() - 280, 40,
                     "Civilization Monitor");

    // =========================
    // 🌌 外交线控制提示
    // =========================

    QFont smallFont;
    smallFont.setPointSize(9);

    painter.setFont(smallFont);

    painter.setPen(Qt::gray);
    //显示实时文明数量
    QFont normalFont;
    normalFont.setPointSize(10);
    painter.setFont(normalFont);

    QString countText = QString("Civilizations: %1")
                            .arg(civilizations.size());

    painter.drawText(width() - 280, 115, countText);

    QString yearText =
        QString("Universe Year: %1")
            .arg(universeYear);
    QString eraText =
        QString("Era: %1")
            .arg(currentEra);

    painter.drawText(
        width() - 280,
        165,
        eraText
        );

    painter.drawText(
        width() - 280,
        140,
        yearText
        );

    // 📈 在侧边栏绘制实时文明数量折线图
    int graphX = width() - 280;
    int graphY = 300;
    int graphHeight = 120;

    // 边框
    painter.setPen(Qt::gray);
    painter.drawRect(graphX, graphY - graphHeight,
                     240, graphHeight);

    // 折线
    painter.setPen(Qt::yellow);

    for (int i = 1; i < history.size(); i++)
    {
        int x1 = graphX + (i - 1);
        int y1 = graphY - history[i - 1] * 2;

        int x2 = graphX + i;
        int y2 = graphY - history[i] * 2;

        painter.drawLine(x1, y1, x2, y2);
    }

    painter.setPen(Qt::white);
    painter.drawText(graphX, graphY + 20,
                     "Population History");

    // =========================
    // 🔴 绘制战争激光
    // =========================

    for (const Laser &laser : lasers)
    {
        QColor laserColor(255, 0, 0, 180);

        QPen pen(laserColor);

        pen.setWidth(2);

        painter.setPen(pen);

        painter.drawLine(laser.start, laser.end);
    }
    // =========================
    // 🌌 先绘制外交线
    // =========================

    for (int i = 0; i < civilizations.size(); i++)
    {
        const Civilization &c = civilizations[i];

        if (c.life <= 0) continue;

        for (int j = i + 1;
             j < civilizations.size();
             j++)
        {
            const Civilization &other =
                civilizations[j];

            if (other.life <= 0) continue;

            // 🌌 聚焦模式
            if (focusMode)
            {
                // 只显示与选中文明相关的线
                if (i != selectedIndex &&
                    j != selectedIndex)
                {
                    continue;
                }
            }

            if (j >= c.relations.size()
                || j >= civilizations.size())
            {
                continue;
            }

            int relation =
                c.relations[j];
            // 🔵 联盟
            if (allianceBox->isChecked() &&
                relation > 20)
            {
                painter.setPen(
                    QPen(
                        QColor(80, 180, 255, 120),
                        2
                        )
                    );

                painter.drawLine(
                    c.pos,
                    other.pos
                    );
            }

            // 🔴 敌对
            else if (enemyBox->isChecked() &&
                     relation < -30)
            {
                painter.setPen(
                    QPen(
                        QColor(255, 60, 60, 120),
                        2
                        )
                    );

                painter.drawLine(
                    c.pos,
                    other.pos
                    );
            }

            // 🟡 中立
            else if (neutralBox->isChecked())
            {
                painter.setPen(
                    QPen(
                        QColor(255, 220, 80, 60),
                        1
                        )
                    );

                painter.drawLine(
                    c.pos,
                    other.pos
                    );
            }
        }
    }

    // 画文明
    for (int i = 0; i < civilizations.size(); i++)
    {
        const Civilization &c = civilizations[i];

        if (c.life <= 0) continue;
        // =========================
        // 🌌 聚焦模式
        // =========================

        if (focusMode)
        {
            // 不是被选中文明
            // 也不是与它有关联的文明
            if (i != selectedIndex)
            {
                bool related = false;

                int relation =
                    civilizations[selectedIndex].relations[i];

                if (relation != 0)
                {
                    related = true;
                }

                if (!related)
                {
                    continue;
                }
            }
        }

        // 🌍 根据能源决定亮度
        int alpha =
            c.energy +
            c.visibility +
            20;

        if (alpha > 255) alpha = 255;
        if (alpha < 50) alpha = 50;

        // =========================
        // 🌌 根据阵营决定颜色
        // =========================

        if (c.faction == "Blue Alliance")
        {
            painter.setBrush(QColor(80, 160, 255, alpha));
        }
        else if (c.faction == "Red Empire")
        {
            painter.setBrush(QColor(255, 80, 80, alpha));
        }
        else if (c.faction == "Green Union")
        {
            painter.setBrush(QColor(80, 255, 120, alpha));
        }
        else if (c.faction == "Golden Federation")
        {
            painter.setBrush(QColor(255, 220, 80, alpha));
        }
        else
        {
            painter.setBrush(QColor(200, 200, 200, alpha));
        }

        // 🔵 根据生命值决定大小
        int radius = c.life / 12;
        if (radius < 4) radius = 4;
        if (radius > 12) radius = 12;

        // =========================
        // ✨ 文明光晕
        // =========================

        QColor glowColor;

        // 根据等级决定光晕颜色
        switch (c.level)
        {
        case 1:
            glowColor = QColor(180, 180, 180, 40);
            break;

        case 2:
            glowColor = QColor(0, 255, 0, 50);
            break;

        case 3:
            glowColor = QColor(255, 0, 0, 60);
            break;

        case 4:
            glowColor = QColor(180, 0, 255, 70);
            break;

        case 5:
            glowColor = QColor(255, 215, 0, 80);
            break;

        default:
            glowColor = QColor(255, 255, 255, 40);
        }

        // 光晕半径
        int glowRadius = radius + c.level * 4;

        // 无边框
        painter.setPen(Qt::NoPen);

        // 绘制光晕
        painter.setBrush(glowColor);
        // =========================
        // 🌌 文明外交连线
        // =========================


        // =========================
        // 🌌 文明疆域范围
        // =========================

        // 科技 + 人口决定疆域
        int territoryRadius =
            40 +
            c.technology * 8 +
            c.population / 500;

        // 防止太大
        if (territoryRadius > 180)
        {
            territoryRadius = 180;
        }

        // =========================
        // 🌈 阵营颜色疆域
        // =========================

        QColor territoryColor;
        int territoryAlpha = c.energy / 3;

        if (territoryAlpha < 20)
        {
            territoryAlpha = 20;
        }

        if (territoryAlpha > 120)
        {
            territoryAlpha = 120;
        }

        if (c.faction == "Blue Alliance")
        {
            territoryColor =
                QColor(80, 160, 255, territoryAlpha);
        }
        else if (c.faction == "Red Empire")
        {
            territoryColor =
                QColor(255, 80, 80,territoryAlpha);
        }
        else if (c.faction == "Green Union")
        {
            territoryColor =
                QColor(80, 255, 120, territoryAlpha);
        }
        else if (c.faction == "Golden Federation")
        {
            territoryColor =
                QColor(255, 220, 80, territoryAlpha);
        }
        else
        {
            territoryColor =
                QColor(200, 200, 200, 30);
        }

        // 绘制疆域
        painter.setBrush(territoryColor);
        painter.setPen(Qt::NoPen);

        painter.drawEllipse(
            c.pos,
            territoryRadius,
            territoryRadius
            );
        painter.setPen(Qt::NoPen);

        painter.drawEllipse(c.pos, glowRadius, glowRadius);

        painter.drawEllipse(c.pos, radius, radius);
        // =========================
        // 🌌 文明名称标签
        // =========================

        if (c.level >= 3 ||
            i == selectedIndex)
        {
            painter.setPen(Qt::white);

            QFont nameFont;
            nameFont.setPointSize(8);

            painter.setFont(nameFont);

            // 名称
            painter.drawText(
                c.pos.x() - 35,
                c.pos.y() - radius - 10,
                c.name
                );

            // 等级
            painter.drawText(
                c.pos.x() - 15,
                c.pos.y() - radius - 25,
                QString("Lv.%1")
                    .arg(c.level)
                );
        }
    }
    // =========================
    // 🌌 事件日志显示
    // =========================

    painter.setPen(Qt::white);

    QFont logFont;
    logFont.setPointSize(8);

    painter.setFont(logFont);

    int logX = width() - 280;
    int logY = 350;

    // 🌌 日志背景框
    painter.setBrush(QColor(10, 10, 20, 180));
    painter.setPen(QColor(80, 80, 100));

    painter.drawRect(
        width() - 290,
        330,
        270,
        180
        );

    painter.setPen(Qt::white);

    painter.drawText(logX, logY,
                     "[ EVENT LOG ]");

    // 最多显示10条
    int logSize =
        eventLogs.size();

    int start =
        qMax(0, logSize - 10);

    for (int i = start;
         i < logSize;
         i++)
    {
        int line =
            i - start;

        if (i >= 0 && i < eventLogs.size())
        {
            QString text = eventLogs[i];

            if (text.length() > 32)
            {
                text = text.left(32) + "...";
            }

            painter.drawText(
                logX,
                logY + 20 + line * 15,
                text
                );
        }
    }

    // 点击显示信息
    if (selectedIndex != -1 && selectedIndex < civilizations.size())
    {
        const Civilization &c = civilizations[selectedIndex];

        painter.setPen(Qt::white);

        QFont infoFont;
        infoFont.setPointSize(10);
        painter.setFont(infoFont);

        int infoX = width() - 280;
        int infoY = 530;

        // =========================
        // 🌌 标题
        // =========================

        painter.drawText(infoX, infoY,
                         "Selected Civilization");

        painter.drawText(infoX, infoY + 30,
                         QString("Name: %1")
                             .arg(c.name));

        painter.drawText(infoX, infoY + 50,
                         QString("Type: %1")
                             .arg(c.type));

        // =========================
        // 🌍 基础信息
        // =========================
        painter.drawText(infoX, infoY + 70,
                         QString("Politics: %1")
                             .arg(c.politics));
        painter.drawText(infoX, infoY + 90,
                         QString("Faction: %1")
                             .arg(c.faction));
        // =========================
        // 🌌 外交关系统计
        // =========================

        int allyCount = 0;
        int enemyCount = 0;
        int neutralCount = 0;

        for (int i = 0;
             i < c.relations.size()
             && i < civilizations.size();
             i++)
        {
            // 跳过自己
            if (i == selectedIndex)
            {
                continue;
            }

            int relation = c.relations[i];

            if (relation > 40)
            {
                allyCount++;
            }
            else if (relation < -40)
            {
                enemyCount++;
            }
            else
            {
                neutralCount++;
            }
        }

        // 🌟 显示统计

        painter.drawText(infoX, infoY + 120,
                         QString("Allies: %1")
                             .arg(allyCount));

        painter.drawText(infoX, infoY + 140,
                         QString("Enemies: %1")
                             .arg(enemyCount));

        painter.drawText(infoX, infoY + 160,
                         QString("Neutral: %1")
                             .arg(neutralCount));

        painter.drawText(infoX, infoY + 180,
                         QString("Population: %1")
                             .arg(c.population));

        painter.drawText(infoX, infoY + 200,
                         QString("Technology: %1")
                             .arg(c.technology));

        painter.drawText(infoX, infoY + 220,
                         QString("Energy: %1")
                             .arg(c.energy));

        painter.drawText(infoX, infoY + 240,
                         QString("Age: %1")
                             .arg(c.age));

        // =========================
        // ⚖️ 社会属性
        // =========================

        painter.drawText(infoX, infoY + 270,
                         QString("Stability: %1")
                             .arg(c.stability));

        painter.drawText(infoX, infoY + 290,
                         QString("Visibility: %1")
                             .arg(c.visibility));

        painter.drawText(infoX, infoY + 310,
                         QString("Aggression: %1")
                             .arg(c.aggression));

        // =========================
        // ⚔️ 军事属性
        // =========================

        painter.drawText(infoX, infoY + 340,
                         QString("Military: %1")
                             .arg(c.military));

        painter.drawText(infoX, infoY + 360,
                         QString("Life: %1")
                             .arg(c.life));

        painter.drawText(infoX, infoY + 380,
                         QString("Speed: %1")
                             .arg(c.speed));

        painter.drawText(infoX, infoY + 400,
                         QString("Level: %1")
                             .arg(c.level));
    }
    // 🌌 宇宙淡入黑幕（修复版）
    if (fadingIntoUniverse)
    {
        if (fadeAlpha > 0)
        {
            painter.setBrush(QColor(0, 0, 0, fadeAlpha));
            painter.setPen(Qt::NoPen);
            painter.drawRect(rect());
        }
        else
        {
            // 防止 fade 结束后仍然挡住画面
            fadingIntoUniverse = false;
        }
    }


}
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    QPoint clickPos = event->pos();
    // =========================
    // 🌌 菜单界面点击逻辑
    // =========================

    if (inMenu)
    {
        int lineY = height() / 2;

        // =========================
        // 🌌 点击滑块
        // =========================
        int sliderLeft = width()/2 - 200;
        int sliderRight = width()/2 + 200;

        int sliderX =
            sliderLeft +
            (sliderRight - sliderLeft) * sliderRatio;

        int dx = clickPos.x() - sliderX;
        int dy = clickPos.y() - lineY;

        if (dx * dx + dy * dy < 400)
        {
            draggingSlider = true;
        }
        // =========================
        // 🌌 Start按钮呼吸效果
        // =========================

        startGlow += glowDirection;

        if (startGlow > 220)
        {
            startGlow = 220;

            glowDirection = -1;
        }

        if (startGlow < 100)
        {
            startGlow = 100;

            glowDirection = 1;
        }

        // =========================
        // 🌌 点击 START 按钮
        // =========================

        QRect startButton(
            width()/2 - 100,
            lineY + 80,
            200,
            50
            );

        if (startButton.contains(clickPos))
        {
            // 🌌 开始启动动画
            startingGame = true;
            gameStarted = true;

            fadeAlpha = 0;
        }

        return;
    }

    selectedIndex = -1;  // 先清空选择
    // 🌌 默认关闭聚焦
    focusMode = false;

    for (int i = 0; i < civilizations.size(); i++)
    {
        Civilization &c = civilizations[i];

        int dx = c.pos.x() - clickPos.x();
        int dy = c.pos.y() - clickPos.y();

        int dist2 = dx * dx + dy * dy;

        // ⭐ 点击范围（20像素）
        if (dist2 <225)
        {
            selectedIndex = i;
            // 🌌 开启聚焦模式
            focusMode = true;
            break;
        }
    }

    update();  // 触发重绘
}
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    mousePos = event->pos();
    update();
    if (!inMenu) return;

    if (draggingSlider)
    {
        int minX = width()/2 - 200;
        int maxX = width()/2 + 200;

        if (draggingSlider)
        {
            int sliderLeft = width()/2 - 200;
            int sliderRight = width()/2 + 200;

            int mouseX = event->pos().x();

            // 限制范围
            if (mouseX < sliderLeft)
            {
                mouseX = sliderLeft;
            }

            if (mouseX > sliderRight)
            {
                mouseX = sliderRight;
            }

            // 🌌 转换为比例
            sliderRatio =
                float(mouseX - sliderLeft)
                / float(sliderRight - sliderLeft);

            // ⭐ 防止越界（关键修复）
            sliderRatio = std::clamp(sliderRatio, 0.0f, 1.0f);

            // 🌌 时间速度
            timeSpeed =
                pow(4.0f, sliderRatio * 2.0f - 1.0f);

            update();
        }
    }
}
void MainWindow::mouseReleaseEvent(QMouseEvent *)
{
    draggingSlider = false;
}
void MainWindow::resizeEvent(QResizeEvent *event)
{
    allianceBox->setGeometry(width() - 280, 70, 80, 25);

    enemyBox->setGeometry(width() - 190, 70, 80, 25);

    neutralBox->setGeometry(width() - 100, 70, 80, 25);
    pauseButton->setGeometry(width() - 140,
                             height() - 50,
                             100,
                             32);

    QMainWindow::resizeEvent(event);
}