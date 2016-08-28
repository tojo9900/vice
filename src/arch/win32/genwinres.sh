#!/bin/sh
idr_counter=100
idd_counter=100
idc_counter=100
ids_counter=1
idm_counter=100
idi_counter=1

echo "/*"
echo " * res.h"
echo " *"
echo " * Autogenerated by genwinres.sh, DO NOT EDIT !!!"
echo " *"
echo " * Written by"
echo " *  Andreas Boose <viceteam@t-online.de>"
echo " *  Ettore Perazzoli <ettore@comm2000.it>"
echo " *  Tibor Biczo <crown@t-online.hu>"
echo " *  Marco van den Heuvel <blackystardust68@yahoo.com>"
echo " *"
echo " * This file is part of VICE, the Versatile Commodore Emulator."
echo " * See README for copyright notice."
echo " *"
echo " *  This program is free software; you can redistribute it and/or modify"
echo " *  it under the terms of the GNU General Public License as published by"
echo " *  the Free Software Foundation; either version 2 of the License, or"
echo " *  (at your option) any later version."
echo " *"
echo " *  This program is distributed in the hope that it will be useful,"
echo " *  but WITHOUT ANY WARRANTY; without even the implied warranty of"
echo " *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the"
echo " *  GNU General Public License for more details."
echo " *"
echo " *  You should have received a copy of the GNU General Public License"
echo " *  along with this program; if not, write to the Free Software"
echo " *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA"
echo " *  02111-1307  USA."
echo " *"
echo " */"
echo ""
echo "#ifndef VICE_RES_H"
echo "#define VICE_RES_H"

while read data
do
  ok="no"
  case ${data%%_*} in
    "IDR") echo "#define" $data $idr_counter
           idr_counter=`expr $idr_counter + 1`
           ok="yes"
    ;;
    "IDD") echo "#define" $data $idd_counter
           idd_counter=`expr $idd_counter + 1`
           ok="yes"
    ;;
    "IDC") echo "#define" $data $idc_counter
           idc_counter=`expr $idc_counter + 1`
           ok="yes"
    ;;
    "IDS") echo "#define" $data $ids_counter
           ids_counter=`expr $ids_counter + 1`
           ok="yes"
    ;;
    "IDM") echo "#define" $data $idm_counter
           idm_counter=`expr $idm_counter + 1`
           ok="yes"
    ;;
    "IDI") echo "#define" $data $idi_counter
           idi_counter=`expr $idi_counter + 1`
           ok="yes"
    ;;
  esac
  if test $ok = "no"
  then
    echo $data
  fi
done

echo ""
echo "#define FIRST_IDR 100"
echo "#define LAST_IDR" $idr_counter

echo "#define FIRST_IDD 100"
echo "#define LAST_IDD" $idd_counter

echo "#define FIRST_IDC 100"
echo "#define LAST_IDC" $idc_counter

echo "#define FIRST_IDS 1"
echo "#define LAST_IDS" $ids_counter

echo "#define FIRST_IDM 100"
echo "#define LAST_IDM" $idm_counter

echo "#define FIRST_IDI 1"
echo "#define LAST_IDI" $idi_counter

echo "#define FIRST_GENERAL 1"
echo "#define LAST_GENERAL 1"

echo ""
echo "#endif"
