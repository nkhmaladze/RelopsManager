#!/usr/bin/bash

source ./config.sh

# mkdir for your DB files and create the DB
mkdir -p $MYDBDIR
echo "Creating DB...."
echo "   $SWATDBDIR/bin/createrelation $SWATDBDIR/include/names.txt $MYDBDIR/ $MYDBDIR/tables.db"
$SWATDBDIR/bin/createrelation $SWATDBDIR/include/names.txt $MYDBDIR/ $MYDBDIR/tables.db
echo "Creating small DB...."
echo "   $SWATDBDIR/bin/createsmall $MYDBDIR/ $MYDBDIR/small.db"
$SWATDBDIR/bin/createsmall $MYDBDIR/ $MYDBDIR/small.db
ls -l $MYDBDIR
