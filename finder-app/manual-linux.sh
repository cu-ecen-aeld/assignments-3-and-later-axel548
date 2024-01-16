#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

set -e
set -u

OUTDIR=/tmp/aeld
KERNEL_REPO=https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git
KERNEL_VERSION=v5.1.10
BUSYBOX_VERSION=1_33_1
FINDER_APP_DIR=$(realpath $(dirname $0))
ARCH=arm64
CROSS_COMPILE=aarch64-none-linux-gnu-

if [ $# -lt 1 ]
then
    echo "Using default directory ${OUTDIR} for output"
else
    OUTDIR=$(realpath $1)
    echo "Using passed directory ${OUTDIR} for output"
fi

mkdir -p ${OUTDIR}

# a. Take a single argument outdir which is the location on the filesystem where the output files should be placed.
# Replace all references to “outdir” in the remainder of the assignment with the absolute path to this directory.
cd "$OUTDIR"

# b. Create a directory outdir if it doesn’t exist. Fail if the directory could not be created.
if [ ! -d "$OUTDIR" ]; then
    mkdir "$OUTDIR" || { echo "Failed to create directory: $OUTDIR"; exit 1; }
fi

cd "$OUTDIR"

# c. Build a kernel image
if [ ! -d "${OUTDIR}/linux-stable" ]; then
    echo "CLONING GIT LINUX STABLE VERSION ${KERNEL_VERSION} IN ${OUTDIR}"
    git clone ${KERNEL_REPO} --depth 1 --single-branch --branch ${KERNEL_VERSION}
fi

if [ ! -e ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ]; then
    cd linux-stable
    echo "Checking out version ${KERNEL_VERSION}"
    git checkout ${KERNEL_VERSION}

    # c.i. Use git to clone the linux kernel source tree if it doesn’t exist in outdir.
    # Checkout the tag specified in the manual-linux.sh script
    # 1. Use --depth 1 command line argument with git if you’d like to minimize download time
    if [ ! -d "${OUTDIR}/linux-stable" ]; then
        git clone --depth 1 ${KERNEL_REPO} --single-branch --branch ${KERNEL_VERSION}
    fi

    # c.i. 2. You may alternatively download and unzip from the corresponding kernel version tarball obtained from
    # https://mirrors.edge.kernel.org/pub/linux/kernel (useful for slow Internet connections).
    # (Assuming tarball is downloaded and extracted to ${OUTDIR}/linux-stable)
    if [ ! -e ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ]; then
        echo "Copying kernel source from tarball"
        cp -r ${OUTDIR}/linux-stable-source/* linux-stable
    fi

    # TODO: Add your kernel build steps here
    # ...

fi

# c.d. Copy resulting files generated in step 1.c to outdir.
cp ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ${OUTDIR}

# e. Your script should build a root filesystem in outdir/rootfs as described in the Module 2 content.
# Performing all of these operations in an automated fashion and completing the TODO items in the “finder-app/manual-linux.sh” script.
# The end result should be a outdir/Image kernel image and outdir/initramfs.cpio.gz file based on the content of a staging directory tree.
cd "$OUTDIR"
if [ ! -d "${OUTDIR}/rootfs" ]; then
    mkdir rootfs
fi

# e.i. Skip the modules_install step discussed in the video content.
# The modules generated with the default kernel build are too large to fit in the initramfs with default memory.
# Alternatively, you can increase the -m argument in the start qemu scripts to a value large enough to fit (currently must be >512m).
# You don't need modules for the simple example we use in assignment 3.

# e.ii. Your writer application from Assignment 2 should be cross compiled and placed in the outdir/rootfs/home directory for execution on target.
# (Assuming writer application binary is in ${FINDER_APP_DIR}/writer and the source code is in ${FINDER_APP_DIR}/writer-src)
${CROSS_COMPILE}gcc -o ${FINDER_APP_DIR}/writer ${FINDER_APP_DIR}/writer-src/writer.c
cp ${FINDER_APP_DIR}/writer ${OUTDIR}/rootfs/home

# f. Copy your finder.sh, conf/username.txt, conf/assignment.txt, and finder-test.sh scripts from Assignment 2 into the outdir/rootfs/home directory.
# f.i. Modify the finder-test.sh script to reference conf/assignment.txt instead of ../conf/assignment.txt.
cp ${FINDER_APP_DIR}/finder.sh ${FINDER_APP_DIR}/conf/username.txt ${FINDER_APP_DIR}/conf/assignment.txt ${OUTDIR}/rootfs/home
sed -i 's|\.\./conf/assignment\.txt|conf/assignment.txt|g' ${OUTDIR}/rootfs/home/finder-test.sh

# g. Copy the autorun-qemu.sh script into the outdir/rootfs/home directory
cp ${FINDER_APP_DIR}/autorun-qemu.sh ${OUTDIR}/rootfs/home

# h. Create a standalone initramfs and outdir/initramfs.cpio.gz file based on the contents of the staging directory tree.
cd ${OUTDIR}/rootfs
find . | cpio -H newc -o > ../initramfs.cpio
gzip -f ../initramfs.cpio
