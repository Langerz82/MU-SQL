#!/bin/bash
###############################################################################
# MuMySQLServer Build Automation Script                                              #
# Written By: Ryan Ashley                                                     #
# Updated By: Cedric Servais                                                  #
# Copyright (c) 2014-2019 MuMySQLServer Project                                      #
# https://getMuMySQLServer.eu/                                                       #
#                                                                             #
# This program is free software; you can redistribute it and/or modify        #
# it under the terms of the GNU General Public License as published by        #
# the Free Software Foundation; either version 2 of the License, or           #
# (at your option) any later version.                                         #
#                                                                             #
# This program is distributed in the hope that it will be useful,             #
# but WITHOUT ANY WARRANTY; without even the implied warranty of              #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                #
# GNU General Public License for more details.                                #
#                                                                             #
# You should have received a copy of the GNU General Public License           #
# along with this program; if not, write to the Free Software                 #
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA     #
###############################################################################

# Global variables
DLGAPP="whiptail"
VERSION="0"
ROOTPATH="$HOME"
SRCPATH="$HOME/Documents/MuMySQL/src"
INSTPATH="$HOME/Documents/MuMySQL/build"
DB_PREFIX="zero"
USER="MuMySQLServer"
P_SOAP="0"
P_DEBUG="0"
P_STD_MALLOC="1"
P_ACE_EXTERNAL="1"
P_PGRESQL="0"
P_TOOLS="0"
P_SD3="1"
P_ELUNA="1"
P_BOTS="0"
CMAKE_CMD="cmake"


function UseCmake3()
{
	# set the command to cmake3 if its there
	which cmake3
	if [ $? -eq 0 ]; then
		CMAKE_CMD="cmake3"
	fi
}

# Function to test for dialog
function UseDialog()
{
  # Search for dialog
  which dialog

  # See if dialog was found
  if [ $? -eq 0 ]; then
    DLGAPP="dialog"
  fi
}

# Function to test if the user is root or not
function CheckRoot()
{
  if [ "$(id -u)" != "0" ]; then
	  Log "This script can only be used as root!" 1
	  exit 1
  else
	  Log "User is root, check passed" 0
  fi
}

# Function to detect the repos
function DetectLocalRepo()
{
  local CUR_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

  # First see if the Windows stuff is in place
  if [ ! -d ../win ]; then
    Log "Windows files directory does not exist, assuming repo has not been cloned." 0
    return 0
  fi

  # See if the sources dircetory exists
  if [ ! -d ../src ]; then
    Log "Source files directory does not exist, assuming repo has not been cloned." 0
    return 0
  fi

  # Check for the CMake directory
  if [ ! -d ../cmake ]; then
    Log "CMake directory does not exist, assuming repo has not been cloned." 0
    return 0
  fi

  # Set the default paths based on the current location
  SRCPATH=$( dirname $CUR_DIR )
  SRCPATH=$( dirname $SRCPATH )

  # Log the detected path
  Log "Detected cloned repository in $SRCPATH" 0
}



