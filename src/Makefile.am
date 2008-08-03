bin_PROGRAMS = madpdf
madpdf_SOURCES = madpdf.c settings.c opt_dlg.c

# set the include path found by configure
INCLUDES= @EWL_CFLAGS@ @EPDF_CFLAGS@

# the library search path.
madpdf_LDFLAGS = @EWL_LIBS@ @EPDF_LIBS@ -lexpat
