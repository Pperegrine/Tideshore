#include "SettlementPage.h"

#include <QHeaderView>
#include <QAbstractItemView>
#include <QFont>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialog>
#include <QMessageBox>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

#include "../config/AssetPaths.h"
#include "AudioManager.h"

SettlementPage::SettlementPage(GameManager* manager, AudioManager* audio, QWidget* parent)
    : QWidget(parent), gameManager(manager), audioManager(audio)
{ setupUi(); refreshPage(); }

void SettlementPage::setupUi()
{
    setStyleSheet("QWidget{background-color:#eaf8ff;} QLabel{color:#1b4463;} QTableWidget{background-color:rgba(255,255,255,220); border:1px solid #b8d9ee; gridline-color:#c7e2f2;} QTableWidget::item:selected{background-color:rgba(183,220,246,170);} ");
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(18,16,18,16);
    mainLayout->setSpacing(10);

    QFrame* header = new QFrame(this);
    header->setStyleSheet("QFrame{background-color: rgba(255,255,255,210); border:1px solid rgba(140,188,220,170); border-radius:10px;}");
    titleLabel = new QLabel("每日结算报告", header);
    titleLabel->setStyleSheet("QLabel{font-size:22px; font-weight:700; color:#1d4d76; background:transparent;}");
    subtitleLabel = new QLabel(header);
    subtitleLabel->setStyleSheet("QLabel{font-size:14px; color:#2f658f; background:transparent;}");
    QVBoxLayout* headerL = new QVBoxLayout(header); headerL->addWidget(titleLabel); headerL->addWidget(subtitleLabel);
    mainLayout->addWidget(header);

    statsFrame = new QFrame(this);
    QHBoxLayout* cards = new QHBoxLayout(statsFrame); cards->setSpacing(8);
    revenueCard = new QLabel(statsFrame); profitCard = new QLabel(statsFrame); customerCard = new QLabel(statsFrame); bestSellerCard = new QLabel(statsFrame);
    for(QLabel* c : {revenueCard,profitCard,customerCard,bestSellerCard}){ c->setMinimumSize(190,78); c->setStyleSheet("QLabel{background-color: rgba(235,248,255,230); border:1px solid rgba(140,188,220,180); border-radius:10px; padding:8px; color:#1c4e71; font-size:14px;}"); cards->addWidget(c);}
    mainLayout->addWidget(statsFrame);

    summaryFrame = new QFrame(this);
    summaryFrame->setStyleSheet("QFrame{background-color: rgba(235,248,255,220); border:1px solid rgba(140,188,220,170); border-radius:10px;}");
    QVBoxLayout* sL = new QVBoxLayout(summaryFrame);
    sL->addWidget(new QLabel("今日总结", summaryFrame));
    summaryTextEdit = new QTextEdit(summaryFrame); summaryTextEdit->setReadOnly(true); summaryTextEdit->setMaximumHeight(120); summaryTextEdit->setStyleSheet("QTextEdit{background:transparent; border:none; color:#204f73;}");
    sL->addWidget(summaryTextEdit);
    mainLayout->addWidget(summaryFrame);

    salesTable = new QTableWidget(this);
    salesTable->setColumnCount(6);
    salesTable->setHorizontalHeaderLabels(QStringList()<<"图标"<<"商品"<<"理论需求"<<"实际售出"<<"缺货数量"<<"表现");
    salesTable->setColumnWidth(0,60);
    salesTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    for(int i=2;i<6;++i) salesTable->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
    salesTable->horizontalHeader()->setStyleSheet("QHeaderView::section{background:#cfe9fb; color:#184765; border:1px solid #b4d8f0; padding:4px; font-size:15px; font-weight:700;}");
    salesTable->setStyleSheet("QTableWidget{font-size:14px; color:#1b4463;} QTableWidget::item{padding:4px;}");
    salesTable->setIconSize(QSize(32,32));
    salesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    salesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainLayout->addWidget(new QLabel("商品销售明细", this));
    mainLayout->addWidget(salesTable, 1);

    QHBoxLayout* footer = new QHBoxLayout();
    footer->addStretch();
    nextDayButton = new QPushButton("前往升级", this);
    nextDayButton->setStyleSheet("QPushButton{background-color: rgba(43,92,130,220); color:white; border-radius:8px; padding:8px 16px; min-width:180px;}");
    footer->addWidget(nextDayButton);
    mainLayout->addLayout(footer);

    connect(nextDayButton, &QPushButton::clicked, this, [this](){ if(audioManager) audioManager->playClickSfx(); onUpgradeClicked(); });
    connect(salesTable, &QTableWidget::cellClicked, this, [this](int row, int){ if(audioManager) audioManager->playClickSfx(); showProductAnalysisPopup(row); });
}

