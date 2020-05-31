TEMPLATE	= subdirs

AQP		= aqp option_parser
HYBRID_EGS	= browserwindow weathertrayicon rsspanel nyrbviewer \
		  matrixquiz
AUDIO_VIDEO_EGS = moviejingle
MODEL_VIEW_EGS	= zipcodes1 zipcodes2 timelog1 timelog2 \
		  folderview censusvisualizer tiledlistview
THREADING_EGS	= image2image numbergrid crossfader findduplicates
RICH_TEXT_EGS	= outputsampler textedit xmledit
GRAPHICS_EGS	= petridish1 pagedesigner1

SUBDIRS		= $$AQP $$HYBRID_EGS $$AUDIO_VIDEO_EGS $$MODEL_VIEW_EGS \
		  $$THREADING_EGS $$RICH_TEXT_EGS $$GRAPHICS_EGS

PHONON_EGS	= playmusic playvideo

PHONON = 
exists($$QMAKE_LIBDIR_QT/libphonon.prl) {
    PHONON = 1
}
exists($$QMAKE_LIBDIR_QT/qt4/plugins/phonon_backend/*) {
    PHONON = 1
}
isEmpty(PHONON) {
    message(Will not build the examples that need Phonon support ($$PHONON_EGS))
} else {
    SUBDIRS += $$PHONON_EGS
}

QT_46_EGS	= petridish2 finddialog pagedesigner2

IS_GE_QT_46 = $$find(QT_VERSION, "^4\\.[6-9]\\.*")
isEmpty(IS_GE_QT_46) {
    message(Will not build the examples that need Qt 4.6 or higher ($$QT_46_EGS))
} else {
    SUBDIRS += $$QT_46_EGS
}
