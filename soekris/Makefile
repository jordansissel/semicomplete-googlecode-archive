ROOT_COOKIE=.done/mtree.root
USR_COOKIE=.done/mtree.usr
VAR_COOKIE=.done/mtree.var
DESTDIR?=$(PWD)/base

#CFLAGS+=-g

# Specify -DCOPYMFSROOT to automatically copy mfsroot to /tftpboot
.if !defined(COPYMFSROOT)
COPYMFSROOT=0
.endif

# specify -DTITLES to enable xterm titles for status updates
.if defined(TITLES)
TITLES=yes
.else
TITLES=yes
.endif

.if defined(DOCLEAN)
DOCLEAN=clean
.else
DOCLEAN="-DPLACEHOLDER"
.endif

KERNCONF?=SOEKRIS

# Hacks...
# - libcrypto doesn't install properly if DESTDIR/usr/include/openssl isn't a directory
# - libpam fails installing when DESTDIR/usr/include/security isn't a directory
# - libm is actually in /usr/src/lib/msun, so I kludge libm detections to msun
#
# lib/libc
#    libc/yp/Makefile.inc has RPCSRC using DESTDIR, this breaks. Remove $(DESTDIR) to fix
#    libc/rpc/Makefile.inc has RPCSRC using DESTDIR, this breaks. Remove $(DESTDIR) to fix
#
# secure/lib/libcrypto
#    for some reason buildinf.h isn't done properly, need to run 'make buildinf.h' first

# Disable building of things we don't need (games, docs, etc)
MAKEOPTS+=-DNOGAMES -DNOMAN -DNOINFO -DNOPROFILE -DNOSHARE
MAKEOPTS+=-DNO_SENDMAIL -DNO_BIND -DNO_BIND_ETC -DNO_I4B 
MAKEOPTS+=-DNO_KERBEROS -DNO_BIND_MTREE -DNO_LPR

.if defined(DEBUG)
MAKEOPTS+=DEBUG_FLAGS=-g
.endif

BIN_FILES=cat chmod cp df echo hostname kill ln ls mkdir mv ps pwd \
			 rm rmdir sh sync test date dd stty expr

SBIN_FILES=adjkerntz devfs dhclient dmesg ifconfig init fsck \
			  mdconfig mount mount_nfs reboot shutdown rcorder sysctl \
			  umount swapon 

USRBIN_FILES=basename awk false killall less passwd pkill \
				 su touch true sed cmp find logger uname mktemp login
				 
USRSBIN_FILES=chown inetd pw pwd_mkdb syslogd
SECURE_FILES=usr.bin/ssh usr.bin/ssh-keygen usr.bin/scp usr.sbin/sshd
LIBEXEC_FILES=rtld-elf save-entropy getty
LIB_FILES=libopie libypclnt

# Careful, termcap is 3 megs... maybe we don't need it that baddly
SHARE_FILES=termcap

# Specify -DNOTOOLS to not build these programs below
.if !defined(NOTOOLS)
BIN_FILES+=csh 
SBIN_FILES+=ping route
USRBIN_FILES+=host netstat top w
USRSBIN_FILES+=traceroute
.endif

.if defined(DEBUG)
GNU_FILES+=usr.bin/gdb
.endif


UNNEEDED_FILES=/usr/bin/omshell /usr/bin/more
UNNEEDED_ETC=X11 bluetooth gnats mail mtree periodic

all: clean world  

world: prebuild build findlibs install-etc patch-etc finish-and-cleanup mfsroot.bin
kernel: kernel.gz
kernel.gz: build-kernel install-kernel

# These files are chflag'd schg. Unflag them so we can clean up...
SCHG_FILES=usr/bin/login usr/bin/passwd usr/bin/yppasswd usr/bin/su bin/rcp \
			  var/empty sbin/init lib/libc.so.5 lib/libcrypt.so.2 libexec/ld-elf.so.1 \
			  usr/lib/libpthread.so.1
clean:
	@for path in $(SCHG_FILES); do \
		chflags noschg $(DESTDIR)/$${path} || true; \
	done
	rm -rf .done base || true

.done:
	[ -d .done ] || mkdir .done

