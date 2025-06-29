#include "widgets/settingspages/DankerinoPage.hpp"

#include "Application.hpp"
#include "common/Channel.hpp"
#include "controllers/hotkeys/HotkeyController.hpp"
#include "controllers/notifications/NotificationController.hpp"
#include "singletons/Settings.hpp"
#include "widgets/BaseWidget.hpp"
#include "widgets/settingspages/GeneralPageView.hpp"
#include "widgets/settingspages/SettingsPage.hpp"
#include "widgets/settingspages/SettingWidget.hpp"

#include <QVBoxLayout>

namespace chatterino {

DankerinoPage::DankerinoPage()
{
    this->setLayout(new QVBoxLayout);
    auto *layout = GeneralPageView::withNavigation(this);    
    this->layout()->addWidget(layout);
    this->initLayout(*layout);
}

bool DankerinoPage::filterElements(const QString &query)
{
    if (this->view_)
        return this->view_->filterElements(query) || query.isEmpty();
    else
        return false;
}

void DankerinoPage::initLayout(GeneralPageView &layout)
{
    auto &s = *getSettings();

    layout.addTitle("Appearance");
    SettingWidget::checkbox("Show placeholder in text input box", s.showTextInputPlaceholder)
        ->addTo(layout);
    layout.addDescription(
        "The placeholder helps indicate where to type. This setting affects the message box.");

    layout.addTitle("Behavior");
    SettingWidget::checkbox("Lowercase tab-completed usernames", s.lowercaseUsernames)
        ->addTo(layout);

    layout.addTitle("Bridge user");
    SettingWidget::checkbox("Allow \"bridge\" users to impersonate others",
                            s.allowBridgeImpersonation)
        ->addTo(layout);
    SettingWidget::lineEdit("Bridge user", s.bridgeUser, "supabridge")
    ->addTo(layout);

    layout.addTitle("Rate Limits");
    SettingWidget::intInput("High rate limit spam delay (ms)", s.twitchHighRateLimitDelay,
                            {.min = 100, .max = 2000, .singleStep = 100})
        ->addTo(layout);
    SettingWidget::intInput("Low rate limit spam delay (ms)", s.twitchLowRateLimitDelay,
                            {.min = 500, .max = 3000, .singleStep = 100})
        ->addTo(layout);

    if (s.dankerinoThreeLetterApiEasterEgg)
    {
        layout.addTitle("Advanced Settings");
        SettingWidget::checkbox(
            "Click to disable GraphQL easter egg and advanced settings (requires restart)",
            s.dankerinoThreeLetterApiEasterEgg)
            ->addTo(layout);

        layout.addTitle("Random Hacks");
        SettingWidget::checkbox("Enable. Required for settings below to work!",
                                s.nonceFuckeryEnabled)
            ->addTo(layout);
        SettingWidget::checkbox("Abnormal nonce detection", s.abnormalNonceDetection)
            ->addTo(layout);
        SettingWidget::checkbox("Webchat detection highlights.", s.normalNonceDetection)
            ->addTo(layout);
        SettingWidget::colorButton("Webchat detected color", s.webchatColor)
            ->addTo(layout);
    }

    layout.addStretch();
    layout.addWidget(new BaseWidget(this));
}

}  // namespace chatterino
