export PATH=/home/kevin/.local/bin:$PATH

# LD_LIBRARY_PATH only needed if you are building without rpath
# export LD_LIBRARY_PATH=/home/kevin/.local/lib:$LD_LIBRARY_PATH

export XDG_DATA_DIRS=/home/kevin/.local/share:${XDG_DATA_DIRS:-/usr/local/share:/usr/share}
export XDG_CONFIG_DIRS=/home/kevin/.local/etc/xdg:${XDG_CONFIG_DIRS:-/etc/xdg}

export QT_PLUGIN_PATH=/usr/lib/qt6/plugins:$QT_PLUGIN_PATH
export QML2_IMPORT_PATH=/usr/lib/qt6/qml:$QML2_IMPORT_PATH

export QT_QUICK_CONTROLS_STYLE_PATH=/usr/lib/qt6/qml/QtQuick/Controls.2/:$QT_QUICK_CONTROLS_STYLE_PATH

export MANPATH=/home/kevin/.local/share/man:${MANPATH:-/usr/local/share/man:/usr/share/man}

export SASL_PATH=/home/kevin/.local/lib/sasl2:${SASL_PATH:-/usr/lib/sasl2}

export PYTHONPATH="/home/kevin/.local/lib/python3.14/site-packages":$PYTHONPATH
