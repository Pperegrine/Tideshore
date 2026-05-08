cmake_minimum_required(VERSION 3.16)
project(Tideshore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Multimedia)

qt_standard_project_setup()

qt_add_executable(Tideshore
        main.cpp
        config/AssetPaths.h config/AssetPaths.cpp
        data/Product.h data/Product.cpp
        data/Store.h data/Store.cpp
        data/Event.h data/Event.cpp
        data/DailyReport.h data/DailyReport.cpp
        data/Employee.h data/Employee.cpp
        logic/MarketSystem.h logic/MarketSystem.cpp
        logic/EventSystem.h logic/EventSystem.cpp
        logic/CustomerSystem.h logic/CustomerSystem.cpp
        logic/SettlementSystem.h logic/SettlementSystem.cpp
        logic/UpgradeSystem.h logic/UpgradeSystem.cpp
        logic/GameManager.h logic/GameManager.cpp
        logic/SaveSystem.h logic/SaveSystem.cpp
        interaction/MainWindow.h interaction/MainWindow.cpp
        interaction/AudioManager.h interaction/AudioManager.cpp
        interaction/StartPage.h interaction/StartPage.cpp
        interaction/IntroPage.h interaction/IntroPage.cpp
        interaction/PixelImageButton.h interaction/PixelImageButton.cpp
        interaction/HintBadge.h interaction/HintBadge.cpp
        interaction/SpriteAnimator.cpp
        interaction/PreOpenPage.h interaction/PreOpenPage.cpp
        interaction/BusinessPage.h interaction/BusinessPage.cpp
        interaction/SettlementPage.h interaction/SettlementPage.cpp
        interaction/UpgradePage.h interaction/UpgradePage.cpp
        interaction/ClickEffectWidget.h interaction/ClickEffectWidget.cpp
        interaction/FloatingText.h interaction/FloatingText.cpp
)

target_link_libraries(Tideshore PRIVATE Qt6::Widgets Qt6::Gui Qt6::Multimedia)