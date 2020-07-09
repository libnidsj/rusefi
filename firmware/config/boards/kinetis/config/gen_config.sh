#!/bin/bash
# This batch files reads rusefi_config.txt and produses firmware persistent configuration headers
# the storage section of rusefi.ini is updated as well

cd ../../../..

pwd

bash gen_signature.sh kin

java \
 -DSystemOut.name=gen_config_kinetis_board \
 -cp ../java_tools/ConfigDefinition.jar \
 com.rusefi.board_generator.BoardReader \
 -yaml config/boards/kinetis/mapping.yaml \
 -firmware_path . \
 -output_file tunerstudio/generated/kinetis_prefix.txt \
 -enumInputFile controllers/algo/rusefi_enums.h \
 -enumInputFile config/boards/kinetis/rusefi_hw_enums.h

java \
 -DSystemOut.name=gen_config_kinetis \
 -Drusefi.generator.lazyfile.enabled=true \
 -jar ../java_tools/ConfigDefinition.jar \
 -definition integration/rusefi_config.txt \
 -cache kinetis \
 -cache_zip_file tunerstudio/generated/cache.zip \
 -ts_destination tunerstudio \
 -tool kinetis_gen_config.bat \
 -firing_order controllers/algo/firing_order.h \
 -with_c_defines false \
 -initialize_to_zero false \
 -ts_output_name generated/rusefi_kinetis.ini \
 -c_defines config/boards/kinetis/config/controllers/algo/rusefi_generated.h \
 -c_destination config/boards/kinetis/config/controllers/algo/engine_configuration_generated_structures.h \
 -signature tunerstudio/generated/signature_kin.txt \
 -signature_destination controllers/generated/signature_kin.h \
 -prepend config/boards/kinetis/config/rusefi_config_kinetis.txt \
 -prepend config/boards/kinetis/config/tunerstudio/generated/kinetis_prefix.txt

