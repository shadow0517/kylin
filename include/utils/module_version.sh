#!/bin/sh

module=$1

MODULE=$(echo $module | tr '[:lower:]' '[:upper:]')

echo "#ifndef _KYLIN_${MODULE}_VERSION_H_"             >  kylin_${module}_version.h
echo "#define _KYLIN_${MODULE}_VERSION_H_"             >> kylin_${module}_version.h
echo "extern char kylin_${module}_build_date[];"       >> kylin_${module}_version.h
echo "#endif /*_KYLIN_${MODULE}_VERSION_H_*/"          >> kylin_${module}_version.h

echo "#include <${module}/kylin_${module}_version.h>"  >  kylin_${module}_version.c
echo "char kylin_${module}_build_date[] = \"`date`\";" >> kylin_${module}_version.c
