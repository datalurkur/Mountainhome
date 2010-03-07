#!/bin/sh
# Original script created by - Joel Beaudoin <joeldoin@gmail.com>

boost_ver="1.42.0"
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
    curl -O -L $repository_url$pkgname
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

build_libs=
available_libs=
get_libs_and_targets () {
  cd $boostroot
  available_libs=`ls -1 -d libs/*/build | cut -d / -f 2`
  build_libs=$available_libs
  for lib in $exclude_libs; do
    build_libs=`echo $build_libs | sed -e s/$lib//g`
  done # build_libs -> the libraries that we want in the framework
  cd -
}

bjam_args='-j4 --without-math --prefix=../install toolset=darwin threading=multi link=shared variant=release cxxflags="-arch i386 -arch x86_64" linkflags="-arch i386 -arch x86_64"'
run_bjam() {
  cmd="bjam stage $1 $bjam_args $2"
  echo $cmd
  perl -e "exec '$cmd'" # Fixes some issues with quotes in $cmd
}

# build_boost_libs()
#
# runs a normal boost build (non-install) with a shared link spec, single-threaded, release
# this is done by specifying libs/date_time/build libs/filesystem/build ... as the bjam targets
#
build_boost_libs () {
  # note: attempt to build using 'bjam -f <jamfile> --builddir=<dir>' to build
  #       outside $boostroot didn't work
  cd $boostroot
  echo "Building Boost ..."
  run_bjam "" "&> $workdir/build.log" || error "Boost libs not built successfully"
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
    lib_link_cmd=$(run_bjam "-n -a" "$lib" | grep dynamiclib)
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
  c++ -dynamiclib -arch i386 -arch x86_64 -o $1 -install_name @executable_path/../Frameworks/$fmwrkname.framework/Versions/A/$fmwrkname $modules $lib_search_paths $extern_libs
  cd -
}

fetch_and_unpack_boost $boost_ver
create_bundle_structure $fmwrkname
copy_headers_to_bundle
get_libs_and_targets
build_boost_libs
create_boost_library $fmwrklib
finish_bundle
echo "Done."
echo
