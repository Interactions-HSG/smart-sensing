# THIS FILE IS AUTOMATICALLY GENERATED
# Project: C:\Program Files (x86)\Cypress\CYALKIT-E02 Solar-Powered BLE Sensor Beacon RDK\1.0\Firmware\BLE\Solar_BLE_Sensor\Solar_BLE_Sensor.cydsn\Solar_BLE_Sensor.cyprj
# Date: Wed, 12 Jun 2024 14:43:02 GMT
#set_units -time ns
create_clock -name {UART_SCBCLK(FFB)} -period 708.33333333333326 -waveform {0 354.166666666667} [list [get_pins {ClockBlock/ff_div_1}]]
create_clock -name {I2CM_SCBCLK(FFB)} -period 125 -waveform {0 62.5} [list [get_pins {ClockBlock/ff_div_2}]]
create_clock -name {CyRouted1} -period 41.666666666666664 -waveform {0 20.8333333333333} [list [get_pins {ClockBlock/dsi_in_0}]]
create_clock -name {CyILO} -period 31250 -waveform {0 15625} [list [get_pins {ClockBlock/ilo}]]
create_clock -name {CyLFCLK} -period 31250 -waveform {0 15625} [list [get_pins {ClockBlock/lfclk}]]
create_clock -name {CyECO} -period 41.666666666666664 -waveform {0 20.8333333333333} [list [get_pins {ClockBlock/eco}]]
create_clock -name {CyIMO} -period 41.666666666666664 -waveform {0 20.8333333333333} [list [get_pins {ClockBlock/imo}]]
create_clock -name {CyHFCLK} -period 41.666666666666664 -waveform {0 20.8333333333333} [list [get_pins {ClockBlock/hfclk}]]
create_clock -name {CySYSCLK} -period 41.666666666666664 -waveform {0 20.8333333333333} [list [get_pins {ClockBlock/sysclk}]]
create_generated_clock -name {UART_SCBCLK} -source [get_pins {ClockBlock/hfclk}] -edges {1 17 35} -nominal_period 708.33333333333326 [list]
create_generated_clock -name {I2CM_SCBCLK} -source [get_pins {ClockBlock/hfclk}] -edges {1 3 7} [list]


# Component constraints for C:\Program Files (x86)\Cypress\CYALKIT-E02 Solar-Powered BLE Sensor Beacon RDK\1.0\Firmware\BLE\Solar_BLE_Sensor\Solar_BLE_Sensor.cydsn\TopDesign\TopDesign.cysch
# Project: C:\Program Files (x86)\Cypress\CYALKIT-E02 Solar-Powered BLE Sensor Beacon RDK\1.0\Firmware\BLE\Solar_BLE_Sensor\Solar_BLE_Sensor.cydsn\Solar_BLE_Sensor.cyprj
# Date: Wed, 12 Jun 2024 14:42:58 GMT
