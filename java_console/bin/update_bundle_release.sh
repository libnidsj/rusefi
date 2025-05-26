#!/bin/bash

cd console
java -jar evotech_autoupdate.jar release
echo Release update done.

# https://github.com/rusefi/rusefi/issues/2601
chmod +x ../bin/*.sh
