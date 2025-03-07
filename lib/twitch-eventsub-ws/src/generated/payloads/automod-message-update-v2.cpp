// WARNING: This file is automatically generated. Any changes will be lost.
#include "twitch-eventsub-ws/chrono.hpp"  // IWYU pragma: keep
#include "twitch-eventsub-ws/detail/errors.hpp"
#include "twitch-eventsub-ws/detail/variant.hpp"  // IWYU pragma: keep
#include "twitch-eventsub-ws/payloads/automod-message-update-v2.hpp"

#include <boost/json.hpp>

namespace chatterino::eventsub::lib::payload::automod_message_update::v2 {

boost::json::result_for<Event, boost::json::value>::type tag_invoke(
    boost::json::try_value_to_tag<Event> /* tag */,
    const boost::json::value &jvRoot)
{
    if (!jvRoot.is_object())
    {
        EVENTSUB_BAIL_HERE(error::Kind::ExpectedObject);
    }
    const auto &root = jvRoot.get_object();

    const auto *jvbroadcasterUserID = root.if_contains("broadcaster_user_id");
    if (jvbroadcasterUserID == nullptr)
    {
        EVENTSUB_BAIL_HERE(error::Kind::FieldMissing);
    }

    auto broadcasterUserID =
        boost::json::try_value_to<String>(*jvbroadcasterUserID);

    if (broadcasterUserID.has_error())
    {
        return broadcasterUserID.error();
    }

    const auto *jvbroadcasterUserLogin =
        root.if_contains("broadcaster_user_login");
    if (jvbroadcasterUserLogin == nullptr)
    {
        EVENTSUB_BAIL_HERE(error::Kind::FieldMissing);
    }

    auto broadcasterUserLogin =
        boost::json::try_value_to<String>(*jvbroadcasterUserLogin);

    if (broadcasterUserLogin.has_error())
    {
        return broadcasterUserLogin.error();
    }

    const auto *jvbroadcasterUserName =
        root.if_contains("broadcaster_user_name");
    if (jvbroadcasterUserName == nullptr)
    {
        EVENTSUB_BAIL_HERE(error::Kind::FieldMissing);
    }

    auto broadcasterUserName =
        boost::json::try_value_to<String>(*jvbroadcasterUserName);

    if (broadcasterUserName.has_error())
    {
        return broadcasterUserName.error();
    }

    const auto *jvuserID = root.if_contains("user_id");
    if (jvuserID == nullptr)
    {
        EVENTSUB_BAIL_HERE(error::Kind::FieldMissing);
    }

    auto userID = boost::json::try_value_to<std::string>(*jvuserID);

    if (userID.has_error())
    {
        return userID.error();
    }

    const auto *jvuserLogin = root.if_contains("user_login");
    if (jvuserLogin == nullptr)
    {
        EVENTSUB_BAIL_HERE(error::Kind::FieldMissing);
    }

    auto userLogin = boost::json::try_value_to<std::string>(*jvuserLogin);

    if (userLogin.has_error())
    {
        return userLogin.error();
    }

    const auto *jvuserName = root.if_contains("user_name");
    if (jvuserName == nullptr)
    {
        EVENTSUB_BAIL_HERE(error::Kind::FieldMissing);
    }

    auto userName = boost::json::try_value_to<std::string>(*jvuserName);

    if (userName.has_error())
    {
        return userName.error();
    }

    const auto *jvmoderatorUserID = root.if_contains("moderator_user_id");
    if (jvmoderatorUserID == nullptr)
    {
        EVENTSUB_BAIL_HERE(error::Kind::FieldMissing);
    }

    auto moderatorUserID =
        boost::json::try_value_to<std::string>(*jvmoderatorUserID);

    if (moderatorUserID.has_error())
    {
        return moderatorUserID.error();
    }

    const auto *jvmoderatorUserLogin = root.if_contains("moderator_user_login");
    if (jvmoderatorUserLogin == nullptr)
    {
        EVENTSUB_BAIL_HERE(error::Kind::FieldMissing);
    }

    auto moderatorUserLogin =
        boost::json::try_value_to<std::string>(*jvmoderatorUserLogin);

    if (moderatorUserLogin.has_error())
    {
        return moderatorUserLogin.error();
    }

    const auto *jvmoderatorUserName = root.if_contains("moderator_user_name");
    if (jvmoderatorUserName == nullptr)
    {
        EVENTSUB_BAIL_HERE(error::Kind::FieldMissing);
    }

    auto moderatorUserName =
        boost::json::try_value_to<std::string>(*jvmoderatorUserName);

    if (moderatorUserName.has_error())
    {
        return moderatorUserName.error();
    }

    const auto *jvmessageID = root.if_contains("message_id");
    if (jvmessageID == nullptr)
    {
        EVENTSUB_BAIL_HERE(error::Kind::FieldMissing);
    }

    auto messageID = boost::json::try_value_to<String>(*jvmessageID);

    if (messageID.has_error())
    {
        return messageID.error();
    }

    const auto *jvmessage = root.if_contains("message");
    if (jvmessage == nullptr)
    {
        EVENTSUB_BAIL_HERE(error::Kind::FieldMissing);
    }

    auto message = boost::json::try_value_to<chat::Message>(*jvmessage);

    if (message.has_error())
    {
        return message.error();
    }

    const auto *jvstatus = root.if_contains("status");
    if (jvstatus == nullptr)
    {
        EVENTSUB_BAIL_HERE(error::Kind::FieldMissing);
    }

    auto status = boost::json::try_value_to<std::string>(*jvstatus);

    if (status.has_error())
    {
        return status.error();
    }

    const auto *jvheldAt = root.if_contains("held_at");
    if (jvheldAt == nullptr)
    {
        EVENTSUB_BAIL_HERE(error::Kind::FieldMissing);
    }

    auto heldAt = boost::json::try_value_to<std::string>(*jvheldAt);

    if (heldAt.has_error())
    {
        return heldAt.error();
    }

    const auto *jvreasonTag = root.if_contains("reason");
    if (jvreasonTag == nullptr)
    {
        EVENTSUB_BAIL_HERE(error::Kind::FieldMissing);
    }

    auto reasonTagRes =
        boost::json::try_value_to<boost::json::string>(*jvreasonTag);
    if (reasonTagRes.has_error())
    {
        return reasonTagRes.error();
    }
    std::string_view reasonTag = *reasonTagRes;
    decltype(std::declval<Event>().reason) reason;
    if (reasonTag == automod::AutomodReason::TAG)
    {
        const auto *reasonVal =
            root.if_contains(detail::fieldFor<automod::AutomodReason>());
        if (!reasonVal)
        {
            EVENTSUB_BAIL_HERE(error::Kind::FieldMissing);
        }
        auto reasonautomodAutomodReason =
            boost::json::try_value_to<automod::AutomodReason>(*reasonVal);
        if (reasonautomodAutomodReason.has_error())
        {
            return reasonautomodAutomodReason.error();
        }
        reason.emplace<automod::AutomodReason>(
            std::move(reasonautomodAutomodReason.value()));
    }
    else if (reasonTag == automod::BlockedTermReason::TAG)
    {
        const auto *reasonVal =
            root.if_contains(detail::fieldFor<automod::BlockedTermReason>());
        if (!reasonVal)
        {
            EVENTSUB_BAIL_HERE(error::Kind::FieldMissing);
        }
        auto reasonautomodBlockedTermReason =
            boost::json::try_value_to<automod::BlockedTermReason>(*reasonVal);
        if (reasonautomodBlockedTermReason.has_error())
        {
            return reasonautomodBlockedTermReason.error();
        }
        reason.emplace<automod::BlockedTermReason>(
            std::move(reasonautomodBlockedTermReason.value()));
    }
    else
    {
        EVENTSUB_BAIL_HERE(error::Kind::UnknownVariant);
    }

    return Event{
        .broadcasterUserID = std::move(broadcasterUserID.value()),
        .broadcasterUserLogin = std::move(broadcasterUserLogin.value()),
        .broadcasterUserName = std::move(broadcasterUserName.value()),
        .userID = std::move(userID.value()),
        .userLogin = std::move(userLogin.value()),
        .userName = std::move(userName.value()),
        .moderatorUserID = std::move(moderatorUserID.value()),
        .moderatorUserLogin = std::move(moderatorUserLogin.value()),
        .moderatorUserName = std::move(moderatorUserName.value()),
        .messageID = std::move(messageID.value()),
        .message = std::move(message.value()),
        .status = std::move(status.value()),
        .heldAt = std::move(heldAt.value()),
        .reason = std::move(reason),
    };
}

boost::json::result_for<Payload, boost::json::value>::type tag_invoke(
    boost::json::try_value_to_tag<Payload> /* tag */,
    const boost::json::value &jvRoot)
{
    if (!jvRoot.is_object())
    {
        EVENTSUB_BAIL_HERE(error::Kind::ExpectedObject);
    }
    const auto &root = jvRoot.get_object();

    const auto *jvsubscription = root.if_contains("subscription");
    if (jvsubscription == nullptr)
    {
        EVENTSUB_BAIL_HERE(error::Kind::FieldMissing);
    }

    auto subscription =
        boost::json::try_value_to<subscription::Subscription>(*jvsubscription);

    if (subscription.has_error())
    {
        return subscription.error();
    }

    const auto *jvevent = root.if_contains("event");
    if (jvevent == nullptr)
    {
        EVENTSUB_BAIL_HERE(error::Kind::FieldMissing);
    }

    auto event = boost::json::try_value_to<Event>(*jvevent);

    if (event.has_error())
    {
        return event.error();
    }

    return Payload{
        .subscription = std::move(subscription.value()),
        .event = std::move(event.value()),
    };
}

}  // namespace chatterino::eventsub::lib::payload::automod_message_update::v2