base: .done
	[ -d $(DESTDIR) ] || mkdir $(DESTDIR)
	-umount /dev/md0c $(DESTDIR)
	-mdconfig -d -u 0 || true
	dd if=/dev/zero of=mfsroot bs=1k count=20480
	mdconfig -a -t vnode -f mfsroot -u 0
	disklabel -rw /dev/md0
	newfs -o space -m 0 -U /dev/md0c
	mount /dev/md0c $(DESTDIR)
	-[ ! -f $(ROOT_COOKIE) ] && \
		mtree -U -p $(DESTDIR)/ < /etc/mtree/BSD.root.dist && touch $(ROOT_COOKIE)
	-[ ! -f $(USR_COOKIE) ] && \
		mtree -U -p $(DESTDIR)/usr < /etc/mtree/BSD.usr.dist && touch $(USR_COOKIE)
	-[ ! -f $(VAR_COOKIE) ] && \
		mtree -U -p $(DESTDIR)/var < /etc/mtree/BSD.var.dist && touch $(VAR_COOKIE)

prebuild: base 

.PATH: $(DESTDIR)
build_all: 
	[ -f .done/$(FILEPATH) ] || \
	for PROG in $(DIRS); do \
		echo "---> $(DESTDIR)/$(FILEPATH)/$${PROG}"; \
		if [ -f $(DESTDIR)/$(FILEPATH)/$${PROG} ]; then \
			echo "*** Existing $(FILEPATH)/$${PROG} found. Skipping..."; \
		else \
			echo "*** Installing $${PROG} / $(FILEPATH)/$${PROG}"; \
			([ "$(TITLES)" = "yes" ] && echo -n "]0;$(FILEPATH)/$${PROG}"); \
			echo make -C /usr/src/$(FILEPATH)/$${PROG} $(DOCLEAN) all install DESTDIR=$(DESTDIR) $(MAKEOPTS); \
			make -C /usr/src/$(FILEPATH)/$${PROG} $(DOCLEAN) all install DESTDIR=$(DESTDIR) $(MAKEOPTS); \
		fi; \
	done;
	#touch .done/$(FILEPATH)

build: base
	make build_all DIRS="$(BIN_FILES)" DESTDIR=$(DESTDIR) FILEPATH="bin" $(MAKEOPTS)
	make build_all DIRS="$(SBIN_FILES)" DESTDIR=$(DESTDIR) FILEPATH="sbin" $(MAKEOPTS)
	make build_all DIRS="$(USRBIN_FILES)" DESTDIR=$(DESTDIR) FILEPATH="usr.bin" $(MAKEOPTS)
	make build_all DIRS="$(USRSBIN_FILES)" DESTDIR=$(DESTDIR) FILEPATH="usr.sbin" $(MAKEOPTS)
	make build_all DIRS="$(LIBEXEC_FILES)" DESTDIR=$(DESTDIR) FILEPATH="libexec" $(MAKEOPTS)
	make build_all DIRS="$(SECURE_FILES)" DESTDIR=$(DESTDIR) FILEPATH="secure" $(MAKEOPTS)
	make build_all DIRS="$(LIB_FILES)" DESTDIR=$(DESTDIR) FILEPATH="lib" $(MAKEOPTS)

#make build_all DIRS="$(SHARE_FILES)" DESTDIR=$(DESTDIR) FILEPATH="share" $(MAKEOPTS)
#make build_all DIRS="$(GNU_FILES)" DESTDIR=$(DESTDIR) FILEPATH="gnu" $(MAKEOPTS)

findlibs: base
	cd $(DESTDIR); \
	LIBS=`ldd -f "%o\n" bin/* sbin/* usr/bin/* usr/sbin/* 2> /dev/null | sort | uniq | sed 's/\..*$$//'`; \
	for i in $${LIBS}; do \
		for d in /usr/src/lib /usr/src/secure/lib /usr/src/gnu/lib; do \
			[ $$i = "libm" ] && i=msun; \
			if [ -d $$d/$$i ]; then \
				EXTRATARGET=""; \
				[ $$i = "libcrypto" ] && [ ! -d $(DESTDIR)/usr/include/openssl ] && mkdir $(DESTDIR)/usr/include/openssl; \
				[ $$i = "libpam" ] && [ ! -d $(DESTDIR)/usr/include/security ] && mkdir $(DESTDIR)/usr/include/security; \
				[ $$i = "libreadline" ] && [ ! -d $(DESTDIR)/usr/include/readline ] && mkdir $(DESTDIR)/usr/include/readline; \
				[ $$i = "libcrypto" ] && EXTRATARGET="buildinf.h"; \
				echo "*** Installing $$i from $$d"; \
				([ "$(TITLES)" = "yes" ] && echo -n "]0; $$i / $$d "); \
				echo "*** Installing $$i from $$d"; \
				echo make -C $$d/$$i $(DOCLEAN) $${EXTRATARGET} all install DESTDIR=$(DESTDIR) $(MAKEOPTS); \
				make -C $$d/$$i $(DOCLEAN) $${EXTRATARGET} all install DESTDIR=$(DESTDIR) $(MAKEOPTS); \
				break; \
			fi; \
		done; \
		find $(DESTDIR) -name '*.a' -print0 | xargs -n 100 -0 rm; \
	done; \
	echo "The following libraries were needed:"; \
  	echo "$$LIBS"

