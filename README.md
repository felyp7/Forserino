![chatterinoLogo](https://user-images.githubusercontent.com/41973452/272541622-52457e89-5f16-4c83-93e7-91866c25b606.png)
Chatterino 2 [![GitHub Actions Build (Windows, Ubuntu, MacOS)](https://github.com/Chatterino/chatterino2/actions/workflows/build.yml/badge.svg?branch=master)](https://github.com/Chatterino/chatterino2/actions?query=workflow%3ABuild+branch%3Amaster) [![Cirrus CI Build (FreeBSD only)](https://api.cirrus-ci.com/github/Chatterino/chatterino2.svg?branch=master)](https://cirrus-ci.com/github/Chatterino/chatterino2/master) [![Chocolatey Package](https://img.shields.io/chocolatey/v/chatterino?include_prereleases)](https://chocolatey.org/packages/chatterino) [![Flatpak Package](https://img.shields.io/flathub/v/com.chatterino.chatterino)](https://flathub.org/apps/details/com.chatterino.chatterino)
============
## Forserino
Forserino is a fork of Dankerino

### Features of Forserino

- `/mods` and `/vips` working in all channels. [source](https://github.com/2547techno/technorino)
- Added `/founders` and user roles in user info popup. [source](https://github.com/2547techno/technorino)
- Feature to disable 20 messages/30sec rate limit.
- Bot limits for JOINs.
- Possibility to upload any file via uploader.
- Option to enable rainbow colors for username. [based on](https://github.com/LosFarmosCTL/chatterino2)

### Screenshots

![Rainbow settings](./images_forserino/rainbow.png)

## Dankerino

Dankerino is a fork of Chatterino 2.

### Features of Dankerino

- Shortcuts for whispering Supibot `/$`.
- 7TV emotes, using the official implementation merged into [upstream](https://github.com/Chatterino/chatterino2)
- Adjust your spam speed. Useful with chats with slightly higher slow mode than the default 1s.
- Stream settings window (beta)
- A couple more appearance settings compared to upstream Chatterino 2:
 - input placeholder toggle
 - toggle for graying-out recent messages

### Screenshots

![Example of editing stream settings](./images_dankerino/example_stream_settings.png)

### Goals of Dankerino

- Add QOL and features (or fixes) that aren't accepted into the upstream repo.
- Being reasonably small

### Non-goals of Dankerino

- Being a full disconnected fork of Chatterino 2, this is just a patchset

### Downloads
You can download the latest Dankerino build over [here](https://github.com/Mm2PL/dankerino/releases/tag/nightly-build)
Windows users can install Dankerino [from Chocolatey](https://chocolatey.org/packages/dankerino).

### Discord

There is a Dankerino discord, although it's not used for much. The invite link is <https://discord.gg/Qru2TSyNZu>.

## Original Chatterino 2 readme

Chatterino 2 is a chat client for [Twitch.tv](https://twitch.tv).
The Chatterino 2 wiki can be found [here](https://wiki.chatterino.com).
Contribution guidelines can be found [here](https://wiki.chatterino.com/Contributing%20for%20Developers).

## Download

Current releases are available at [https://chatterino.com](https://chatterino.com).
Windows users can also install Chatterino [from Chocolatey](https://chocolatey.org/packages/chatterino).

## Nightly build

You can download the latest Chatterino 2 build over [here](https://github.com/Chatterino/chatterino2/releases/tag/nightly-build)

You might also need to install the [VC++ Redistributables](https://aka.ms/vs/17/release/vc_redist.x64.exe) from Microsoft if you do not have it installed already.  
If you still receive an error about `MSVCR120.dll missing`, then you should install the [VC++ 2013 Restributable](https://download.microsoft.com/download/2/E/6/2E61CFA4-993B-4DD4-91DA-3737CD5CD6E3/vcredist_x64.exe).

## Building

To get source code with required submodules run:

```shell
git clone --recurse-submodules https://github.com/Chatterino/chatterino2.git
```

or

```shell
git clone https://github.com/Chatterino/chatterino2.git
cd chatterino2
git submodule update --init --recursive
```

- [Building on Windows](../master/BUILDING_ON_WINDOWS.md)
- [Building on Windows with vcpkg](../master/BUILDING_ON_WINDOWS_WITH_VCPKG.md)
- [Building on Linux](../master/BUILDING_ON_LINUX.md)
- [Building on macOS](../master/BUILDING_ON_MAC.md)
- [Building on FreeBSD](../master/BUILDING_ON_FREEBSD.md)

## Git blame

This project has big commits in the history which touch most files while only doing stylistic changes. To improve the output of git-blame, consider setting:

```shell
git config blame.ignoreRevsFile .git-blame-ignore-revs
```

This will ignore all revisions mentioned in the [`.git-blame-ignore-revs`
file](./.git-blame-ignore-revs). GitHub does this by default.

## Code style

The code is formatted using [clang-format](https://clang.llvm.org/docs/ClangFormat.html). Our configuration is found in the [.clang-format](.clang-format) file in the repository root directory.

For more contribution guidelines, take a look at [the wiki](https://wiki.chatterino.com/Contributing%20for%20Developers/).

## Doxygen

Doxygen is used to generate project information daily and is available [here](https://doxygen.chatterino.com).