# Function to log results
function Log()
{
  local TIMESTAMP=$( date +%Y-%m-%d:%H:%M:%S )

  # Check the number of parameters
  if [ $# -ne 2 ]; then
    echo "Logging usage: Log <message> <echo flag>"
    return 1
  fi

  # Echo to the console if requested
  if [ $2 -eq 1 ]; then
    echo "$1"
  fi

  # Append the string to the log
  echo "$TIMESTAMP $1" >> ~/getMuMySQLServer.log
}



# Function to install prerequisite libraries
function GetPrerequisites()
{
  # First, we need to check the installer.
  installer=0

  which apt-get

  if [ $? -ne 0 ]; then
    Log "apt-get isn't the installer by default" 1
  else
    installer=1
	apt-get -y install git lsb-release curl
  fi

  which yum

  if [ $? -ne 0 ]; then
    Log "yum isn't the installer by default" 1
  else
	installer=1
	yum -y install git redhat-lsb curl
  fi

  which aptitude
  if [ $? -ne 0 ]; then
    Log "aptitude isn't the installer by default" 1
  else
    installer=1
	aptitude -y install git lsb-release curl
  fi

  # Then, let's check that we have the necessary tools to define the OS version.
  which lsb_release

  if [ $? -ne 0 ]; then
    Log "Cannot define your OS distribution and version." 1
    return 0
  fi

  local OS=$(lsb_release -si)
  local VER=$(lsb_release -sc)
  local OS_VER=1

  # Ask the user to continue
  $DLGAPP --backtitle "MuMySQLServer Linux Build Configuration" --title "Install Required Dependencies" \
    --yesno "Would you like to install the required build and development packages?" 8 60

  # Check the user's response
  if [ $? -ne 0 ]; then
    Log "User declined to install required tools and development libraries." 1
    return 0
  fi

  # Inform the user of the need for root access
  $DLGAPP --backtitle "MuMySQLServer Linux Build Configuration" --title "Install Required Dependencies" \
    --yesno "Installing packages requires root access, which you will be prompted for.\nDo you want to proceed?" 8 60

  # Check the user's response
  if [ $? -ne 0 ]; then
    Log "User declined to proved root access for package installation." 1
    return 0
  fi

  # Handle OS
  case ${OS} in
    "LinuxMint")
      case ${VER} in
        "sarah")
          # Linux Mint 18 - Ubuntu Xenial based
          su -c "aptitude -y install build-essential cmake libbz2-dev libace-dev libssl-dev libmysqlclient-dev" root
          ;;
        "rosa")
          # Linux Mint 17.3 - Ubuntu Trusty based
          su -c "aptitude -y install build-essential cmake libbz2-dev libace-dev libssl-dev libmysqlclient-dev" root
          ;;
        "rafaela")
          # Linux Mint 17.2 - Ubuntu Trusty based
          su -c "aptitude -y install build-essential cmake libbz2-dev libace-dev libssl-dev libmysqlclient-dev" root
          ;;
        "rebecca")
          # Linux Mint 17.1 - Ubuntu Trusty based
          su -c "aptitude -y install build-essential cmake libbz2-dev libace-dev libssl-dev libmysqlclient-dev" root
          ;;
        "qiana")
          # Linux Mint 17 - Ubuntu Trusty based
          su -c "aptitude -y install build-essential cmake libbz2-dev libace-dev libssl-dev libmysqlclient-dev" root
          ;;
        "maya")
          # Linux Mint 13 - Ubuntu Precise based
          su -c "aptitude -y install build-essential cmake libbz2-dev libace-dev libssl-dev libmysqlclient-dev" root
          ;;
        "betsy")
          # LMDE 2 - Debian Jessie based
          su -c "aptitude -y install build-essential linux-headers-$(uname -r) autoconf automake cmake libbz2-dev libace-dev libace-6.2.8 libssl-dev libmysqlclient-dev libtool zliblg-dev" root
          ;;
        *)
          OS_VER=0
          ;;
      esac
      ;;
    "Ubuntu")
      case ${VER} in
        "precise")
          # Ubuntu 12.04 LTS
          su -c "apt-get -y install curl autoconf automake cmake libbz2-dev libace-dev libssl-dev libmysqlclient-dev libtool" root
          ;;
        "trusty")
          # Ubuntu 14.04 LTS
          su -c "apt-get -y install curl autoconf automake cmake libbz2-dev libace-dev libssl-dev libmysqlclient-dev libtool" root
          ;;
        "xenial")
          # Ubuntu 16.04 LTS
          su -c "apt-get -y install curl autoconf automake cmake libbz2-dev libace-dev libssl-dev libmysqlclient-dev libtool" root
          ;;
        "yakkety")
          # Ubuntu 16.10
          su -c "apt-get -y install curl autoconf automake cmake libbz2-dev libace-dev libssl-dev libmysqlclient-dev libtool" root
          ;;
	"zesty")
	  # Ubuntu 17.04
	  su -c "apt-get -y install curl autoconf automake cmake libbz2-dev libace-dev libssl-dev libmysqlclient-dev libtool" root
	  ;;
	"artful")
	  # Ubuntu 17.10
	  su -c "apt-get -y install curl autoconf automake cmake libbz2-dev libace-dev libssl-dev libmysqlclient-dev libtool" root
	  ;;
        "bionic")
	  # Ubuntu 18.04
	  su -c "apt-get -y install curl autoconf automake cmake libbz2-dev libace-dev libssl-dev libmysqlclient-dev libtool" root
	  ;;
        *)
          OS_VER=0
          ;;
      esac
      ;;
    "Debian")
      case ${VER} in
        "jessie")
          # Debian 8.0 "current"
          su -c "aptitude -y install curl build-essential autoconf automake cmake libbz2-dev libace-dev libssl-dev libmysqlclient-dev libtool" root
          ;;
        "stretch")
          # Debian Next
          su -c "aptitude -y install curl build-essential autoconf automake cmake libbz2-dev libace-dev libssl-dev libmysqlclient-dev libtool" root
          ;;
        *)
          OS_VER=0
          ;;
      esac
      ;;
    "RedHatEntrepriseServer")
      case ${VER} in
        "santiago")
          # Red Hat 6.x
          su -c "yum -y install curl build-essential linux-headers-$(uname -r) autoconf automake cmake libbz2-dev libace-dev ace-6.3.3 libssl-dev libmysqlclient-dev libtool zliblg-dev" root
          ;;
        "maipo")
          # Red Hat 7.x
          su -c "yum -y install curl build-essential linux-headers-$(uname -r) autoconf automake cmake libbz2-dev libace-dev ace-6.3.3 libssl-dev libmysqlclient-dev libtool zliblg-dev" root
          ;;
        *)
          OS_VER=0
          ;;
      esac
      ;;
	"CentOS")
      case ${VER} in
        "Core")
          # Default CentOS - Adding necessary RPM third-party.
		  rpm -Uv ftp://ftp.pbone.net/mirror/ftp5.gwdg.de/pub/opensuse/repositories/devel:/libraries:/ACE:/micro/CentOS_7/x86_64/ace-6.3.3-55.1.x86_64.rpm
		  rpm -Uv ftp://rpmfind.net/linux/centos/7/os/x86_64/Packages/perl-Net-Telnet-3.03-19.el7.noarch.rpm
		  rpm -Uv ftp://ftp.pbone.net/mirror/ftp5.gwdg.de/pub/opensuse/repositories/devel:/libraries:/ACE:/micro:/versioned/CentOS_7/x86_64/mpc-6.3.3-42.1.x86_64.rpm
		  rpm -Uv ftp://rpmfind.net/linux/centos/7/os/x86_64/Packages/libtool-2.4.2-22.el7_3.x86_64.rpm
		  rpm -Uv ftp://ftp.pbone.net/mirror/ftp5.gwdg.de/pub/opensuse/repositories/devel:/libraries:/ACE:/micro/CentOS_7/x86_64/ace-devel-6.3.3-55.1.x86_64.rpm
		  su -c "yum -y install epel-release"
          su -c "yum -y install curl autoconf automake cmake3 ace-devel ace-6.3.3 openssl-devel mysql-devel libtool gcc-c++" root
          ;;
        *)
          OS_VER=0
          ;;
      esac
      ;;
    "Fedora")
      case ${VER} in
        "TwentyFive")
          # Fedora 25 - Adding necessary RPM third-party.
		  su -c "yum -y install autoconf automake libtool gcc-c++" root
		  # Getting and building ACE. Not provided in RPM for Fedora...
		  rm -rf ACE-6.3.3.tar.bz2
		  rm -rf ACE_wrappers
		  wget ftp://download.dre.vanderbilt.edu/previous_versions/ACE-6.3.3.tar.bz2
		  tar xjvf ACE-6.3.3.tar.bz2
		  export ACE_ROOT=/root/ACE_wrappers
		  echo '#include "ace/config-linux.h"' >> $ACE_ROOT/ace/config.h
		  echo 'include $(ACE_ROOT)/include/makeinclude/platform_linux.GNU' >> $ACE_ROOT/include/makeinclude/platform_macros.GNU
		  echo 'INSTALL_PREFIX=/usr/local' >> $ACE_ROOT/include/makeinclude/platform_macros.GNU
		  export LD_LIBRARY_PATH=$ACE_ROOT/lib:$LD_LIBRARY_PATH
		  CD $ACE_ROOT
		  make
		  make install
		  cd ~
		  # Installing remaining dependencies..
          su -c "yum -y install cmake openssl-devel mariadb-devel" root
          ;;
        *)
          OS_VER=0
          ;;
      esac
      ;;
	*)
      OS_VER=0
      ;;
  esac

  # See if a supported OS was detected
  if [ ${OS_VER} -ne 0 ]; then
    # Log success
    Log "The development tools and libraries have been installed!" 1
  else
    # Note the error
    Log "Could not identify the current OS. Nothing was installed." 1
  fi
}

