#!/bin/sh


echo -n 'Preparing files...'
cd ..

rm -f connectagram.desktop.in.bak
cp connectagram.desktop.in connectagram.desktop.in.bak
sed -e '/^Icon/ d' -e '/^Keywords/ d' -i connectagram.desktop.in

rm -f connectagram.appdata.xml.in.bak
cp connectagram.appdata.xml.in connectagram.appdata.xml.in.bak
sed '/<developer_name>/ d' -i connectagram.appdata.xml.in

cd po
echo ' DONE'


echo -n 'Extracting messages...'
xgettext --from-code=UTF-8 --output=description.pot \
	--package-name='Connectagram' --copyright-holder='Graeme Gott' \
	../*.in
sed 's/CHARSET/UTF-8/' -i description.pot
echo ' DONE'


echo -n 'Updating translations...'
rm -f LINGUAS
for POFILE in *.po;
do
	echo -n " $POFILE"
	echo -n " ${POFILE%???}" >> LINGUAS
	msgmerge --quiet --update --backup=none $POFILE description.pot
done
echo >> LINGUAS
echo ' DONE'


echo -n 'Cleaning up...'
cd ..

rm -f connectagram.desktop.in
mv connectagram.desktop.in.bak connectagram.desktop.in

rm -f connectagram.appdata.xml.in
mv connectagram.appdata.xml.in.bak connectagram.appdata.xml.in

echo ' DONE'