install-etc: base
	make -C /usr/src/etc distribution DESTDIR=$(DESTDIR) $(MAKEOPTS)

finish-and-cleanup: base
	[ "$(DESTDIR)" = "" ] && exit 1 || true
	rm -rf $(DESTDIR)/usr/include || true
	rm -rf $(DESTDIR)/usr/share/ || true
	(cd $(DESTDIR); rm -f $(UNNEEDED_FILES))
	find $(DESTDIR) -name '*.a' -print0 | xargs -n 100 -0 rm
	
patch-etc: base
	@echo no-op

mount:
	@[ -f .done/mounted ] && echo "$(DESTDIR) already mounted" || \
	( \
	mdconfig -du0 2> /dev/null || true; \
	umount $(DESTDIR) 2> /dev/null || true; \
	mdconfig -a -t vnode -f mfsroot -u 0; \
	mount /dev/md0c $(DESTDIR); \
	mount -t procfs - $(DESTDIR)/proc; \
	mount -t devfs - $(DESTDIR)/dev; \
	df $(DESTDIR) $(DESTDIR)/proc $(DESTDIR)/dev; \
	touch .done/mounted )

#@[ ! -f .done/mounted ] && echo "$(DESTDIR) isn't mounted" || \
umount: unmount
forceunmount:
	touch .done/mounted
	make unmount

unmount:
	@[ ! -f .done/mounted ] && echo "$(DESTDIR) isn't mounted" || \
	( \
	umount $(DESTDIR)/proc 2> /dev/null || true; \
	umount $(DESTDIR)/dev 2> /dev/null || true; \
	umount $(DESTDIR); \
	mdconfig -du0; \
	[ -f .done/mounted ] && rm .done/mounted ) 

test:
	make mount
	-chroot $(DESTDIR) /bin/sh
	make unmount

jail:
	make mount
	-ifconfig em0 alias 192.168.1.131 netmask 0xffffffff
	jail $(DESTDIR) 192.168.1.131 192.168.1.131 /bin/sh
	ifconfig em0 -alias 192.168.1.131 netmask 0xffffffff
	make unmount

fixetc:
	if [ ! -f .done/$(.TARGET) ]; then \
		set -x; \
		for i in etc/; do \
			cp $$i $(DESTDIR)/etc; \
		done;  \
		mount -t procfs - $(DESTDIR)/proc; \
		mount -t devfs - $(DESTDIR)/dev; \
		chroot $(DESTDIR) pw useradd psionic; \
		chroot $(DESTDIR) pwd_mkdb /etc/master.passwd; \
		chroot $(DESTDIR) /bin/sh -c "/etc/rc.d/preseedrandom; /etc/rc.d/sshd keygen"; \
		umount $(DESTDIR)/proc; \
		umount $(DESTDIR)/dev; \
		touch .done/$(.TARGET); \
	fi;

mfsroot.bin: fixetc
	umount $(DESTDIR);
	mdconfig -d -u 0
	@echo "mfsroot is now ready to be copied to /tftpboot"
	[ "$(COPYMFSROOT)" -eq 1 ] && cp mfsroot /tftpboot || true

build-kernel:
	make -C /usr/src buildkernel KERNCONF=$(KERNCONF) -DNO_MODULES -DNOCLEAN $(MAKEOPTS)

install-kernel:
	gzip -v9c /usr/obj/usr/src/sys/$(KERNCONF)/kernel > kernel.gz

install-base: mfsroot
	gzip -v9c mfsroot > /tftpboot/mfsroot.gz

stats:
	make mount
	@cd $(DESTDIR); \
	for file in `find bin sbin libexec lib usr/bin usr/sbin usr/lib -type f`; do \
		du -s $$file; \
	done;	
	make unmount

statdirs:
	make mount
	cd $(DESTDIR); find ./ -type d -print0 | xargs -0 du -s 
	make unmount