void SettlementPage::refreshPage(){ refreshSummary(); refreshSalesTable(); if(audioManager && gameManager->todayReport.profit > 0) audioManager->playCoinSfx(); }

QString SettlementPage::productIconPathByName(const QString& n) const{
    const QDir root(AssetPaths::assetRoot());
    if(n.contains("矿泉水")) return root.filePath("products/product_water.png");
    if(n.contains("罐装茶饮")) return root.filePath("products/product_canned_tea.png");
    if(n.contains("袋装面包")||n.contains("面包")) return root.filePath("products/product_bread.png");
    if(n.contains("速食泡面")) return root.filePath("products/product_noodles.png");
    if(n.contains("薯片")) return root.filePath("products/product_chips.png");
    if(n.contains("巧克力")) return root.filePath("products/product_chocolate.png");
    if(n.contains("雨伞")) return root.filePath("products/product_umbrella.png");
    if(n.contains("电池/应急灯")||n.contains("电池")) return root.filePath("products/product_battery_light.png");
    return root.filePath("ui/icons/icon_shop_bag.png");
}

QString SettlementPage::performanceText(int d,int s,int sh) const{
    if(sh>0) return "供给不足";
    if(s==0) return "滞销/未售出";
    if(s>=d && sh==0) return "表现良好";
    return "正常";
}

void SettlementPage::refreshSummary(){
    const DailyReport& r=gameManager->todayReport;
    subtitleLabel->setText(QString("第 %1 天营业复盘").arg(gameManager->store.currentDay));
    revenueCard->setText(QString("<span style=\"font-size:14px;\">营业额</span><br><span style=\"font-size:20px; font-weight:700;\">%1</span>").arg(r.revenue));
    profitCard->setText(QString("<span style=\"font-size:14px;\">利润</span><br><span style=\"font-size:20px; font-weight:700;\">%1</span>").arg(r.profit));
    customerCard->setText(QString("<span style=\"font-size:14px;\">客流量</span><br><span style=\"font-size:20px; font-weight:700;\">%1</span>").arg(r.customerCount));
    bestSellerCard->setText(QString("<span style=\"font-size:14px;\">热销商品</span><br><span style=\"font-size:15px; font-weight:700;\">%1</span>").arg(r.bestSeller.isEmpty()?"暂无":r.bestSeller));
    QString summary = r.summaryText;
    if(summary.trimmed().isEmpty()) summary = "今日经营数据已完成结算。\n建议结合热销与缺货情况，优化明日备货。";
    if(r.profit<0) summary.prepend("今日出现亏损，建议控制进货成本。\n");
    if(!r.shortageProduct.isEmpty()) summary.append("\n缺货提示："+r.shortageProduct+"，建议增加备货。");
    summaryTextEdit->setPlainText(summary);
}

