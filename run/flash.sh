#!/bin/env bash
set -e

usage() {
  echo "Usage: $0 -m <main_file>"
  exit 1
}

# Parse arguments
while getopts ":m:" opt; do
  case ${opt} in
    m )
      LF_MAIN=$OPTARG
      ;;
    \? )
      echo "Invalid option: -$OPTARG" 1>&2
      usage
      ;;
    : )
      echo "Invalid option: -$OPTARG requires an argument" 1>&2
      usage
      ;;
  esac
done
shift $((OPTIND -1))

# Check if LF_MAIN is set
if [ -z "${LF_MAIN}" ]; then
  usage
fi

LF_MAIN_NAME=$(basename ${LF_MAIN} .lf)

# Run picotool with the specified main file
FLASH_COMMAND="picotool flash -x build/${LF_MAIN_NAME}.elf"
echo "Running picotool flash command: ${FLASH_COMMAND}"
${FLASH_COMMAND}