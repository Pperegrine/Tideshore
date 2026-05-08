#include "AssetPaths.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

QString AssetPaths::assetRoot()
{
    const QString appDir = QCoreApplication::applicationDirPath();
    const QString cwd = QDir::currentPath();

    const QStringList candidates = {
        QDir(appDir).filePath("../assets"),
        QDir(appDir).filePath("assets"),
        QDir(cwd).filePath("assets"),
        QDir(cwd).filePath("../assets")
    };

    for (const QString& candidate : candidates) {
        QFileInfo info(candidate);
        if (info.exists() && info.isDir()) {
            return info.absoluteFilePath();
        }
    }

    qWarning() << "[Assets] assets folder not found, fallback to:" << candidates.first();
    return QFileInfo(candidates.first()).absoluteFilePath();
}

QString AssetPaths::filePath(const QString& relativePath)
{
    return QDir(assetRoot()).filePath(relativePath);
}

bool AssetPaths::exists(const QString& path)
{
    return QFileInfo::exists(path);
}

#define RET(p) QString AssetPaths::p(){ return filePath(

RET(townStreetDay) "backgrounds/town_street_day.png"); }
RET(shopExteriorDay) "backgrounds/shop_exterior_day.png"); }
RET(shopInteriorBasic) "backgrounds/shop_interior_basic.png"); }

RET(employeeCashier) "characters/employee_cashier.png"); }
RET(employeePlanner) "characters/employee_planner.png"); }
RET(employeeOperator) "characters/employee_operator.png"); }
RET(employeeManagerGuide) "characters/employee_manager_guide.png"); }
RET(employeeStockClerk) "characters/employee_stock_clerk.png"); }

RET(productWater) "products/product_water.png"); }
RET(productBread) "products/product_bread.png"); }
RET(productChocolate) "products/product_chocolate.png"); }
RET(productChips) "products/product_chips.png"); }
RET(productCannedTea) "products/product_canned_tea.png"); }
RET(productNoodles) "products/product_noodles.png"); }
RET(productUmbrella) "products/product_umbrella.png"); }
RET(productBatteryLight) "products/product_battery_light.png"); }

RET(eventHeavyRain) "events/event_heavy_rain.png"); }
RET(eventHotWeather) "events/event_hot_weather.png"); }
RET(eventStockpileFood) "events/event_stockpile_food.png"); }
RET(eventStudentFlow) "events/event_student_flow.png"); }

RET(logoTideshore) "ui/logo/logo_tideshore.png"); }

RET(iconPlay) "ui/icons/icon_play.png"); }
RET(iconSettings) "ui/icons/icon_settings.png"); }
RET(iconSave) "ui/icons/icon_save.png"); }
RET(iconClose) "ui/icons/icon_close.png"); }
RET(iconConfirm) "ui/icons/icon_confirm.png"); }
RET(iconArrowLeft) "ui/icons/icon_arrow_left.png"); }
RET(iconArrowRight) "ui/icons/icon_arrow_right.png"); }
RET(iconCoin) "ui/icons/icon_coin.png"); }
RET(iconStar) "ui/elements/icon_star.png"); }
RET(iconCalendar) "ui/icons/icon_calendar.png"); }
RET(iconAlert) "ui/icons/icon_alert.png"); }
RET(iconSound) "ui/icons/icon_sound.png"); }
RET(iconHelp) "ui/icons/icon_help.png"); }
RET(iconShopBag) "ui/icons/icon_shop_bag.png"); }
RET(iconCleanBroom) "ui/icons/icon_clean_broom.png"); }
RET(iconDeliveryBoxes) "ui/icons/icon_delivery_boxes.png"); }
RET(iconNoticeBoard) "ui/icons/icon_notice_board.png"); }
RET(iconHomeNeedsCleanup) "ui/icons/icon_home_needs_cleanup.png"); }
RET(iconTeamCustomers) "ui/icons/icon_team_customers.png"); }

RET(panelWoodText) "ui/panels/panel_wood_text.png"); }
RET(panelSquareBlueGold) "ui/panels/ui_panel_square_blue_gold.png"); }
RET(panelSquareCard) "ui/panels/ui_panel_square_card.png"); }
RET(btnBlueStatesStrip) "ui/buttons/btn_blue_states_strip.png"); }
RET(btnWoodStatesStrip) "ui/buttons/btn_wood_states_strip.png"); }
RET(btnBlueNormal) "ui/buttons/btn_blue_normal.png"); }
RET(btnBlueHover) "ui/buttons/btn_blue_hover.png"); }
RET(btnBluePressed) "ui/buttons/btn_blue_pressed.png"); }
RET(btnWoodNormal) "ui/buttons/btn_wood_normal.png"); }
RET(btnWoodHover) "ui/buttons/btn_wood_hover.png"); }
RET(btnWoodPressed) "ui/buttons/btn_wood_pressed.png"); }

RET(uiInventorySlot) "ui/elements/ui_inventory_slot.png"); }
RET(uiLabelTagBlank) "ui/elements/ui_label_tag_blank.png"); }
RET(uiProgressVertical) "ui/elements/ui_progress_vertical.png"); }
RET(uiProgressVerticalSimple) "ui/elements/ui_progress_vertical_simple.png"); }

RET(zpixFont) "fonts/zpix.ttf"); }

void AssetPaths::debugPrintMissingAssets()
{
    const QStringList paths = {
        townStreetDay(), shopExteriorDay(), shopInteriorBasic(),
        employeeCashier(), employeePlanner(), employeeOperator(), employeeManagerGuide(), employeeStockClerk(),
        productWater(), productBread(), productChocolate(), productChips(), productCannedTea(), productNoodles(), productUmbrella(), productBatteryLight(),
        eventHeavyRain(), eventHotWeather(), eventStockpileFood(), eventStudentFlow(),
        logoTideshore(),
        iconPlay(), iconSettings(), iconSave(), iconClose(), iconConfirm(), iconArrowLeft(), iconArrowRight(),
        iconCoin(), iconStar(), iconCalendar(), iconAlert(), iconSound(), iconHelp(), iconShopBag(), iconCleanBroom(),
        iconDeliveryBoxes(), iconNoticeBoard(), iconHomeNeedsCleanup(), iconTeamCustomers(),
        panelWoodText(), panelSquareBlueGold(), panelSquareCard(), btnBlueStatesStrip(), btnWoodStatesStrip(),
        btnBlueNormal(), btnBlueHover(), btnBluePressed(), btnWoodNormal(), btnWoodHover(), btnWoodPressed(),
        uiInventorySlot(), uiLabelTagBlank(), uiProgressVertical(), uiProgressVerticalSimple(),
        filePath("ui/panels/ui_panel_section_large.png"),
filePath("ui/panels/ui_popup_analysis_card.png"),
filePath("ui/panels/ui_hint_bottom_strip.png"),
filePath("ui/panels/ui_hud_top_strip.png"),
filePath("ui/panels/ui_popup_product_detail_card.png"),
filePath("ui/panels/ui_report_main_panel.png"),
filePath("ui/panels/ui_popup_detail_card.png"),
filePath("ui/panels/ui_status_message_strip.png"),
filePath("ui/panels/ui_popup_goods_prepare_panel.png"),
        zpixFont()
    };

    bool allFound = true;
    for (const QString& path : paths) {
        if (!exists(path)) {
            qWarning() << "[Assets] Missing:" << path;
            allFound = false;
        }
    }

    if (allFound) {
        qDebug() << "[Assets] All registered assets found.";
    }
}