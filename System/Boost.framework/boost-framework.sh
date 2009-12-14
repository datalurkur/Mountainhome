#!/bin/sh
#
# create a Boost framework in the current directory
#
# make sure to move the created Boost.framework bundle to
# either ~/Library/Frameworks or /Library/Frameworks"
#
# 2007.10.16 - initial script created - Joel Beaudoin <joeldoin@gmail.com>
#
#

boost_ver="1.34.1"
exclude_libs="test" # libs not to include in the framework dylib


workdir=`pwd`

error () {
  echo "Error: $1"
  exit 1
}

repository_url="http://downloads.sourceforge.net/boost/"
pkgname_ver_encoding=`echo $boost_ver | sed -e 's/\./_/g'`
pkgname="boost_${pkgname_ver_encoding}.tar.bz2"
boostroot="$workdir/boost_${pkgname_ver_encoding}"
fetch_and_unpack_boost () {
  # $1 - boost version n.nn.nn[.alpha|.beta]
  # download boost tarball and unpack
  if [ ! -e $pkgname ]; then
    echo "Fetching Boost $1 ..."
    curl -O -# -L $repository_url$pkgname
  fi
  if [ ! -d $boostroot ]; then
    echo "Unpacking ..."
    tar xjf $pkgname
  fi
}

# create_bundle_structure()
#
# creates the framework bundle structure in the current directory
#
#    Boost.framework
#      |--Versions
#           |--A
#             |--Headers
#             |--Libraries (for things like Boost.Test)
#             |--Boost (dylib that will be built later)
#           |--Current -> A
#      |--Headers -> Current/Headers
#      |--Libraries -> Current/Libraries
#      |--Boost -> Current/Boost (later in the script)
#
fmwrkname="Boost"
fmwrkdir="$workdir/${fmwrkname}.framework"
fmwrklibdir="$fmwrkdir/Versions/A"
fmwrkheaderdir="$fmwrkdir/Versions/A/Headers"
fmwrklib="$fmwrklibdir/$fmwrkname"
create_bundle_structure () {
  # $1 - framework name
  if [ ! -d $fmwrkdir ]; then
    echo "Creating initial framework bundle structure ..."
    mkdir -p $fmwrklibdir
    mkdir $fmwrkheaderdir
    cd $fmwrkdir/Versions
    ln -s A Current # Versions/Current -> Versions/A
    cd .. # $fmwrkdir
    ln -s Versions/Current/Headers Headers
    cd .. # start dir
  fi
}

finish_bundle () {
  echo "Finishing the bundle ..."
  cd $fmwrkdir
  ln -s -f Versions/Current/$fmwrkname .
  cd -
}

copy_headers_to_bundle () {
  # copy the boost headers to the framework
  if [ ! -e headers_copied ]; then
    echo "Copying Boost header files to the framework bundle ..."
    cp -R $boostroot/boost/* $fmwrkdir/Headers
    touch headers_copied
  fi
}

configure_boost () {
  # run the boost configure to build bjam and generate misc files
  # note: configure expects to be run from boostroot
  # note: added --prefix to avoid accidental /usr/lib install
  if [ ! -e $boostroot/Makefile ]; then
    echo "Configuring the Boost build ..."
    cd $boostroot
    ./configure --prefix=$workdir/install &> /dev/null
    cd -
  fi
}

available_libs=
build_libs=
targets=
get_libs_and_targets () {
  cd $boostroot
  available_libs=`ls -1 -d libs/*/build | cut -d / -f 2`
  build_libs=$available_libs
  for lib in $exclude_libs; do
    build_libs=`echo $build_libs | sed -e s/$lib//g`
  done # build_libs -> the libraries that we want in the framework
  for lib in $build_libs; do
    targets="$targets libs/$lib/build"
  done
  cd -
}

bjam=
bjam_cmd=
get_bjam_cmd () {
  cd $boostroot
  if [ ! -e Makefile ]; then
    echo "Error: Boost not configured properly"
    exit 1
  fi
  bjam=`cat Makefile | grep '^BJAM=' | sed -e s/BJAM=//`
  bjam_cmd="$bjam --user-config=user-config.jam \
     --prefix=../install --layout=system \
     link=shared threading=single variant=release"
  cd -
}

# build_boost_libs()
#
# runs a normal boost build (non-install) with a shared link spec, single-threaded, release
# this is done by specifying libs/date_time/build libs/filesystem/build ... as the bjam targets
#
build_boost_libs () {
  # $1 - targets

  # note: attempt to build using 'bjam -f <jamfile> --builddir=<dir>' to build
  #       outside $boostroot didn't work
  cd $boostroot
  echo "Building Boost ..."
  $bjam_cmd $1 &> $workdir/build.log || error "Boost libs not built successfully"
  cd -
}

# create_boost_library()
#
# creates the boost framework library with the specified libraries
create_boost_library () {
  # $1 - output file

  echo "Creating framework library ..."
  echo "  Getting link info ..."
  cd $boostroot
  for lib in $build_libs; do
    # note: this grep will only work if the build spec causes only one library to be built
    lib_link_cmd=`$bjam_cmd $lib -n -a | grep dynamiclib`
    # extract the object files mentioned on the link line
    modules="$modules $(echo $lib_link_cmd | \
               awk '{ for (i=1;i<=NF;++i) if ($i~/\.o/) print $i }' | \
               sed -e s/\"//g)"
    # extract any library search paths (-L) mentioned on the link line
    lib_search_paths="$lib_search_paths $(echo $lib_link_cmd | \
                        awk '{ for (i=1;i<=NF;++i) if ($i~/\-L/) print $i }' | \
                        sed -e s/\"//g)"
    # extract any external library links (-l) mentioned on the link line
    extern_libs="$extern_libs $(echo $lib_link_cmd | \
                   awk '{ for (i=1;i<=NF;++i) if ($i~/\-l/) print $i }' | \
                   sed -e s/\"//g)"
  done
  # get rid of duplicate modules
  modules=`echo $modules | awk '{ for (i=1;i<=NF;++i) print $i}' | sort | uniq`
  echo "  Linking ..."
  c++ -dynamiclib -o $1 -install_name @executable_path/../Frameworks/$fmwrkname.framework/Versions/A/$fmwrkname $modules $lib_search_paths $extern_libs
  cd -
}

fetch_and_unpack_boost $boost_ver
create_bundle_structure $fmwrkname
copy_headers_to_bundle
configure_boost
get_libs_and_targets
get_bjam_cmd
build_boost_libs "$targets"
create_boost_library $fmwrklib
finish_bundle
echo "Done."
echo

# *** ADD INFO.PLIST FILE CREATION ***

# run a simple test, linking against the new framework
