
# This file was created by mkconfig.rb when ruby was built.  Any
# changes made to this file will be lost the next time ruby is built.

module RbConfig
  RUBY_VERSION == "1.9.1" or
    raise "ruby lib version (1.9.1) doesn't match executable version (#{RUBY_VERSION})"

  TOPDIR = File.dirname(__FILE__).chomp!("/lib/ruby/1.9.1/i386-darwin10.2.0")
  DESTDIR = '' unless defined? DESTDIR
  CONFIG = {}
  CONFIG["DESTDIR"] = DESTDIR
  CONFIG["INSTALL"] = '/usr/bin/install -c'
  CONFIG["EXEEXT"] = ""
  CONFIG["prefix"] = (TOPDIR || DESTDIR + "/Users/loch/Downloads/ruby-1.9.1-p376/BRENT")
  CONFIG["ruby_install_name"] = "ruby"
  CONFIG["RUBY_INSTALL_NAME"] = "ruby"
  CONFIG["RUBY_SO_NAME"] = "ruby"
  CONFIG["BUILTIN_TRANSSRCS"] = " newline.c"
  CONFIG["MANTYPE"] = "doc"
  CONFIG["NROFF"] = "/usr/bin/nroff"
  CONFIG["vendorhdrdir"] = "$(rubyhdrdir)/vendor_ruby"
  CONFIG["sitehdrdir"] = "$(rubyhdrdir)/site_ruby"
  CONFIG["rubyhdrdir"] = "$(includedir)/$(RUBY_INSTALL_NAME)-$(ruby_version)"
  CONFIG["configure_args"] = " '--enable-shared' '--prefix=/Users/loch/Downloads/ruby-1.9.1-p376/BRENT/' 'CFLAGS=-arch i386 -arch x86_64' 'LDFLAGS=-arch i386 -arch x86_64'"
  CONFIG["vendordir"] = "$(libdir)/$(RUBY_INSTALL_NAME)/vendor_ruby"
  CONFIG["sitedir"] = "$(libdir)/$(RUBY_INSTALL_NAME)/site_ruby"
  CONFIG["ruby_version"] = "1.9.1"
  CONFIG["sitearch"] = "i386-darwin10.2.0"
  CONFIG["arch"] = "i386-darwin10.2.0"
  CONFIG["MAKEFILES"] = "Makefile"
  CONFIG["THREAD_MODEL"] = "pthread"
  CONFIG["EXPORT_PREFIX"] = ""
  CONFIG["COMMON_HEADERS"] = ""
  CONFIG["COMMON_MACROS"] = ""
  CONFIG["COMMON_LIBS"] = ""
  CONFIG["MAINLIBS"] = ""
  CONFIG["ENABLE_SHARED"] = "yes"
  CONFIG["DLDLIBS"] = ""
  CONFIG["SOLIBS"] = ""
  CONFIG["LIBRUBYARG_SHARED"] = "-l$(RUBY_SO_NAME)"
  CONFIG["LIBRUBYARG_STATIC"] = "-l$(RUBY_SO_NAME)-static"
  CONFIG["LIBRUBYARG"] = "$(LIBRUBYARG_SHARED)"
  CONFIG["LIBRUBY"] = "$(LIBRUBY_SO)"
  CONFIG["LIBRUBY_ALIASES"] = "lib$(RUBY_SO_NAME).$(MAJOR).$(MINOR).dylib lib$(RUBY_SO_NAME).dylib"
  CONFIG["LIBRUBY_SO"] = "lib$(RUBY_SO_NAME).$(MAJOR).$(MINOR).$(TEENY).dylib"
  CONFIG["LIBRUBY_A"] = "lib$(RUBY_SO_NAME)-static.a"
  CONFIG["RUBYW_INSTALL_NAME"] = ""
  CONFIG["rubyw_install_name"] = ""
  CONFIG["LIBRUBY_DLDFLAGS"] = "-install_name $(libdir)/lib$(RUBY_SO_NAME).dylib -current_version $(MAJOR).$(MINOR).$(TEENY) -compatibility_version $(ruby_version)"
  CONFIG["LIBRUBY_LDSHARED"] = "cc -dynamiclib -undefined suppress -flat_namespace"
  CONFIG["warnflags"] = "-Wall -Wno-parentheses"
  CONFIG["debugflags"] = "-g"
  CONFIG["optflags"] = "-O2"
  CONFIG["cflags"] = "$(optflags) $(debugflags) $(warnflags)"
  CONFIG["cppflags"] = ""
  CONFIG["RDOCTARGET"] = "install-doc"
  CONFIG["ARCHFILE"] = ""
  CONFIG["EXTOUT"] = ".ext"
  CONFIG["PREP"] = "miniruby$(EXEEXT)"
  CONFIG["setup"] = "Setup"
  CONFIG["EXTSTATIC"] = ""
  CONFIG["STRIP"] = "strip -A -n"
  CONFIG["TRY_LINK"] = ""
  CONFIG["LIBPATHENV"] = "DYLD_LIBRARY_PATH"
  CONFIG["RPATHFLAG"] = ""
  CONFIG["LIBPATHFLAG"] = " -L%s"
  CONFIG["LINK_SO"] = ""
  CONFIG["LIBEXT"] = "a"
  CONFIG["DLEXT2"] = ""
  CONFIG["DLEXT"] = "bundle"
  CONFIG["LDSHAREDXX"] = ""
  CONFIG["LDSHARED"] = "cc -dynamic -bundle -undefined suppress -flat_namespace"
  CONFIG["CCDLFLAGS"] = " -fno-common"
  CONFIG["STATIC"] = ""
  CONFIG["ARCH_FLAG"] = ""
  CONFIG["DLDFLAGS"] = ""
  CONFIG["ALLOCA"] = ""
  CONFIG["RMALL"] = "rm -fr"
  CONFIG["RMDIRS"] = "$(top_srcdir)/tool/rmdirs"
  CONFIG["MAKEDIRS"] = "mkdir -p"
  CONFIG["CP"] = "cp"
  CONFIG["RM"] = "rm -f"
  CONFIG["INSTALL_DATA"] = "$(INSTALL) -m 644"
  CONFIG["INSTALL_SCRIPT"] = "$(INSTALL)"
  CONFIG["INSTALL_PROGRAM"] = "$(INSTALL)"
  CONFIG["SET_MAKE"] = ""
  CONFIG["LN_S"] = "ln -s"
  CONFIG["DLLWRAP"] = ""
  CONFIG["WINDRES"] = ""
  CONFIG["NM"] = ""
  CONFIG["OBJCOPY"] = ""
  CONFIG["OBJDUMP"] = ""
  CONFIG["ASFLAGS"] = ""
  CONFIG["AS"] = "as"
  CONFIG["AR"] = "ar"
  CONFIG["RANLIB"] = "ranlib"
  CONFIG["COUTFLAG"] = "-o "
  CONFIG["OUTFLAG"] = "-o "
  CONFIG["CPPOUTFILE"] = "-o conftest.i"
  CONFIG["GNU_LD"] = "no"
  CONFIG["EGREP"] = "/usr/bin/grep -E"
  CONFIG["GREP"] = "/usr/bin/grep"
  CONFIG["CPP"] = "gcc -E"
  CONFIG["CXXFLAGS"] = " $(cxxflags)"
  CONFIG["CXX"] = "g++"
  CONFIG["OBJEXT"] = "o"
  CONFIG["CPPFLAGS"] = " -D_XOPEN_SOURCE -D_DARWIN_C_SOURCE $(DEFS) $(cppflags)"
  CONFIG["LDFLAGS"] = "-L. -arch i386 -arch x86_64 -L/usr/local/lib"
  CONFIG["CFLAGS"] = "-arch i386 -arch x86_64 $(cflags)  -fno-common -pipe -fno-common"
  CONFIG["CC"] = "gcc"
  CONFIG["target_os"] = "darwin10.2.0"
  CONFIG["target_vendor"] = "apple"
  CONFIG["target_cpu"] = "i386"
  CONFIG["target"] = "i386-apple-darwin10.2.0"
  CONFIG["host_os"] = "darwin10.2.0"
  CONFIG["host_vendor"] = "apple"
  CONFIG["host_cpu"] = "i386"
  CONFIG["host"] = "i386-apple-darwin10.2.0"
  CONFIG["build_os"] = "darwin10.2.0"
  CONFIG["build_vendor"] = "apple"
  CONFIG["build_cpu"] = "i386"
  CONFIG["build"] = "i386-apple-darwin10.2.0"
  CONFIG["TEENY"] = "1"
  CONFIG["MINOR"] = "9"
  CONFIG["MAJOR"] = "1"
  CONFIG["BASERUBY"] = "ruby"
  CONFIG["target_alias"] = ""
  CONFIG["host_alias"] = ""
  CONFIG["build_alias"] = ""
  CONFIG["LIBS"] = "-lpthread -ldl -lobjc "
  CONFIG["ECHO_T"] = ""
  CONFIG["ECHO_N"] = ""
  CONFIG["ECHO_C"] = "\\\\c"
  CONFIG["DEFS"] = ""
  CONFIG["mandir"] = "$(datarootdir)/man"
  CONFIG["localedir"] = "$(datarootdir)/locale"
  CONFIG["libdir"] = "$(exec_prefix)/lib"
  CONFIG["psdir"] = "$(docdir)"
  CONFIG["pdfdir"] = "$(docdir)"
  CONFIG["dvidir"] = "$(docdir)"
  CONFIG["htmldir"] = "$(docdir)"
  CONFIG["infodir"] = "$(datarootdir)/info"
  CONFIG["docdir"] = "$(datarootdir)/doc/$(PACKAGE)"
  CONFIG["oldincludedir"] = "/usr/include"
  CONFIG["includedir"] = "$(prefix)/include"
  CONFIG["localstatedir"] = "$(prefix)/var"
  CONFIG["sharedstatedir"] = "$(prefix)/com"
  CONFIG["sysconfdir"] = "$(prefix)/etc"
  CONFIG["datadir"] = "$(datarootdir)"
  CONFIG["datarootdir"] = "$(prefix)/share"
  CONFIG["libexecdir"] = "$(exec_prefix)/libexec"
  CONFIG["sbindir"] = "$(exec_prefix)/sbin"
  CONFIG["bindir"] = "$(exec_prefix)/bin"
  CONFIG["exec_prefix"] = "$(prefix)"
  CONFIG["PACKAGE_URL"] = ""
  CONFIG["PACKAGE_BUGREPORT"] = ""
  CONFIG["PACKAGE_STRING"] = ""
  CONFIG["PACKAGE_VERSION"] = ""
  CONFIG["PACKAGE_TARNAME"] = ""
  CONFIG["PACKAGE_NAME"] = ""
  CONFIG["PATH_SEPARATOR"] = ":"
  CONFIG["SHELL"] = "/bin/sh"
  CONFIG["rubylibdir"] = "$(libdir)/$(ruby_install_name)/$(ruby_version)"
  CONFIG["archdir"] = "$(rubylibdir)/$(arch)"
  CONFIG["sitelibdir"] = "$(sitedir)/$(ruby_version)"
  CONFIG["sitearchdir"] = "$(sitelibdir)/$(sitearch)"
  CONFIG["vendorlibdir"] = "$(vendordir)/$(ruby_version)"
  CONFIG["vendorarchdir"] = "$(vendorlibdir)/$(sitearch)"
  CONFIG["topdir"] = File.dirname(__FILE__)
  MAKEFILE_CONFIG = {}
  CONFIG.each{|k,v| MAKEFILE_CONFIG[k] = v.dup}
  def RbConfig::expand(val, config = CONFIG)
    val.gsub!(/\$\$|\$\(([^()]+)\)|\$\{([^{}]+)\}/) do
      var = $&
      if !(v = $1 || $2)
	'$'
      elsif key = config[v = v[/\A[^:]+(?=(?::(.*?)=(.*))?\z)/]]
	pat, sub = $1, $2
	config[v] = false
	RbConfig::expand(key, config)
	config[v] = key
	key = key.gsub(/#{Regexp.quote(pat)}(?=\s|\z)/n) {sub} if pat
	key
      else
	var
      end
    end
    val
  end
  CONFIG.each_value do |val|
    RbConfig::expand(val)
  end
end
Config = RbConfig # compatibility for ruby-1.8.4 and older.
CROSS_COMPILING = nil unless defined? CROSS_COMPILING
