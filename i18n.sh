#! /usr/bin/env sh
# SPDX-FileCopyrightText: 2026 QuickBar contributors
#
# SPDX-License-Identifier: GPL-2.0-or-later
#

# USAGE:
#
#     ./i18n.sh update <template|all|[locale]>
#
#   Update the template/everything/a specific locale
#
#     ./i18n.sh new [locale]
#
#   Generate a new locale (copies the template to a new folder with the locale name)
#
# NOTE: run this from the root of the repo, or it might break :)


if [ "$1" == "update" ]; then

  # functions for doing stuff

  update_template() {
    # create it if it doesn't exist (it should exist...)
    touch po/plasma_applet_org.quickbar.globalmenu.pot

    # extract from C++ source and merge into the pot
    xgettext --from-code=UTF-8 \
      -k_ -ki18n:1 -ki18nc:1c,2 -ki18np:1,2 -ki18ncp:1c,2,3 \
      -o po/plasma_applet_org.quickbar.globalmenu.pot \
      --join-existing \
      src/*.cpp src/*.h

    # also extract from QML, also merge into the pot
    xgettext --from-code=UTF-8 \
      -L JavaScript \
      -k_ -ki18n:1 -ki18nc:1c,2 -ki18np:1,2 -ki18ncp:1c,2,3 \
      -o po/plasma_applet_org.quickbar.globalmenu.pot \
      --join-existing \
      qml/*.qml
  }

  update_locale() {
    # update the locale po file from the latest pot
    msgmerge --update "po/${1}/plasma_applet_org.quickbar.globalmenu.po" \
      po/plasma_applet_org.quickbar.globalmenu.pot
  }
  
  if [ "$2" == "template" ]; then
    update_template

  elif [ "$2" == "all" ]; then
    update_template
    for l in po/*/; do
      update_locale "$(basename "$l")"
    done

  else
    if [ ! -d "po/${2}" ]; then
      echo "Locale ${2} does not exist!"
      exit 2
    fi
    update_locale "$2"
  fi
    

elif [ "$1" == "new" ]; then

  if [ -d "po/${2}" ]; then
    echo "Locale ${2} already exists!"
    exit 3
  fi

  mkdir -p "po/${2}"
  cp "po/plasma_applet_org.quickbar.globalmenu.pot" "po/${2}/plasma_applet_org.quickbar.globalmenu.po"

else  # syntax error
  echo "No verb provided!"
  exit 1
fi

