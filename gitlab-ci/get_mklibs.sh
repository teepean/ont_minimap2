#! /usr/bin/env bash

REMOTE=sparse


function remove-ofan-mklibs-files {
    # Get rid of existing includes and cmake things in mklibs
    # $1 root minknow-libs checkout dir
    # $2 os-specific folder within minknow-libs (e.g. osx_64)
    rm -rf $1/$2/include/albacore || echo "failed to remove $1/$2/include/albacore"
    rm -rf $1/$2/include/ont_core || echo "failed to remove $1/$2/include/ont_core"
    rm -rf $1/$2/lib/cmake || echo "failed to remove $1/$2/lib/cmake"
    echo "Existing ofan mklibs files removed"
}


if [ $# -ne 4 ]; then
    echo "usage: get_mklibs.sh directory branch folder"
    echo "    directory     Directory to clone mklibs to."
    echo "    group         Group where mklibs lives, e.g. ofan / minknow."
    echo "    branch        Branch of mklibs to check out."
    echo "    folder        Folder within mklibs to check out."
    echo
    echo "Note this script checks out *only* one folder from mklibs."
    exit 1
else
    ORIG_DIR=$(pwd)
    # Check if we have the directory already
    if [ -d $1 ]; then
	echo "Found mklibs directory $1."
	cd $1
	if ( git fetch $REMOTE &&
		git reset --hard $REMOTE/$3 &&
		cd $ORIG_DIR ); then
	    echo "Reset existing mklibs directory."
	    remove-ofan-mklibs-files $1 $4
	    exit 0
	elif ( cd $ORIG_DIR &&
		rm -rf $1 ); then
	    echo "Could not reset mklibs repo -- trying a fresh clone."
	    # We'll do that below now that we've cleaned the slate.
	else
	    echo "FAILURE: Could not remove existing mklibs directory."
	    exit 1
	fi
    fi
    # We'll use sparseCheckout and --depth 1 to try and speed things up
    # See http://unix.stackexchange.com/a/233335
    if ( git init $1  &&
	    cd $1
	    git remote add -t $3 $REMOTE https://$GITUSER@git.oxfordnanolabs.local/$2/minknow-libs.git &&
	    git fetch $REMOTE --depth 1 &&
	    git config core.sparseCheckout true &&
	    echo "$4" >> .git/info/sparse-checkout &&
	    git lfs fetch -I $4 $REMOTE $REMOTE/$3 &&
	    git checkout $REMOTE/$3 &&
	    cd $ORIG_DIR ); then
	echo "Cloned new mklibs successfully"
	echo "    and checked out branch $3 folder $4."
	remove-ofan-mklibs-files $1 $4
	exit 0
    else
	echo "FAILURE: Unable to clone new mklibs!"
	exit 1
    fi
fi
