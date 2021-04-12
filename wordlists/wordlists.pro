TEMPLATE = aux

LANGUAGES = ca cs de el es en fr he hu id nl pl pt pt_BR ru

updatewords.input = LANGUAGES
updatewords.output = $$PWD/../data/${QMAKE_FILE_BASE}/words
updatewords.depends = ${QMAKE_FILE_IN}
updatewords.commands = $$OUT_PWD/../wordlist -l ${QMAKE_FILE_BASE} -o ${QMAKE_FILE_OUT} ${QMAKE_FILE_IN}
updatewords.CONFIG += no_link target_predeps

QMAKE_EXTRA_COMPILERS += updatewords
