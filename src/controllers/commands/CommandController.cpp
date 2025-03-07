#include "controllers/commands/CommandController.hpp"

#include "Application.hpp"
#include "common/Channel.hpp"
#include "controllers/accounts/AccountController.hpp"
#include "controllers/commands/builtin/chatterino/Debugging.hpp"
#include "controllers/commands/builtin/Misc.hpp"
#include "controllers/commands/builtin/twitch/AddModerator.hpp"
#include "controllers/commands/builtin/twitch/AddVIP.hpp"
#include "controllers/commands/builtin/twitch/Announce.hpp"
#include "controllers/commands/builtin/twitch/Ban.hpp"
#include "controllers/commands/builtin/twitch/Block.hpp"
#include "controllers/commands/builtin/twitch/ChatSettings.hpp"
#include "controllers/commands/builtin/twitch/Chatters.hpp"
#include "controllers/commands/builtin/twitch/DeleteMessages.hpp"
#include "controllers/commands/builtin/twitch/GetModerators.hpp"
#include "controllers/commands/builtin/twitch/GetVIPs.hpp"
#include "controllers/commands/builtin/twitch/Raid.hpp"
#include "controllers/commands/builtin/twitch/RemoveModerator.hpp"
#include "controllers/commands/builtin/twitch/RemoveVIP.hpp"
#include "controllers/commands/builtin/twitch/SendReply.hpp"
#include "controllers/commands/builtin/twitch/SendWhisper.hpp"
#include "controllers/commands/builtin/twitch/ShieldMode.hpp"
#include "controllers/commands/builtin/twitch/Shoutout.hpp"
#include "controllers/commands/builtin/twitch/StartCommercial.hpp"
#include "controllers/commands/builtin/twitch/Unban.hpp"
#include "controllers/commands/builtin/twitch/UpdateChannel.hpp"
#include "controllers/commands/builtin/twitch/UpdateColor.hpp"
#include "controllers/commands/builtin/twitch/Warn.hpp"
#include "controllers/commands/Command.hpp"
#include "controllers/commands/CommandContext.hpp"
#include "controllers/commands/CommandModel.hpp"
#include "controllers/plugins/PluginController.hpp"
#include "messages/Message.hpp"
#include "messages/MessageBuilder.hpp"
#include "providers/IvrApi.hpp"
#include "providers/twitch/api/Helix.hpp"
#include "providers/twitch/TwitchAccount.hpp"
#include "providers/twitch/TwitchChannel.hpp"
#include "providers/twitch/TwitchCommon.hpp"
#include "singletons/Emotes.hpp"
#include "singletons/Paths.hpp"
#include "util/CombinePath.hpp"
#include "util/QStringHash.hpp"
#include "common/Env.hpp"
#include "common/LinkParser.hpp"
#include "common/QLogging.hpp"
#include "common/SignalVector.hpp"
#include "controllers/userdata/UserDataController.hpp"
#include "messages/Image.hpp"
#include "messages/MessageElement.hpp"
#include "messages/MessageThread.hpp"
#include "providers/twitch/TwitchIrcServer.hpp"
#include "singletons/Settings.hpp"
#include "singletons/Theme.hpp"
#include "singletons/WindowManager.hpp"
#include "util/Clipboard.hpp"
#include "util/FormatTime.hpp"
#include "util/Helpers.hpp"
#include "util/IncognitoBrowser.hpp"
#include "util/PostToThread.hpp"
#include "util/StreamLink.hpp"
#include "util/Twitch.hpp"
#include "widgets/dialogs/ReplyThreadPopup.hpp"
#include "widgets/dialogs/UserInfoPopup.hpp"
#include "widgets/helper/ChannelView.hpp"
#include "widgets/splits/Split.hpp"
#include "widgets/splits/SplitContainer.hpp"
#include "widgets/Window.hpp"

#include <QApplication>
#include <QDesktopServices>
#include <QFile>
#include <QRegularExpression>
#include <QUrl>
#include <unordered_map>