void SettlementPage::refreshSalesTable(){
    const auto& sold=gameManager->todayReport.soldCounts; const auto& dem=gameManager->todayReport.demandCounts; const auto& sho=gameManager->todayReport.shortageCounts;
    QStringList names=sold.keys();
    salesTable->clearSpans();
    salesTable->setRowCount(names.size());
    if(names.isEmpty()) { salesTable->setRowCount(1); salesTable->setSpan(0,0,1,6); salesTable->setItem(0,0,new QTableWidgetItem("今日暂无商品销售数据")); return; }
    for(int i=0;i<names.size();++i){
        const QString rawName = names[i];
        QString n=rawName.trimmed(); if(n.isEmpty()){ qWarning() << "[DailyReport] empty product name at row" << i; n = "未知商品"; } int d=dem.value(rawName,0), s=sold.value(rawName,0), sh=sho.value(rawName,0);
        QTableWidgetItem* iconItem = new QTableWidgetItem();
        const QString iconPath = productIconPathByName(n);
        qDebug() << "[DailyReport] row" << i << "productName" << n << "iconPath" << iconPath;
        QPixmap icon(iconPath);
        if(icon.isNull()) {
            qWarning() << "[SettlementPage] missing product icon for" << n << "path=" << productIconPathByName(n);
            icon = QPixmap(QDir(AssetPaths::assetRoot()).filePath("ui/icons/icon_shop_bag.png"));
            if(icon.isNull()) icon = QPixmap(QDir(AssetPaths::assetRoot()).filePath("ui/icons/icon_help.png"));
        }
        if(!icon.isNull()) iconItem->setIcon(QIcon(icon.scaled(32,32,Qt::KeepAspectRatio,Qt::SmoothTransformation)));
        salesTable->setItem(i,0,iconItem);
        auto *nameItem = new QTableWidgetItem(n);
        nameItem->setForeground(QBrush(QColor(24,71,101)));
        QFont nameFont = nameItem->font();
        nameFont.setBold(true);
        nameItem->setFont(nameFont);
        salesTable->setItem(i,1,nameItem);
        auto *dItem=new QTableWidgetItem(QString::number(d)); dItem->setTextAlignment(Qt::AlignCenter);
        auto *sItem=new QTableWidgetItem(QString::number(s)); sItem->setTextAlignment(Qt::AlignCenter);
        auto *shItem=new QTableWidgetItem(QString::number(sh)); shItem->setTextAlignment(Qt::AlignCenter); if(sh>0) shItem->setForeground(QBrush(QColor(210,88,44)));
        salesTable->setItem(i,2,dItem); salesTable->setItem(i,3,sItem); salesTable->setItem(i,4,shItem);
        salesTable->setItem(i,5,new QTableWidgetItem(performanceText(d,s,sh)));
        salesTable->setRowHeight(i,48);
    }
}

