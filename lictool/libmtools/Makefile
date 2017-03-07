MTOOL_SRCS := buffer.c charsetConv.c codepages.c config.c copyfile.c \
	devices.c dirCache.c directory.c direntry.c expand.c fat.c fat_free.c \
	file.c file_name.c filter.c \
	floppyd_io.c force_io.c hash.c init.c llong.c mainloop.c match.c mattrib.c \
	mbadblocks.c mcat.c mcd.c mclasserase.c mcopy.c mdel.c mdir.c \
	mdoctorfat.c mdu.c mformat.c minfo.c misc.c missFuncs.c mk_direntry.c \
	mlabel.c mmd.c mmount.c mmove.c mpartition.c mshortname.c \
	mshowfat.c mtools.c mzip.c patchlevel.c plain_io.c precmd.c privileges.c \
	scsi.c signal.c stream.c streamcache.c subdir.c tty.c unixdir.c \
	vfat.c xdf_io.c
MTOOL_OBJS := $(MTOOL_SRCS:.c=.o)

%.o: %.c
	gcc -c -I. -DHAVE_CONFIG_H -DSYSCONFDIR=\"/usr/local/etc\" -DCPU_x86_64 -DVENDOR_unknown -DOS_linux_gnu -g -O2 -Wall -fno-strict-aliasing -c -I/usr/include $<

libmtools.a: $(MTOOL_OBJS)
	ar rc $@ $(MTOOL_OBJS)

clean:
	rm $(MTOOL_OBJS)
