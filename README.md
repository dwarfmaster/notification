# Xcbnotif
This software is a simple notification displayer for X sessions. It must be
launched once at the beggining of a session and stay in the background. It will
read commands from the `/tmp/xcbnotif.fifo` fifo. Its configuration is in the
file `$HOME/.xcbnotif_config`.

## Concepts
A notification is a little window spawned on one corner of the screen. To draw
notification, you must write the right command onto the `/tmp/xcbnotif.fifo`
file. Colors and font are fixed at launch time by reading the config file. To
refer to a special type of notification (defined by colors and font), each
type is given a name (see configuration). A type is called a level.

This software won't listen for incoming notifications in dbus. If you want this
functionnality, use another software as a link, like cow-notify [1].

## Configuration
It has a tree-like configuration. A key is identified by a name and a namepath,
followed by its value is after a colon. The value must not contain spaces. A
typical line could be `path1.path2.key : value`.

### Tree
The values accepted are :
- `global` : it is the namespace where the general config is done.
  - `list` : a comma separated list of all the notification levels,
      eg `urgent,normal`.
  - `width` : the default width in pixel of a notification. It can be specified
      for each levels.
  - `gravity` : In which corner of the screen the notifications will be
      displayed. Accepted values are `top_right`, `top_left`, `bottom_right`
      and `bottom_left`.
  - `padding` : namespace containings the details about padding from the
      borders of the screen.
    - `hori` : the horizontal padding in pixels.
    - `vert` : the vertical padding in pixels.
    - `space` : The space between two notifications.
  - `gc` : the namespace for the default graphical details.
    - `bg` : the color of the background.
    - `fg` : the color of the text.
    - `bc` : the color of the border.
    - `width` : the width of the border.
    - `font` : the font used for the text. It must be the pattern for an X core
               font eg `-misc-fixed-*`. If no font is specified, the default
               one is used : `-*-terminal-medium-r-*-*-14-*-*-*-*-*-iso8859-*`.
- `//mode//` : the namespace to configure a special level. It must have been
    first declared in `global.list`. The values set here override the default
    ones setted in `global`.
  - `gc` : level-specific graphic namespace. It contains accepts the same
      entries as `global.gc`. If it contains an invalid valid, the program will
      not create the level.
  - `width` : Same as `global.width`, but for a specific level.

### Colors
The colors can be written using three syntaxes :
- `#x` : where x is in [0-9a-f]. It is a shade of grey.
- `#rgb` : the rgb conponents, each in [0-9a-f].
- `#rrggbb` : the rgb conponents, each in [0-9a-f], but with more precision.

## Commands
There are five commands accepted :
- `notif` : creates a new notification. It must have three arguments. The first
    one is an integer stating the time in seconds the notification must stay on
    the screen. The second one is the level of the notification. Finally, the
    third one is the text of the notification. If the level does not exists,
    the notifiation will not be printed.
- `close` : close the newest notification.
- `close_all` : close all the notifications.
- `end` : close all the notifications and stops the server.
- `kill` : same as `end`.

[1] https://github.com/Cloudef/cow-notify