# Function to setup the technical user
function GetUser()
{
  local TMPUSER="$USER"

  # Set the user
  TMPUSER=$($DLGAPP --backtitle "MuMySQLServer Linux Build Configuration" --title "User to run MuMySQLServer" \
     --inputbox "Default: $USER" 8 60 3>&2 2>&1 1>&3)

  # Exit if cancelled
  if [ $? -ne 0 ]; then
    Log "User selection was cancelled. No changes have been made to your system." 1
    exit 0
  fi

  # Change the user only if it was modified
  if [ ! -z "$TMPUSER" ]; then
    USER="$TMPUSER"
  fi

  # Validate user
  id $USER > /dev/null 2>&1
  if [ $? -ne 0 ]; then
    Log "Creating user: $USER" 1
    useradd -m -d /home/$USER $USER > /dev/null 2>&1

    if [ $? -ne 0 ]; then
      Log "Error: Failed to create the specified user!" 1
      exit 1
    fi

		usermod -L $USER > /dev/null 2>&1
  else
    # User already exist, asking to keep the user
    $DLGAPP --backtitle "MuMySQLServer Linux Build Configuration" --title "User already exist" \
      --yesno "Would you like to keep the user \"$USER\"?" 8 60

    if [ $? -ne 0 ]; then
      Log "Removing user: $USER" 1
      userdel -r $USER > /dev/null 2>&1

      Log "Creating user: $USER" 1
      useradd -m -d /home/$USER $USER > /dev/null 2>&1

      if [ $? -ne 0 ]; then
        Log "Error: Failed to create the specified user!" 1
        exit 1
      fi

		  usermod -L $USER > /dev/null 2>&1
    fi
  fi

  ROOTPATH="/home/"$USER
  Log "User: $USER" 0
}

