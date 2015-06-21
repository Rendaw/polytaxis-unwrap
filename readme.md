# What is `polytaxis-unwrap`?

`polytaxis-unwrap` is a tool to open files with polytaxis headers in applications that don't support polytaxis.

`polytaxis-unwrap` does this by exposing a fuse filesystem that mirrors the root filesystem.  Any file opened in the fuse filesystem transparently hides the polytaxis header if present.

This is probably most useful with a path translator like `unpt` in [polytaxis-utils](https://github.com/Rendaw/polytaxis-utils) or `polytaxis-unwrap`-aware applications like `ptq` (also in polytaxis-utils) and [polytaxis-adventure](https://github.com/Rendaw/ptadventure).

### Installation

#### Linux
Installation requires fuse and tup.

After cloning the repository:
```bash
git submodule update --init
tup init
cp tup.template.config tup.config
tup upd
```

Copy `polytaxis-unwrap` to somewhere on your path.

#### Mac OS X

1. Install homebrew
2. Run
```
sudo chown -R $USER /usr/local/include
sudo chown -R $USER /usr/local/lib
brew install rendaw/tap/polytaxis-unwrap
```

### Usage

`polytaxis-unwrap` takes no command line arguments.  Start it and leave it running until you no longer need to unwrap files.

### Launching at boot

#### Linux (with systemd)

Requires a distribution that supports user systemd instances.

Create the file `~/.config/systemd/user/polytaxis-unwrap.service` with the contents:
```
[Unit]
Description=polytaxis-unwrap

[Service]
Type=simple
ExecStart=/usr/bin/polytaxis-unwrap

[Install]
WantedBy=default.target
```

Run the following to start and run `polytaxis-unwrap` at boot (with proper systemd configuration).
```
systemctl --user start polytaxis-unwrap.service
systemctl --user enable polytaxis-unwrap.service
```

#### Mac OSX (with launchd)

Create the file `~/Library/LaunchAgents/polytaxis-unwrap.plist` with the contents:
```
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>Label</key>
    <string>com.zarbosoft.polytaxis-unwrap</string>
    <key>ProgramArguments</key>
    <array>
        <string>polytaxis-unwrap</string>
    </array>
    <key>KeepAlive</key>
    <true/>
</dict>
</plist>
```

Run
```
launchctl load ~/Library/LaunchAgents/polytaxis-unwrap.plist
launchctl start ~/Library/LaunchAgents/polytaxis-unwrap.plist
```

