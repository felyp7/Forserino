#pragma once

#include "widgets/settingspages/SettingsPage.hpp"

class QLabel;
class QCheckBox;
class QComboBox;

namespace chatterino {

class GeneralPageView;
class DescriptionLabel;
struct DropdownArgs;

class ForserinoPage : public SettingsPage
{
    Q_OBJECT

public:
    ForserinoPage();

    bool filterElements(const QString &query);

private:
    void initLayout(GeneralPageView &layout);
    void initExtra();

    QString getFont(const DropdownArgs &args) const;

    DescriptionLabel *cachePath_{};
    GeneralPageView *view_{};
};

}  // namespace chatterino
