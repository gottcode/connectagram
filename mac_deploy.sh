#!/bin/bash

APP='Connectagram'
BUNDLE="$APP.app"
VERSION=$(git rev-parse --short HEAD)

cp -f share/connectagram/words "$BUNDLE/Contents/Resources"

macdeployqt $BUNDLE -dmg -no-plugins
mv "$APP.dmg" "${APP}_$VERSION.dmg"
