#include "widgets/settingspages/ForserinoPage.hpp"

#include "Application.hpp"
#include "common/Version.hpp"
#include "singletons/Settings.hpp"
#include "widgets/BaseWindow.hpp"
#include "widgets/helper/Line.hpp"
#include "widgets/settingspages/GeneralPageView.hpp"
#include "widgets/settingspages/SettingWidget.hpp"

#include <QDesktopServices>
#include <QFileDialog>
#include <QFontDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QScrollArea>

namespace chatterino {

ForserinoPage::ForserinoPage()
{
    auto y = new QVBoxLayout;
    auto x = new QHBoxLayout;
    auto *view = GeneralPageView::withNavigation(this);
    this->view_ = view;
    x->addWidget(view);
    auto z = new QFrame;
    z->setLayout(x);
    y->addWidget(z);
    this->setLayout(y);

    this->initLayout(*view);
}

bool ForserinoPage::filterElements(const QString &query)
{
    if (this->view_)
        return this->view_->filterElements(query) || query.isEmpty();
    else
        return false;
}

void ForserinoPage::initLayout(GeneralPageView &layout)
{
    auto &s = *getSettings();

    layout.addTitle("Rainbow username colors");

    // Main checkbox
    auto *rainbowMessages = SettingWidget::checkbox(
        "Change color to create a rainbow effect before sending each message",
        s.rainbowMessages);
    rainbowMessages->addTo(layout);

    // Dependent checkbox (no longer conditionally enabled)
    auto *rainbowMethod = SettingWidget::checkbox(
        "Make colors change after sending a message",
        s.rainbowMethod);
    rainbowMethod->addTo(layout);

    SettingWidget::checkbox("Use true rainbow colors (requires Twitch Prime or Turbo)",
                            s.rainbowMessagesPrime)
        ->addTo(layout);
    SettingWidget::intInput("Rainbow speed (HSL hue increase per new color)",
                            s.rainbowSpeed, {1, 100, 1})
        ->addTo(layout);
    SettingWidget::intInput("Rainbow starting color (HSL hue)",
                            s.rainbowStartingHue, {0, 359, 1})
        ->addTo(layout);
    SettingWidget::intInput("Rainbow saturation (default is 153)",
                            s.rainbowSaturation, {0, 255, 1})
        ->addTo(layout);
    SettingWidget::intInput("Rainbow light (default is 128)",
                            s.rainbowLight, {0, 255, 1})
        ->addTo(layout);

    // Channel whitelist group
    auto *channelLayout = new QFormLayout;
    SettingWidget::checkbox("Allow rainbow to only work in certain channels",
                            s.allowRainbowChannels)
        ->addTo(layout, channelLayout);
    SettingWidget::lineEdit("Channels where to use rainbow",
                            s.rainbowChannels, "forsen")
        ->addTo(layout, channelLayout);
    layout.addLayout(channelLayout);

    // Default color group
    auto *colorLayout = new QFormLayout;
    SettingWidget::checkbox("Enable default color",
                            s.enableDefaultColor)
        ->addTo(layout, colorLayout);
    SettingWidget::lineEdit("Default color",
                            s.defaultColor, "#000000")
        ->addTo(layout, colorLayout);
    layout.addLayout(colorLayout);

    layout.addTitle("Miscellaneous");
    SettingWidget::checkbox("Disable 20 messages/30sec rate limit (enables bot limits)",
                            s.ignoreMaxMessageRateLimit)
        ->addTo(layout);
    SettingWidget::checkbox("Use bot limits for JOINs",
                            s.useBotLimitsJoin)
        ->addTo(layout);
    SettingWidget::checkbox("Show banned reason in usercard",
                            s.showBannedReason)
        ->addTo(layout);
    SettingWidget::checkbox("Show user roles in usercard",
                            s.showUserRoles)
        ->addTo(layout);
    SettingWidget::checkbox("Show user bio in usercard",
                            s.showUserBio)
        ->addTo(layout);

    layout.addStretch();
    layout.addWidget(new BaseWidget(this));
}

}  // namespace chatterino