# Function to get the source and installation paths
function GetPaths()
{
  local TMPPATH="$HOME"

  # Set the source path
  TMPPATH=$($DLGAPP --backtitle "MuMySQLServer Linux Build Configuration" --title "Source-Code Path" \
    --inputbox "Default: $SRCPATH" 8 60 3>&2 2>&1 1>&3)

  # Exit if cancelled
  if [ $? -ne 0 ]; then
    Log "Source path selection was cancelled. No changes have been made to your system." 1
    exit 0
  fi

  # Change the path only if it was modified
  if [ ! -z "$TMPPATH" ]; then
    SRCPATH="$TMPPATH"
  fi

  # Validate source path
  if [ ! -d "$SRCPATH" ]; then
    $DLGAPP --backtitle "MuMySQLServer Linux Build Configuration" --title "Path does not exist" \
      --yesno "Would you like to create the directory \"$SRCPATH\"?" 8 60

    if [ $? -eq 0 ]; then
      Log "Creating source path: $SRCPATH" 1
      mkdir -p "$SRCPATH" > /dev/null 2>&1

      # Check to see if the directory was created
      if [ $? -ne 0 ]; then
        Log "Error: Failed to create the specified source-code directory!" 1
        exit 1
      fi
    else
      Log "Source path creation cancelled. No modifications have been made to your system." 1
      exit 0
    fi
  else
    # Check for old sources
    if [ -d "$SRCPATH/server" ] || [ -d "$SRCPATH/database" ]; then
      # Ask to remove the old sources
      $DLGAPP --backtitle "MuMySQLServer Linux Build Configuration" --title "Path already exists" \
        --yesno "Would you like to remove the old sources? (Answer yes if you are cloning MuMySQLServer)" 9 60

      # Remove the old sources if requested
      if [ $? -eq 0 ]; then
        Log "Removing old sources from: $SRCPATH/*" 1
        rm -rf $SRCPATH/*

        # Check for removal failure
        if [ $? -ne 0 ]; then
          Log "Error: Failed to remove old sources!" 1
          exit 1
        fi
      fi
    fi
  fi

  # Set the installation path
  TMPPATH=$($DLGAPP --backtitle "MuMySQLServer Linux Build Configuration" --title "Installation Path" \
    --inputbox "Default: $INSTPATH" 8 60 3>&2 2>&1 1>&3)

  # Exit if cancelled
  if [ $? -ne 0 ]; then
    Log "Install path selection cancelled. Only the source path has been created." 1
    exit 0
  fi

  # Change the path only if it was modified
  if [ ! -z "$TMPPATH" ]; then
    INSTPATH="$TMPPATH"
  fi

  # Validate install path
  if [ ! -d "$INSTPATH" ]; then
    $DLGAPP --backtitle "MuMySQLServer Linux Build Configuration" --title "Path does not exist" \
      --yesno "Would you like to create the directory \"$INSTPATH\"?" 8 60

    if [ $? -eq 0 ];then
      Log "Creating install path: $INSTPATH" 1
      mkdir -p "$INSTPATH" > /dev/null 2>&1

      # Check to see if the directory was created
      if [ $? -ne 0 ]; then
        Log "Error: Failed to create the specified installation directory!" 1
        exit 1
      fi
    else
      Log "Install path creation cancelled. Only the source path has been created."
      exit 0
    fi
  else
    # Check for an old installation
    if [ -d "$INSTPATH/bin" ] || [ -d "$INSTPATH/lib" ] || [ -d "$INSTPATH/include" ]; then

      # Ask to remove the old installation
      $DLGAPP --backtitle "MuMySQLServer Linux Build Configuration" --title "Path already exists" \
        --yesno "Would you like to uninstall the current version of MuMySQLServer first?" 0 0

      # Check the user's response
      if [ $? -eq 0 ]; then
        Log "Removing old MuMySQLServer installation..." 1

        # Clean up the binaries
        if [ -d "$INSTPATH/bin" ]; then
          rm -rf $INSTPATH/bin
        fi

        # Clean up the old includes
        if [ -d "$INSTPATH/include" ]; then
          rm -rf $INSTPATH/include
        fi

        # Clean up the library files
        if [ -d "$INSTPATH/lib" ]; then
          rm -rf $INSTPATH/lib
        fi

        # Clean up the old logs
        if [ -d "$INSTPATH/logs" ]; then
          rm -rf $INSTPATH/logs/*
        fi
      fi
    fi
  fi

  # Log the settings
  Log "Install path: $INSTPATH" 0
  Log "Source path: $SRCPATH" 0
}



# Function to clone or update sources
function GetMuMySQLServer()
{
  local CLONE="0"
  local BRANCH=""

  CLONE=$($DLGAPP --backtitle "MuMySQLServer Linux Build Configuration" --title "Clone or update MuMySQLServer" \
    --menu "Would you like to clone, update, or continue?" 0 0 3 \
    0 "Clone a fresh copy of MuMySQLServer" \
    1 "Update your existing copy of MuMySQLServer" \
    2 "Use existing copy" \
    3>&2 2>&1 1>&3)

  # Exit if cancelled
  if [ $? -ne 0 ]; then
    Log "Source cloning cancelled. Only the install and source paths have been created." 1
    exit 0
  fi

  # Clone from scratch if selected
  if [[ $CLONE = *0* ]]; then
    # Pull a different branch?
    case "$VERSION" in
      0)
        releases=$(curl -s 'https://api.github.com/repos/MuMySQLServerzero/server/branches' | grep "name" | awk 'BEGIN{FS="\""}{print $4}' | tr '\n' ' ')
        ;;
      1)
        releases=$(curl -s 'https://api.github.com/repos/MuMySQLServerone/server/branches' | grep "name" | awk 'BEGIN{FS="\""}{print $4}' | tr '\n' ' ')
        ;;
      2)
        releases=$(curl -s 'https://api.github.com/repos/MuMySQLServertwo/server/branches' | grep "name" | awk 'BEGIN{FS="\""}{print $4}' | tr '\n' ' ')
        ;;
      3)
        releases=$(curl -s 'https://api.github.com/repos/MuMySQLServerthree/server/branches' | grep "name" | awk 'BEGIN{FS="\""}{print $4}' | tr '\n' ' ')
        ;;
      4)
        releases=$(curl -s 'https://api.github.com/repos/MuMySQLServerfour/server/branches' | grep "name" | awk 'BEGIN{FS="\""}{print $4}' | tr '\n' ' ')
        ;;
      *)
        Log "Error: Unknown version to select branch" 1
        ;;
    esac

    COUNTER=1
    RADIOLIST=""  # variable where we will keep the list entries for radiolist dialog
    for i in $releases; do
      if [ $COUNTER -eq 1 ]; then
        RADIOLIST="$RADIOLIST $COUNTER $i on "
        BRANCH=$i
      else
        RADIOLIST="$RADIOLIST $COUNTER $i off "
      fi
      let COUNTER=COUNTER+1
    done

    TMPBRANCH=$($DLGAPP --backtitle "MuMySQLServer Linux Build Configuration" --title "Select Branch" \
      --radiolist "Default: $BRANCH" 0 0 $COUNTER \
      $RADIOLIST \
      3>&2 2>&1 1>&3)

    # Exit if cancelled
    if [ $? -ne 0 ]; then
      Log "Branch selection cancelled. Only the install and source paths have been created." 1
      exit 0
    fi

    BRANCH=$(echo $releases | awk '{print $'$TMPBRANCH'}')

    # Set the branch
    if [ -z "$BRANCH" ]; then
      BRANCH="$releases | awk '{print $1}'"
    fi

    # Clone the selected version
    case "$VERSION" in
      0)
        Log "Cloning Zero branch: $BRANCH" 1
        git clone http://github.com/MuMySQLServerzero/server.git "$SRCPATH/server" -b $BRANCH --recursive
        git clone http://github.com/MuMySQLServerzero/database.git "$SRCPATH/database" -b $BRANCH --recursive
        ;;

      1)
        Log "Cloning One branch: $BRANCH" 1
        git clone http://github.com/MuMySQLServerone/server.git "$SRCPATH/server" -b $BRANCH --recursive
        git clone http://github.com/MuMySQLServerone/database.git "$SRCPATH/database" -b $BRANCH --recursive
        ;;

      2)
        Log "Cloning Two branch: $BRANCH" 1
        git clone http://github.com/MuMySQLServertwo/server.git "$SRCPATH/server" -b $BRANCH --recursive
        git clone http://github.com/MuMySQLServertwo/database.git "$SRCPATH/database" -b $BRANCH --recursive
        ;;

      3)
        Log "Cloning Three branch: $BRANCH" 1
        git clone http://github.com/MuMySQLServerthree/server.git "$SRCPATH/server" -b $BRANCH --recursive
        git clone http://github.com/MuMySQLServerthree/database.git "$SRCPATH/database" -b $BRANCH --recursive
        ;;

      4)
        Log "Cloning Four branch: $BRANCH" 1
        git clone http://github.com/MuMySQLServerfour/server.git "$SRCPATH/server" -b $BRANCH --recursive
        git clone http://github.com/MuMySQLServerfour/database.git "$SRCPATH/database" -b $BRANCH --recursive
        ;;

      *)
        Log "Error: Unknown release selected for cloning!" 1
        exit 1
        ;;
    esac

    # Log success
    Log "Cloned the selected repository!" 1
  fi

  # Update the local repositories if selected
  if [[ $CLONE = *1* ]]; then
    Log "Updating your local repository..." 1

    # Update the core sources
    cd "$SRCPATH/server"
    git pull

    # Now update the database sources
    cd "$SRCPATH/database"
    git pull

    # Log success
    Log "Updated the local respository!" 1
  fi

  # use existing repository
  if [[ $CLONE = *2* ]]; then
    Log "Using existing local repository" 1
  fi
}



# Function to set the build options
function GetBuildOptions()
{
  # Select build options
  OPTIONS=$($DLGAPP --backtitle "MuMySQLServer Linux Build Configuration" \
    --title "Build Options" \
    --checklist "Please select your build options" 0 56 7 \
    1 "Enable Debug" Off \
    2 "Use Standard Malloc" On \
    3 "Use External ACE Libraries" On \
    5 "Build Client Tools" On \
    3>&2 2>&1 1>&3)

  if [ $? -ne 0 ]; then
    Log "Build option selection cancelled. MuMySQLServer sources have been cloned." 1
    return 0
  fi

  # See if debug was selected
  if [[ $OPTIONS == *1* ]]; then
    P_DEBUG="1"
  else
    P_DEBUG="0"
  fi

  # See if standard malloc was selected
  if [[ $OPTIONS == *2* ]]; then
    P_STD_MALLOC="1"
  else
    P_STD_MALLOC="0"
  fi

  # See if external ACE was selected
  if [[ $OPTIONS == *3* ]]; then
    P_ACE_EXTERNAL="1"
  else
    P_ACE_EXTERNAL="0"
  fi

  # See if the client tools were selected
  if [[ $OPTIONS == *5* ]]; then
    P_TOOLS="1"
  else
    P_TOOLS="0"
  fi

}



# Function to build MuMySQLServer
function BuildMuMySQLServer()
{
  # Last chance to cancel building
  $DLGAPP --backtitle "MuMySQLServer Linux Build Configuration" --title "Proceed to build MuMySQLServer" \
    --yesno "Are you sure you want to build MuMySQLServer?" 8 60

  # Check the user's answer
  if [ $? -ne 0 ]; then
    Log "Cancelled by user. MuMySQLServer has been cloned but not built." 1
    exit 0
  fi

  # See if the build directory exists and clean up if possible
  if [ -d "$SRCPATH/build" ]; then
    # See if a makefile exists and clean up
    if [ -f $SRCPATH/build/Makefile ]; then
      Log "Cleaning the old build..." 1
      cd "$SRCPATH/build"
      make clean
    fi
  fi

  # Attempt to create the build directory if it doesn't exist
  if [ ! -d "$SRCPATH/build" ]; then
    mkdir "$SRCPATH/build"

    # See if creation was successful
    if [ $? -ne 0 ]; then
      Log "Error: Failed to create the build directory!" 1
      exit 1
    fi
  fi

  # Attempt to configure and build MuMySQLServer
  Log "Building MuMySQLServer..." 0
  cd "$SRCPATH/build"
  # make sure we are using the cmake3
  UseCmake3
  $CMAKE_CMD .. -DDEBUG=$P_DEBUG -DUSE_STD_MALLOC=$P_STD_MALLOC -DACE_USE_EXTERNAL=$P_ACE_EXTERNAL -DBUILD_TOOLS=$P_TOOLS -DSOAP=$P_SOAP -DCMAKE_INSTALL_PREFIX="$INSTPATH"
  make

  # Check for an error
  if [ $? -ne 0 ]; then
    Log "There was an error building MuMySQLServer!" 1
    exit 1
  fi

  # Log success
  Log "MuMySQLServer has been built!" 0
}



# Function to install MuMySQLServer
function InstallMuMySQLServer()
{
  # Ask to install now
  $DLGAPP --backtitle "MuMySQLServer Linux Build Configuration" --title "Install MuMySQLServer" \
    --yesno "Do you want to install MuMySQLServer now?" 8 0

  # Return if no
  if [ $? -ne 0 ]; then
    $DLGAPP --backtitle "MuMySQLServer Linux Build Configuration" --title "Install MuMySQLServer" \
      --msgbox "You may install MuMySQLServer later by changing to:\n$SRCPATH/build\nAnd running: make install" 24 60

    Log "MuMySQLServer has not been installed after being built." 1
    exit 0
  fi

  # Install MuMySQLServer
  cd "$SRCPATH/build"
  make install

  # Make sure the install succeeded
  if [ $? -ne 0 ]; then
    Log "There was an error installing MuMySQLServer!" 1
    exit 1
  fi
}



# Function to create a Code::Blocks project
function CreateCBProject
{
  # Create the dircetory if it does not exist
  if [ ! -d $SRCPATH/build ]; then
    mkdir $SRCPATH/build
  fi

  # Now create the C::B project
  cd $SRCPATH/build
  # make sure we are using the cmake3
  UseCmake3
  $CMAKE_CMD .. -G "CodeBlocks - Unix Makefiles"
}



# Prepare the log
Log "+------------------------------------------------------------------------------+" 0
Log "| MuMySQLServer Configuration Script                                                  |" 0
Log "| Written By: Ryan Ashley                                                      |" 0
Log "| Updated By: Cedric Servais                                                   |" 0
Log "+------------------------------------------------------------------------------+" 0

# Check if user who is running this is root
CheckRoot

# Select which dialog to use
UseDialog

# Select which activities to do
TASKS=$($DLGAPP --backtitle "MuMySQLServer Linux Build Configuration" --title "Select Tasks" \
  --checklist "Please select the tasks to perform" 0 70 8 \
  1 "Install Prerequisites" On \
  2 "Set Download And Install Paths" On \
  3 "Clone Source Repositories" On \
  4 "Build MuMySQLServer" On \
  5 "Install MuMySQLServer" On \
  8 "Create Code::Blocks Project File" Off \
  3>&2 2>&1 1>&3)

# Verify that the options were selected
if [ $? -ne 0 ]; then
  Log "All operations cancelled. Exiting." 1
  exit 0
fi

# Install prerequisites?
if [[ $TASKS == *1* ]]; then
  GetPrerequisites
fi

# Select release and set paths?
if [[ $TASKS == *2* ]] || [[ $TASKS == *3* ]] || [[ $TASKS == *4* ]] || [[ $TASKS == *5* ]] || [[ $TASKS == *7* ]]; then
  GetUser
fi

if [[ $TASKS == *2* ]] || [[ $TASKS == *3* ]] || [[ $TASKS == *4* ]] || [[ $TASKS == *5* ]] || [[ $TASKS == *6* ]] || [[ $TASKS == *7* ]]; then
  GetPaths
fi

# Clone repos?
if [[ $TASKS == *3* ]]; then
  GetMuMySQLServer
fi

# Build MuMySQLServer?
if [[ $TASKS == *4* ]]; then
  GetBuildOptions
  BuildMuMySQLServer
fi

# Install MuMySQLServer?
if [[ $TASKS == *5* ]]; then
  InstallMuMySQLServer
fi

# Create C::B project?
if [[ $TASKS == *8* ]]; then
  CreateCBProject
fi

# If one of these actions has been performed, then we know the user.
if [[ $TASKS == *2* ]] || [[ $TASKS == *3* ]] || [[ $TASKS == *4* ]] || [[ $TASKS == *5* ]] || [[ $TASKS == *7* ]]; then
  Log "Changing ownership of the extracted directories" 1
  chown -R $USER:$USER "$INSTPATH"
fi

# Display the end message
echo
echo "================================================================================"
echo "The selected tasks have been completed. If you built or installed MuMySQLServer, please"
echo "edit your configuration files to use the database you configured for your MuMySQLServer"
echo "server. If you have not configured your databases yet, please do so before"
echo "starting your server for the first time."
echo "================================================================================"
exit 0
