#!/bin/bash

APP='Connectagram'
BUNDLE="$APP.app"
VERSION='1.0.1'

cp -f share/connectagram/words "$BUNDLE/Contents/Resources"

macdeployqt $BUNDLE -dmg -no-plugins
mv "$APP.dmg" "${APP}_$VERSION.dmg"
