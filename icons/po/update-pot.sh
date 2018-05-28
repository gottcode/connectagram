#!/bin/sh


echo -n 'Preparing files...'
cd ..

rm -f connectagram.desktop.in
cp connectagram.desktop connectagram.desktop.in
sed -e '/^Name\[/ d' \
	-e '/^GenericName\[/ d' \
	-e '/^Comment\[/ d' \
	-e '/^Icon/ d' \
	-e '/^Keywords/ d' \
	-i connectagram.desktop.in

rm -f connectagram.appdata.xml.in
cp connectagram.appdata.xml connectagram.appdata.xml.in
sed -e '/p xml:lang/ d' \
	-e '/summary xml:lang/ d' \
	-e '/name xml:lang/ d' \
	-e '/<developer_name>/ d' \
	-i connectagram.appdata.xml.in

cd po
echo ' DONE'


echo -n 'Extracting messages...'
xgettext --from-code=UTF-8 --output=description.pot \
	--package-name='Connectagram' --copyright-holder='Graeme Gott' \
	../*.in
sed 's/CHARSET/UTF-8/' -i description.pot
echo ' DONE'


echo -n 'Cleaning up...'
cd ..

rm -f connectagram.desktop.in
rm -f connectagram.appdata.xml.in

echo ' DONE'