namespace {

using namespace chatterino;

bool areIRCCommandsStillAvailable()
{
    // 11th of February 2023, 06:00am UTC
    const QDateTime migrationTime(QDate(2023, 2, 11), QTime(6, 0), Qt::UTC);
    auto now = QDateTime::currentDateTimeUtc();
    return now < migrationTime;
}

QString useIRCCommand(const QStringList &words)
{
    // Reform the original command
    auto originalCommand = words.join(" ");

    // Replace the / with a . to pass it along to TMI
    auto newCommand = originalCommand;
    newCommand.replace(0, 1, ".");

    qCDebug(chatterinoTwitch)
        << "Forwarding command" << originalCommand << "as" << newCommand;

    return newCommand;
}

using VariableReplacer = std::function<QString(
    const QString &, const ChannelPtr &, const Message *)>;

const VariableReplacer NO_OP_PLACEHOLDER =
    [](const auto &altText, const auto &channel, const auto *message) {
        return altText;
    };

const std::unordered_map<QString, VariableReplacer> COMMAND_VARS{
    {
        "channel.name",
        [](const auto &altText, const auto &channel, const auto *message) {
            (void)(altText);  //unused
            (void)(message);  //unused
            return channel->getName();
        },
    },
    {
        "channel.id",
        [](const auto &altText, const auto &channel, const auto *message) {
            (void)(message);  //unused
            auto *tc = dynamic_cast<TwitchChannel *>(channel.get());
            if (tc == nullptr)
            {
                return altText;
            }

            return tc->roomId();
        },
    },
    {
        // NOTE: The use of {channel} is deprecated and support for it will drop at some point
        // Users should be encouraged to use {channel.name} instead.
        "channel",
        [](const auto &altText, const auto &channel, const auto *message) {
            (void)(altText);  //unused
            (void)(message);  //unused
            return channel->getName();
        },
    },
    {
        "stream.game",
        [](const auto &altText, const auto &channel, const auto *message) {
            (void)(message);  //unused
            auto *tc = dynamic_cast<TwitchChannel *>(channel.get());
            if (tc == nullptr)
            {
                return altText;
            }
            const auto &status = tc->accessStreamStatus();
            return status->live ? status->game : altText;
        },
    },
    {
        "stream.title",
        [](const auto &altText, const auto &channel, const auto *message) {
            (void)(message);  //unused
            auto *tc = dynamic_cast<TwitchChannel *>(channel.get());
            if (tc == nullptr)
            {
                return altText;
            }
            const auto &status = tc->accessStreamStatus();
            return status->live ? status->title : altText;
        },
    },
    {
        "my.id",
        [](const auto &altText, const auto &channel, const auto *message) {
            (void)(channel);  //unused
            (void)(message);  //unused
            auto uid =
                getApp()->getAccounts()->twitch.getCurrent()->getUserId();
            return uid.isEmpty() ? altText : uid;
        },
    },
    {
        "my.name",
        [](const auto &altText, const auto &channel, const auto *message) {
            (void)(channel);  //unused
            (void)(message);  //unused
            auto name =
                getApp()->getAccounts()->twitch.getCurrent()->getUserName();
            return name.isEmpty() ? altText : name;
        },
    },
    {
        "user.name",
        [](const auto &altText, const auto &channel, const auto *message) {
            (void)(channel);  //unused
            if (message == nullptr)
            {
                return altText;
            }

            const auto &v = message->loginName;

            if (v.isEmpty())
            {
                return altText;
            }

            return v;
        },
    },
    {
        // NOTE: The use of {user} is deprecated and support for it will drop at some point
        // Users should be encouraged to use {user.name} instead.
        "user",
        [](const auto &altText, const auto &channel, const auto *message) {
            (void)(channel);  //unused
            if (message == nullptr)
            {
                return altText;
            }

            const auto &v = message->loginName;

            if (v.isEmpty())
            {
                return altText;
            }

            return v;
        },
    },
    {
        "msg.id",
        [](const auto &altText, const auto &channel, const auto *message) {
            (void)(channel);  //unused
            if (message == nullptr)
            {
                return altText;
            }

            const auto &v = message->id;

            if (v.isEmpty())
            {
                return altText;
            }

            return v;
        },
    },
    {
        // NOTE: The use of {msg-id} is deprecated and support for it will drop at some point
        // Users should be encouraged to use {msg.id} instead.
        "msg-id",
        [](const auto &altText, const auto &channel, const auto *message) {
            (void)(channel);  //unused
            if (message == nullptr)
            {
                return altText;
            }

            const auto &v = message->id;

            if (v.isEmpty())
            {
                return altText;
            }

            return v;
        },
    },
    {
        "msg.text",
        [](const auto &altText, const auto &channel, const auto *message) {
            (void)(channel);  //unused
            if (message == nullptr)
            {
                return altText;
            }

            const auto &v = message->messageText;

            if (v.isEmpty())
            {
                return altText;
            }

            return v;
        },
    },
    {
        // NOTE: The use of {message} is deprecated and support for it will drop at some point
        // Users should be encouraged to use {msg.text} instead.
        "message",
        [](const auto &altText, const auto &channel, const auto *message) {
            (void)(channel);  //unused
            if (message == nullptr)
            {
                return altText;
            }

            const auto &v = message->messageText;

            if (v.isEmpty())
            {
                return altText;
            }

            return v;
        },
    },
    // variables used in mod buttons and the like, these make no sense in normal commands, so they are left empty
    {"input.text", NO_OP_PLACEHOLDER},
};

}  // namespace

