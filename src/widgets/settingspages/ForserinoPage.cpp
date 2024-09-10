#include "ForserinoPage.hpp"

#include "Application.hpp"
#include "common/Version.hpp"
#include "singletons/Settings.hpp"
#include "widgets/BaseWindow.hpp"
#include "widgets/helper/Line.hpp"
#include "widgets/settingspages/GeneralPageView.hpp"

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
    auto view = new GeneralPageView;
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
     // First checkbox: Main setting
    QCheckBox *rainbowMessagesCheckBox = layout.addCheckbox(
        "Change color to create a rainbow effect before sending each message",
        s.rainbowMessages);

    // Second checkbox: Subsetting, initially disabled
    QCheckBox *rainbowMethodCheckBox = layout.addCheckbox(
        "Make colors change after sending a message",  
        s.rainbowMethod, false, 
        "By default, colors change before the message is sent (for slower internet, Enabled is better)");

    // Disable the second checkbox initially if the first one is not checked
    rainbowMethodCheckBox->setEnabled(s.rainbowMessages.getValue());

    QObject::connect(rainbowMessagesCheckBox, &QCheckBox::stateChanged, 
    [rainbowMethodCheckBox](int state) {
        rainbowMethodCheckBox->setEnabled(state == Qt::Checked);
    });
    
    layout.addCheckbox(
        "Use true rainbow colors (requires Twitch Prime or Turbo)",
        s.rainbowMessagesPrime);
    layout.addIntInput("Rainbow speed (HSL hue increase per new color)",
                       s.rainbowSpeed, 1, 100, 1);
    layout.addIntInput("Rainbow starting color (HSL hue)", s.rainbowStartingHue,
                       0, 359, 1);
    layout.addIntInput("Rainbow saturation (default is 153)", s.rainbowSaturation, 0, 255, 1);
    layout.addIntInput("Rainbow light (default is 128)", s.rainbowLight, 0, 255, 1);
    auto *groupLayout = new QFormLayout();
    auto *lineEdit = this->createLineEdit(s.rainbowChannels);
        groupLayout->addRow(
            this->createCheckBox("Allow rainbow to only work in certain channels",
                                 s.allowRainbowChannels));
        lineEdit->setPlaceholderText("forsen");
        groupLayout->addRow("Channels where to use rainbow: ", lineEdit);
        layout.addLayout(groupLayout);
    auto *groupLayout2 = new QFormLayout();
    auto *lineEdit2 = this->createLineEdit(s.defaultColor);
    groupLayout->addRow(
            this->createCheckBox("Enable default color",
                                 s.enableDefaultColor));
    lineEdit2->setPlaceholderText("#000000");
    groupLayout2->addRow("Default color: ", lineEdit2);
    layout.addLayout(groupLayout2);

    layout.addTitle("Miscellaneous");
    layout.addCheckbox(
        "Disable 20 messages/30sec rate limit (enables bot limits)",
        s.ignoreMaxMessageRateLimit);
    layout.addCheckbox("Use bot limits for JOINs", s.useBotLimitsJoin);
    layout.addCheckbox("Show banned reason in usercard", s.showBannedReason);
    layout.addCheckbox("Show user roles in usercard", s.showUserRoles);
    layout.addCheckbox("Show user bio in usercard", s.showUserBio);

    layout.addStretch();
    // invisible element for width
    auto inv = new BaseWidget(this);
    //    inv->setScaleIndependantWidth(600);
    layout.addWidget(inv);
}

}  // namespace chatterino
