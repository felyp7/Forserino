// SPDX-FileCopyrightText: 2019 Contributors to Chatterino <https://chatterino.com>
//
// SPDX-License-Identifier: MIT

#include "common/Modes.hpp"

#include "util/CombinePath.hpp"

#include <QCoreApplication>

namespace chatterino {

const Modes *Modes::instancePtr = nullptr;

Modes::Modes(const Args &args)
{
    if (args.portableEnable)
    {
        this->isPortable = true;
    }

    QFile file(combinePath(QCoreApplication::applicationDirPath(), "modes"));
    if (!file.open(QIODevice::ReadOnly))
    {
        return;
    }

    while (!file.atEnd())
    {
        auto line = QString(file.readLine()).trimmed();

        if (line == "portable")
        {
            this->isPortable = true;
        }
        else if (line == "externally-packaged")
        {
            this->isExternallyPackaged = true;
        }
    }
}

const Modes &Modes::instance()
{
    if (!instancePtr)
    {
        static Modes fallbackInstance({});
        return fallbackInstance;
    }
    return *instancePtr;
}

void Modes::setInstance(const Modes &modes)
{
    instancePtr = &modes;
}

}  // namespace chatterino
