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
set -x #echo on
DLGAPP="whiptail"
VERSION="0"
ROOTPATH="$HOME"
SRCPATH="$HOME/Documents/MuMySQL"
INSTPATH="$HOME/Documents/MuMySQL/build"
DB_PREFIX="zero"
USER="ubuntu"
P_SOAP="0"
P_DEBUG="0"
P_STD_MALLOC="1"
P_ACE_EXTERNAL="1"
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
    if [ -d "$INSTPATH" ]; then
      # Ask to remove the old sources
      $DLGAPP --backtitle "MuMySQLServer Linux Build Configuration" --title "Path already exists" \
        --yesno "Would you like to remove the old sources? (Answer yes if you are cloning MuMySQLServer)" 9 60

      # Remove the old sources if requested
      if [ $? -eq 0 ]; then
        Log "Removing old sources from: $SRCPATH/../build" 1
        rm -rf $SRCPATH/../build

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
function BuildConnectServer()
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
  if [ -d "$INSTPATH" ]; then
    # See if a makefile exists and clean up
    if [ -f $INSTPATH/Makefile ]; then
      Log "Cleaning the old build..." 1
      cd "$INSTPATH"
      make clean
    fi
  fi

  # Attempt to create the build directory if it doesn't exist
  if [ ! -d "$INSTPATH" ]; then
    mkdir "$INSTPATH"

    # See if creation was successful
    if [ $? -ne 0 ]; then
      Log "Error: Failed to create the build directory!" 1
      exit 1
    fi
  fi

CMAKE_SOURCE_PATH=$SRCPATH/src/connectserver
CMAKE_BUILD_PATH=$INSTPATH/connectserver

  # Attempt to configure and build MuMySQLServer
  Log "Building MuMySQLServer..." 0
  cd "$INSTPATH"
  # make sure we are using the cmake3
  UseCmake3
  $CMAKE_CMD $SRCPATH -DDEBUG=$P_DEBUG -DUSE_STD_MALLOC=$P_STD_MALLOC -DACE_USE_EXTERNAL=$P_ACE_EXTERNAL -DBUILD_TOOLS=$P_TOOLS -DSOAP=$P_SOAP -DCMAKE_INSTALL_PREFIX="$INSTPATH"
  make

  # Check for an error
  if [ $? -ne 0 ]; then
    Log "There was an error building MuMySQLServer!" 1
    exit 1
  fi

  # Log success
  Log "MuMySQLServer has been built!" 0
}

# Function to build MuMySQLServer
function BuildGameServer()
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
  if [ -d "$INSTPATH" ]; then
    # See if a makefile exists and clean up
    if [ -f $INSTPATH/Makefile ]; then
      Log "Cleaning the old build..." 1
      cd "$INSTPATH"
      make clean
    fi
  fi

  # Attempt to create the build directory if it doesn't exist
  if [ ! -d "$INSTPATH" ]; then
    mkdir "$INSTPATH"

    # See if creation was successful
    if [ $? -ne 0 ]; then
      Log "Error: Failed to create the build directory!" 1
      exit 1
    fi
  fi

CMAKE_SOURCE_PATH=$SRCPATH/src/gameserver
CMAKE_BUILD_PATH=$INSTPATH/gameserver


  # Attempt to configure and build MuMySQLServer
  Log "Building MuMySQLServer..." 0
  cd "$INSTPATH"
  # make sure we are using the cmake3
  UseCmake3
  $CMAKE_CMD $SRCPATH -DDEBUG=$P_DEBUG -DUSE_STD_MALLOC=$P_STD_MALLOC -DACE_USE_EXTERNAL=$P_ACE_EXTERNAL -DBUILD_TOOLS=$P_TOOLS -DSOAP=$P_SOAP -DCMAKE_INSTALL_PREFIX="$INSTPATH"
  make

  # Check for an error
  if [ $? -ne 0 ]; then
    Log "There was an error building MuMySQLServer!" 1
    exit 1
  fi

  # Log success
  Log "MuMySQLServer has been built!" 0
}


# Function to create a Code::Blocks project
function CreateCBProject
{
  # Create the dircetory if it does not exist
  if [ ! -d $INSTPATH ]; then
    mkdir $INSTPATH
  fi

  # Now create the C::B project
  cd $INSTPATH
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
TASKS=$($DLGAPP --backtitle "MuMySQLServer Linux Build Configuration" \
  --title "Select Tasks" \
  --checklist "Please select the tasks to perform" 0 70 8 \
  1 "Install Prerequisites" Off \
  2 "Build ConnectServer" On \
  3 "Build GameServer" Off \
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

if [[ $TASKS == *2* ]] || [[ $TASKS == *3* ]] || [[ $TASKS == *4* ]] || [[ $TASKS == *5* ]] || [[ $TASKS == *6* ]] || [[ $TASKS == *7* ]]; then
  GetPaths
fi


# Clone repos?
if [[ $TASKS == *2* ]]; then
  GetBuildOptions
  BuildConnectServer
fi

# Clone repos?
if [[ $TASKS == *3* ]]; then
  GetBuildOptions
  BuildGameServer
fi

# Create C::B project?
if [[ $TASKS == *8* ]]; then
  CreateCBProject
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