namespace chatterino {

CommandController::CommandController(const Paths &paths)
{
    // Update commands map when the vector of commands has been updated
    auto addFirstMatchToMap = [this](auto args) {
        this->userCommands_.remove(args.item.name);

        for (const Command &cmd : this->items)
        {
            if (cmd.name == args.item.name)
            {
                this->userCommands_[cmd.name] = cmd;
                break;
            }
        }

        int maxSpaces = 0;

        for (const Command &cmd : this->items)
        {
            auto localMaxSpaces = cmd.name.count(' ');
            if (localMaxSpaces > maxSpaces)
            {
                maxSpaces = localMaxSpaces;
            }
        }

        this->maxSpaces_ = maxSpaces;
    };
    // We can safely ignore these signal connections since items will be destroyed
    // before CommandController
    std::ignore = this->items.itemInserted.connect(addFirstMatchToMap);
    std::ignore = this->items.itemRemoved.connect(addFirstMatchToMap);

    // Initialize setting manager for commands.json
    auto path = combinePath(paths.settingsDirectory, "commands.json");
    this->sm_ = std::make_shared<pajlada::Settings::SettingManager>();
    this->sm_->saveMethod =
        pajlada::Settings::SettingManager::SaveMethod::OnlySaveIfChanged;
    this->sm_->setPath(qPrintable(path));
    this->sm_->setBackupEnabled(true);
    this->sm_->setBackupSlots(9);

    // Delayed initialization of the setting storing all commands
    this->commandsSetting_.reset(
        new pajlada::Settings::Setting<std::vector<Command>>(
            "/commands", pajlada::Settings::SettingOption::CompareBeforeSet,
            this->sm_));

    // Update the setting when the vector of commands has been updated (most
    // likely from the settings dialog)
    std::ignore = this->items.delayedItemsChanged.connect([this] {
        this->commandsSetting_->setValue(this->items.raw());
    });

    // Load commands from commands.json
    this->sm_->load();

    // Add loaded commands to our vector of commands (which will update the map
    // of commands)
    for (const auto &command : this->commandsSetting_->getValue())
    {
        this->items.append(command);
    }

    /// Deprecated commands

    this->registerCommand("/ignore", &commands::ignoreUser);

    this->registerCommand("/unignore", &commands::unignoreUser);

    this->registerCommand("/follow", &commands::follow);

    this->registerCommand("/unfollow", &commands::unfollow);

    /// Supported commands

    this->registerCommand("/debug-args", &commands::listArgs);

    this->registerCommand("/debug-env", &commands::listEnvironmentVariables);

    this->registerCommand("/uptime", &commands::uptime);

    this->registerCommand("/block", &commands::blockUser);

    this->registerCommand("/unblock", &commands::unblockUser);

    this->registerCommand("/user", &commands::user);

    this->registerCommand("/usercard", &commands::openUsercard);

    this->registerCommand("/requests", &commands::requests);

    this->registerCommand("/lowtrust", &commands::lowtrust);

    this->registerCommand("/chatters", &commands::chatters);

    this->registerCommand("/test-chatters", &commands::testChatters);

    auto formatModsError = [](HelixGetModeratorsError error, QString message) {
        using Error = HelixGetModeratorsError;

        QString errorMessage = QString("Failed to get moderators - ");

        switch (error)
        {
            case Error::Forwarded: {
                errorMessage += message;
            }
            break;

            case Error::UserMissingScope: {
                errorMessage += "Missing required scope. "
                                "Re-login with your "
                                "account and try again.";
            }
            break;

            case Error::UserNotAuthorized: {
                errorMessage +=
                    "Due to Twitch restrictions, "
                    "this command can only be used by the broadcaster. "
                    "To see the list of mods you must use the Twitch website.";
            }
            break;

            case Error::Unknown: {
                errorMessage += "An unknown error has occurred.";
            }
            break;
        }
        return errorMessage;
    };

    this->registerCommand("/mods", &commands::getModerators);

    this->registerCommand("/clip", &commands::clip);

    this->registerCommand("/marker", &commands::marker);

    this->registerCommand("/streamlink", &commands::streamlink);

    this->registerCommand("/popout", &commands::popout);

    this->registerCommand("/popup", &commands::popup);

    this->registerCommand("/clearmessages", &commands::clearmessages);

    this->registerCommand("/settitle", &commands::setTitle);

    this->registerCommand("/setgame", &commands::setGame);

    this->registerCommand("/openurl", &commands::openURL);

    this->registerCommand("/raw", &commands::sendRawMessage);

    this->registerCommand("/reply", &commands::sendReply);

#ifndef NDEBUG
    this->registerCommand("/fakemsg", &commands::injectFakeMessage);
    this->registerCommand("/debug-update-to-no-stream",
                          &commands::injectStreamUpdateNoStream);
#endif

    this->registerCommand("/copy", &commands::copyToClipboard);

    this->registerCommand("/color", &commands::updateUserColor);

    this->registerCommand("/clear", &commands::deleteAllMessages);

    this->registerCommand("/delete", &commands::deleteOneMessage);

    this->registerCommand("/mod", &commands::addModerator);

    this->registerCommand("/unmod", &commands::removeModerator);

    this->registerCommand("/announce", &commands::sendAnnouncement);
    this->registerCommand("/announceblue", &commands::sendAnnouncementBlue);
    this->registerCommand("/announcegreen", &commands::sendAnnouncementGreen);
    this->registerCommand("/announceorange", &commands::sendAnnouncementOrange);
    this->registerCommand("/announcepurple", &commands::sendAnnouncementPurple);

    this->registerCommand("/vip", &commands::addVIP);

    this->registerCommand("/unvip", &commands::removeVIP);

    this->registerCommand("/unban", &commands::unbanUser);
    this->registerCommand("/untimeout", &commands::unbanUser);

    this->registerCommand("/raid", &commands::startRaid);

    this->registerCommand("/unraid", &commands::cancelRaid);

    this->registerCommand("/emoteonly", &commands::emoteOnly);
    this->registerCommand("/emoteonlyoff", &commands::emoteOnlyOff);

    this->registerCommand("/subscribers", &commands::subscribers);
    this->registerCommand("/subscribersoff", &commands::subscribersOff);

    this->registerCommand("/slow", &commands::slow);
    this->registerCommand("/slowoff", &commands::slowOff);

    this->registerCommand("/followers", &commands::followers);
    this->registerCommand("/followersoff", &commands::followersOff);
    this->registerCommand(
        "/founders", [](const QStringList &words, auto channel) -> QString {
            auto twitchChannel = dynamic_cast<TwitchChannel *>(channel.get());
            QString target(words.value(1).toLower());

            if (twitchChannel == nullptr)
            {
                channel->addMessage(makeSystemMessage(
                    "The /founders command only works in Twitch Channels"), MessageContext::Original);
                return "";
            }

            if (words.value(1).isEmpty())
            {
                target = channel->getName();
            }

            getIvr()->getFounders(
                target,
                [channel, twitchChannel, target](auto result) {
                    std::vector<HelixModerator> founders;

                    for (int i = 0; i < result.size(); i++)
                    {
                        QJsonObject founderJson;

                        founderJson.insert("user_id",
                                           result.at(i).toObject().value("id"));
                        founderJson.insert(
                            "user_name",
                            result.at(i).toObject().value("displayName"));
                        founderJson.insert(
                            "user_login",
                            result.at(i).toObject().value("login"));

                        HelixModerator founder(founderJson);
                        founders.push_back(founder);
                    }

                    channel->addMessage(MessageBuilder::makeListOfUsersMessage(
                                    QString("The founders of %1 are").arg(target), founders,
                                    twitchChannel),
                                    MessageContext::Original);

                },
                [channel]() {
                    channel->addMessage(
                        makeSystemMessage("Could not get founders list!"), MessageContext::Original);
                });
            return "";
        });
    this->registerCommand("/uniquechat", &commands::uniqueChat);
    this->registerCommand("/r9kbeta", &commands::uniqueChat);
    this->registerCommand("/uniquechatoff", &commands::uniqueChatOff);
    this->registerCommand("/r9kbetaoff", &commands::uniqueChatOff);

    this->registerCommand("/timeout", &commands::sendTimeout);

    this->registerCommand("/ban", &commands::sendBan);
    this->registerCommand("/banid", &commands::sendBanById);

    this->registerCommand("/warn", &commands::sendWarn);

    for (const auto &cmd : TWITCH_WHISPER_COMMANDS)
    {
        this->registerCommand(cmd, &commands::sendWhisper);
    }

auto formatVIPListError = [](HelixListVIPsError error,
                                 const QString &message) -> QString {
        using Error = HelixListVIPsError;

        QString errorMessage = QString("Failed to list VIPs - ");

        switch (error)
        {
            case Error::Forwarded: {
                errorMessage += message;
            }
            break;

            case Error::Ratelimited: {
                errorMessage += "You are being ratelimited by Twitch. Try "
                                "again in a few seconds.";
            }
            break;

            case Error::UserMissingScope: {
                // TODO(pajlada): Phrase MISSING_REQUIRED_SCOPE
                errorMessage += "Missing required scope. "
                                "Re-login with your "
                                "account and try again.";
            }
            break;

            case Error::UserNotAuthorized: {
                // TODO(pajlada): Phrase MISSING_PERMISSION
                errorMessage += "You don't have permission to "
                                "perform that action.";
            }
            break;

            case Error::UserNotBroadcaster: {
                errorMessage +=
                    "Due to Twitch restrictions, "
                    "this command can only be used by the broadcaster. "
                    "To see the list of VIPs you must use the Twitch website.";
            }
            break;

            case Error::Unknown: {
                errorMessage += "An unknown error has occurred.";
            }
            break;
        }
        return errorMessage;
    };

    this->registerCommand("/vips", &commands::getVIPs);

    this->registerCommand("/commercial", &commands::startCommercial);

    this->registerCommand("/unstable-set-user-color",
                          &commands::unstableSetUserClientSideColor);

    this->registerCommand("/debug-force-image-gc",
                          &commands::forceImageGarbageCollection);

    this->registerCommand("/debug-force-image-unload",
                          &commands::forceImageUnload);

    this->registerCommand("/debug-test", &commands::debugTest);

    this->registerCommand("/shield", &commands::shieldModeOn);
    this->registerCommand("/shieldoff", &commands::shieldModeOff);

    this->registerCommand("/shoutout", &commands::sendShoutout);

    this->registerCommand("/c2-set-logging-rules", &commands::setLoggingRules);
    this->registerCommand("/c2-theme-autoreload", &commands::toggleThemeReload);
}

void CommandController::save()
{
    this->sm_->save();
}

CommandModel *CommandController::createModel(QObject *parent)
{
    CommandModel *model = new CommandModel(parent);
    model->initialize(&this->items);

    return model;
}

QString CommandController::execCommand(const QString &textNoEmoji,
                                       ChannelPtr channel, bool dryRun)
{
    QString text =
        getApp()->getEmotes()->getEmojis()->replaceShortCodes(textNoEmoji);
    QStringList words = text.split(' ', Qt::SkipEmptyParts);

    if (words.length() == 0)
    {
        return text;
    }

    QString commandName = words[0];

    {
        // check if user command exists
        const auto it = this->userCommands_.find(commandName);
        if (it != this->userCommands_.end())
        {
            text = getApp()->getEmotes()->getEmojis()->replaceShortCodes(
                this->execCustomCommand(words, it.value(), dryRun, channel));

            words = text.split(' ', Qt::SkipEmptyParts);

            if (words.length() == 0)
            {
                return text;
            }

            commandName = words[0];
        }
    }

    if (!dryRun)
    {
        // check if command exists
        const auto it = this->commands_.find(commandName);
        if (it != this->commands_.end())
        {
            if (auto *command = std::get_if<CommandFunction>(&it->second))
            {
                return (*command)(words, channel);
            }
            if (auto *command =
                    std::get_if<CommandFunctionWithContext>(&it->second))
            {
                CommandContext ctx{
                    words,
                    channel,
                    dynamic_cast<TwitchChannel *>(channel.get()),
                };
                return (*command)(ctx);
            }

            return "";
        }
    }

    // We have checks to ensure words cannot be empty, so this can never wrap around
    auto maxSpaces = std::min(this->maxSpaces_, (qsizetype)words.length() - 1);
    for (int i = 0; i < maxSpaces; ++i)
    {
        commandName += ' ' + words[i + 1];

        const auto it = this->userCommands_.find(commandName);
        if (it != this->userCommands_.end())
        {
            return this->execCustomCommand(words, it.value(), dryRun, channel);
        }
    }

    if (!dryRun && channel->getType() == Channel::Type::TwitchWhispers)
    {
        channel->addSystemMessage("Use /w <username> <message> to whisper");
        return "";
    }

    return text;
}

#ifdef CHATTERINO_HAVE_PLUGINS
bool CommandController::registerPluginCommand(const QString &commandName)
{
    if (this->commands_.contains(commandName))
    {
        return false;
    }

    this->commands_[commandName] = [commandName](const CommandContext &ctx) {
        return getApp()->getPlugins()->tryExecPluginCommand(commandName, ctx);
    };
    this->pluginCommands_.append(commandName);
    return true;
}

bool CommandController::unregisterPluginCommand(const QString &commandName)
{
    if (!this->pluginCommands_.contains(commandName))
    {
        return false;
    }
    this->pluginCommands_.removeAll(commandName);
    return this->commands_.erase(commandName) != 0;
}
#endif

void CommandController::registerCommand(const QString &commandName,
                                        CommandFunctionVariants commandFunction)
{
    assert(this->commands_.count(commandName) == 0);

    this->commands_[commandName] = std::move(commandFunction);

    this->defaultChatterinoCommandAutoCompletions_.append(commandName);
}

QString CommandController::execCustomCommand(
    const QStringList &words, const Command &command, bool /* dryRun */,
    ChannelPtr channel, const Message *message,
    std::unordered_map<QString, QString> context)
{
    QString result;

    static QRegularExpression parseCommand(
        R"((^|[^{])({{)*{(\d+\+?|([a-zA-Z.-]+)(?:;(.+?))?)})");

    int lastCaptureEnd = 0;

    auto globalMatch = parseCommand.globalMatch(command.func);
    int matchOffset = 0;

    while (true)
    {
        QRegularExpressionMatch match =
            parseCommand.match(command.func, matchOffset);

        if (!match.hasMatch())
        {
            break;
        }

        result += command.func.mid(lastCaptureEnd,
                                   match.capturedStart() - lastCaptureEnd + 1);

        lastCaptureEnd = match.capturedEnd();
        matchOffset = lastCaptureEnd - 1;

        QString wordIndexMatch = match.captured(3);

        bool plus = wordIndexMatch.at(wordIndexMatch.size() - 1) == '+';
        wordIndexMatch = wordIndexMatch.replace("+", "");

        bool ok;
        int wordIndex = wordIndexMatch.replace("=", "").toInt(&ok);
        if (!ok || wordIndex == 0)
        {
            auto varName = match.captured(4);
            auto altText = match.captured(5);  // alt text or empty string

            auto var = context.find(varName);

            if (var != context.end())
            {
                // Found variable in `context`
                result += var->second.isEmpty() ? altText : var->second;
                continue;
            }

            auto it = COMMAND_VARS.find(varName);
            if (it != COMMAND_VARS.end())
            {
                // Found variable in `COMMAND_VARS`
                result += it->second(altText, channel, message);
                continue;
            }

            // Fall back to replacing it with the actual matched string
            result += "{" + match.captured(3) + "}";
            continue;
        }

        if (words.length() <= wordIndex)
        {
            continue;
        }

        if (plus)
        {
            bool first = true;
            for (int i = wordIndex; i < words.length(); i++)
            {
                if (!first)
                {
                    result += " ";
                }
                result += words[i];
                first = false;
            }
        }
        else
        {
            result += words[wordIndex];
        }
    }

    result += command.func.mid(lastCaptureEnd);

    if (result.size() > 0 && result.at(0) == '{')
    {
        result = result.mid(1);
    }

    return result.replace("{{", "{");
}

QStringList CommandController::getDefaultChatterinoCommandList()
{
    return this->defaultChatterinoCommandAutoCompletions_;
}

}  // namespace chatterino
