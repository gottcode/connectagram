!versionAtLeast(QT_VERSION, 5.12) {
	error("Connectagram requires Qt 5.12 or greater")
}

TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = tools wordlists src