void SettlementPage::showProductAnalysisPopup(int row){
    if(row<0 || row>=salesTable->rowCount() || !salesTable->item(row,1)) return;
    QString name=salesTable->item(row,1)->text();
    int demand=salesTable->item(row,2)?salesTable->item(row,2)->text().toInt():0;
    int sold=salesTable->item(row,3)?salesTable->item(row,3)->text().toInt():0;
    int shortage=salesTable->item(row,4)?salesTable->item(row,4)->text().toInt():0;

    const Product* p=nullptr; for(const Product& pp:gameManager->store.products){ if(pp.name==name){ p=&pp; break; } }
    QString category = p? p->category : "暂无";
    QString playerPrice = p? QString::number(p->playerPrice):"暂无";
    QString suggested = p? QString::number(p->suggestedPrice):"暂无";
    QString stock = p? QString::number(p->stock):"暂无";
    QString income = p? QString::number(p->playerPrice * sold):"暂无";
    double sellRate = sold / double(qMax(demand,1)); double shortRate = shortage / double(qMax(demand,1));

    QString analysis = shortage>0?"该商品今日存在缺货，说明备货不足，可能损失了部分销售机会。":(sold==0?"该商品今日几乎没有销售，可能需求较低或价格不够合适。":(sold>=demand&&shortage==0?"该商品今日表现良好，需求稳定且库存准备充分。":"该商品今日表现正常，可继续观察后续需求变化。"));
    QString advice = shortage>0?"建议明天增加进货数量。":(sold==0?"建议明天减少进货或适当降价。":(sold>=demand&&shortage==0?"建议维持当前备货和售价。":"建议保持中等备货，继续观察。"));

    QDialog dlg(this); dlg.setWindowTitle("商品分析"); dlg.resize(width()*0.58, height()*0.52);
    dlg.setStyleSheet("QDialog{background-color:#e9f6ff;} QLabel{color:#214d70;}");
    QVBoxLayout* root = new QVBoxLayout(&dlg);
    root->setContentsMargins(12,12,12,12);
    QFrame* analysisCard = new QFrame(&dlg);
    analysisCard->setObjectName("analysisCard");
    analysisCard->setStyleSheet("QFrame#analysisCard{background-color: rgba(232,246,255,235); border:1px solid rgba(145,190,220,180); border-radius:10px;}");
    QVBoxLayout* cardLayout = new QVBoxLayout(analysisCard);
    cardLayout->setContentsMargins(20,20,20,20);
    cardLayout->setSpacing(12);

    QLabel* popupTitle = new QLabel("商品分析", &dlg);
    popupTitle->setStyleSheet("font-size:20px; font-weight:700; color:#1d4d76;");
    cardLayout->addWidget(popupTitle);

    QHBoxLayout* body = new QHBoxLayout();
    body->setSpacing(16);

    QVBoxLayout* leftCol = new QVBoxLayout();
    leftCol->setSpacing(10);
    QLabel* icon = new QLabel(&dlg);
    icon->setFixedSize(128,128);
    icon->setAlignment(Qt::AlignCenter);
    QPixmap pix(productIconPathByName(name));
    if (pix.isNull()) {
        qWarning() << "[SettlementPage] missing popup icon for" << name << "path=" << productIconPathByName(name);
        pix = QPixmap(QDir(AssetPaths::assetRoot()).filePath("ui/icons/icon_shop_bag.png"));
        if (pix.isNull()) pix = QPixmap(QDir(AssetPaths::assetRoot()).filePath("ui/icons/icon_help.png"));
    }
    if (!pix.isNull()) icon->setPixmap(pix.scaled(128,128,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    leftCol->addWidget(icon,0,Qt::AlignTop|Qt::AlignHCenter);
    QLabel* nameLabel = new QLabel(name, &dlg);
    nameLabel->setStyleSheet("font-size:18px; font-weight:700;");
    QLabel* cateLabel = new QLabel(QString("类别：%1").arg(category), &dlg);
    cateLabel->setStyleSheet("font-size:15px;");
    leftCol->addWidget(nameLabel);
    leftCol->addWidget(cateLabel);
    leftCol->addStretch();
    body->addLayout(leftCol,0);

    QTextEdit* text = new QTextEdit(&dlg);
    text->setReadOnly(true);
    text->setStyleSheet("QTextEdit{background-color: rgba(232,246,255,235); border:1px solid rgba(145,190,220,180); border-radius:10px; color:#214d70; font-size:15px; padding:10px;}");
    text->setHtml(QString(
        "<div style='font-size:15px;'>"
        "<div style='font-size:16px; font-weight:700; margin-bottom:8px;'>基础信息</div>"
        "<div>今日售价：%1</div><div>建议售价：%2</div><div>当前库存：%3</div>"
        "<div style='margin-top:12px; font-size:16px; font-weight:700; margin-bottom:8px;'>销售表现</div>"
        "<div>理论需求：%4</div><div>实际售出：%5</div><div>缺货数量：%6</div><div>售出率：%7</div><div>缺货率：%8</div><div>今日收入：%9</div>"
        "<div style='margin-top:12px; font-size:16px; font-weight:700; margin-bottom:8px;'>经营分析</div>"
        "<div style='font-size:15px;'>%10</div>"
        "<div style='margin-top:12px; font-size:16px; font-weight:700; margin-bottom:8px;'>明日建议</div>"
        "<div style='font-size:15px;'>%11</div>"
        "</div>")
        .arg(playerPrice, suggested, stock,
             QString::number(demand), QString::number(sold), QString::number(shortage),
             QString::number(sellRate * 100.0, 'f', 1) + "%",
             QString::number(shortRate * 100.0, 'f', 1) + "%",
             income, analysis, advice));
    body->addWidget(text,1);

    cardLayout->addLayout(body,1);

    QHBoxLayout* footer = new QHBoxLayout();
    footer->addStretch();
    QPushButton* closeBtn = new QPushButton("关闭", &dlg);
    closeBtn->setStyleSheet("QPushButton{background-color:#2e6f9c;color:white;border-radius:8px;padding:8px 18px; font-size:16px; min-width:96px;}");
    footer->addWidget(closeBtn);
    cardLayout->addLayout(footer);
    root->addWidget(analysisCard);
    connect(closeBtn,&QPushButton::clicked,&dlg,&QDialog::accept);
    dlg.exec();
}

void SettlementPage::onUpgradeClicked(){ emit upgradeClicked(); }