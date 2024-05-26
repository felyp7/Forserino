#include "controllers/commands/builtin/twitch/GetModerators.hpp"

#include "common/Channel.hpp"
#include "controllers/commands/CommandContext.hpp"
#include "messages/MessageBuilder.hpp"
#include "providers/IvrApi.hpp"
#include "providers/twitch/api/Helix.hpp"
#include "providers/twitch/TwitchChannel.hpp"
#include "providers/twitch/TwitchMessageBuilder.hpp"

namespace {

using namespace chatterino;

QString formatModsError(HelixGetModeratorsError error, const QString &message)
{
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
}

}  // namespace

namespace chatterino::commands {

QString getModerators(const CommandContext &ctx)
{
    if (ctx.channel == nullptr)
    {
        return "";
    }

    if (ctx.twitchChannel == nullptr)
    {
        ctx.channel->addMessage(makeSystemMessage(
            "The /mods command only works in Twitch Channels."));
        return "";
    }

    if (ctx.twitchChannel->isBroadcaster())
    {
        getHelix()->getModerators(
            ctx.twitchChannel->roomId(), 500,
            [channel{ctx.channel},
             twitchChannel{ctx.twitchChannel}](auto result) {
                if (result.empty())
                {
                    channel->addMessage(makeSystemMessage(
                        "This channel does not have any moderators."));
                    return;
                }
                // TODO: sort results?

                MessageBuilder builder;
                TwitchMessageBuilder::listOfUsersSystemMessage(
                    "The moderators of this channel are", result, twitchChannel,
                    &builder);
                channel->addMessage(builder.release());
            },
            [channel{ctx.channel}, formatModsError{formatModsError}](
                auto error, auto message) {
                auto errorMessage = formatModsError(error, message);
                channel->addMessage(makeSystemMessage(errorMessage));
            });
    }
    else
    {
        QString target = ctx.channel->getName();
        getIvr()->getMods2807Tools(
            target,
            [channel{ctx.channel}, twitchChannel{ctx.twitchChannel},
             target](auto result) {
                if (result.isEmpty())
                {
                    channel->addMessage(makeSystemMessage(
                        "This channel does not have any moderators."));
                    return;
                }

                std::vector<HelixModerator> mods;
                for (int i = 0; i < result.size(); i++)
                {
                    QJsonObject modJson;

                    modJson.insert("user_id",
                                   result.at(i).toObject().value("id"));
                    modJson.insert("user_name", result.at(i).toObject().value(
                                                    "displayName"));
                    modJson.insert("user_login",
                                   result.at(i).toObject().value("login"));

                    HelixModerator moderator(modJson);

                    mods.push_back(moderator);
                }

                MessageBuilder builder;
                TwitchMessageBuilder::listOfUsersSystemMessage(
                    "The moderators of this channel are", mods, twitchChannel,
                    &builder);
                channel->addMessage(builder.release());
            },
            [channel{ctx.channel}]() {
                channel->addMessage(
                    makeSystemMessage("Could not get moderators list!"));
            });
    }
    return "";
}

}  // namespace chatterino::commands
