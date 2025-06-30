#!/usr/bin/env bash
except=$1
if [ "$except" = "FE_UNDERFLOW" ]; then
  signal_code_option1=FPE_FLTUND
  signal_code_option2=UFF
elif [ "$except" = "FE_OVERFLOW" ]; then
  signal_code_option1=FPE_FLTOVF
  signal_code_option2=OFF
elif [ "$except" = "FE_INEXACT" ]; then
  signal_code_option1=FPE_FLTRES
  signal_code_option2=IXF
elif [ "$except" = "FE_INVALID" ]; then
  signal_code_option1=FPE_FLTINV
  signal_code_option2=IOF
elif [ "$except" = "FE_DIVBYZERO" ]; then
  signal_code_option1=FPE_FLTDIV
  signal_code_option2=DZF
else
  signal_code_option1=undefined
  signal_code_option2=undefined
fi

set -x

output=floating_point_exception_$1.log
./floating_point_exception $except >$output 2>&1

set +x

echo "Test output:"
cat $output

if [[ $(grep "${signal_code_option1}" $output) ]] ; then
   echo "SUCCESS: Detected signal code ${signal_code_option1}"
   exit 0
fi
if [[ $(grep "${signal_code_option2}" $output) ]] ; then
   echo "SUCCESS: Detected signal code ${signal_code_option2}"
   exit 0
fi

if [[ $(grep "Could not trap ${except}" $output) ]] ; then
    echo "WARNING: Could not trap ${except}, but not treating as failure."
    exit 0
fi


# No signal code was detected in the output
echo "FAILURE: signal code ${signal_code_option1} or ${signal_code_option2} were not detected in output" 
exit 1
