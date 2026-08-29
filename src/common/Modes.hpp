// SPDX-FileCopyrightText: 2019 Contributors to Chatterino <https://chatterino.com>
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common/Args.hpp"

namespace chatterino {

class Modes
{
public:
    explicit Modes(const Args &args = {});

    static const Modes &instance();

    /// Marked by the line `portable` or `portableEnable` from `Args`
    bool isPortable{};

    /// Marked by the line `externally-packaged`
    ///
    /// The externally packaged mode comes with the following changes:
    ///  - No shortcuts are created by default
    bool isExternallyPackaged{};

    static void setInstance(const Modes &modes);

private:
    static const Modes *instancePtr;
};

}  // namespace chatterino
