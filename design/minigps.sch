EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:minigps
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text Notes 8200 7650 0    60   ~ 0
19/08/2016
Wire Wire Line
	6550 4200 5350 4200
Wire Wire Line
	5350 4200 5350 5400
Wire Wire Line
	5350 5400 4700 5400
Wire Wire Line
	6550 4100 5450 4100
Wire Wire Line
	5450 4100 5450 5500
Wire Wire Line
	5450 5500 4700 5500
Wire Wire Line
	4700 5300 8300 5300
Wire Wire Line
	8300 5300 8300 4400
Wire Wire Line
	8300 4400 7650 4400
Wire Wire Line
	6550 4500 5050 4500
Wire Wire Line
	5050 4050 5050 4500
Wire Wire Line
	5050 4500 5050 5000
Wire Wire Line
	5050 5000 4700 5000
Wire Wire Line
	6550 4600 4950 4600
Wire Wire Line
	4950 3950 4950 4600
Wire Wire Line
	4950 4600 4950 4900
Wire Wire Line
	4950 4900 4700 4900
Wire Wire Line
	6550 4800 4850 4800
Wire Wire Line
	4850 4800 4850 4800
Wire Wire Line
	4850 4800 4700 4800
Wire Wire Line
	6550 4400 5150 4400
Wire Wire Line
	5150 4400 5150 5100
Wire Wire Line
	5150 5100 4700 5100
Wire Wire Line
	6550 4300 5250 4300
Wire Wire Line
	5250 4300 5250 5200
Wire Wire Line
	5250 5200 4700 5200
Wire Wire Line
	4700 3350 8300 3350
Wire Wire Line
	8300 3350 8300 4300
Wire Wire Line
	8300 4300 7650 4300
Wire Wire Line
	4700 4050 5050 4050
Connection ~ 5050 4500
Wire Wire Line
	4700 3950 4950 3950
Connection ~ 4950 4600
Wire Wire Line
	6450 4450 5550 4450
Wire Wire Line
	5550 4450 5550 3850
Wire Wire Line
	5550 3850 4700 3850
Wire Wire Line
	6450 4350 5650 4350
Wire Wire Line
	5650 4350 5650 3750
Wire Wire Line
	5650 3750 4700 3750
Text Notes 7400 7500 0    60   ~ 0
minigps
Text Notes 10600 7650 0    60   ~ 0
0.1
$Comp
L Arduino_Pro_Mini_8Mhz APM?
U 1 1 57B71DA7
P 7150 4600
F 0 "APM?" H 6300 4700 39  0000 C CNN
F 1 "Arduino_Pro_Mini_8Mhz" H 6550 4600 39  0000 C CNN
F 2 "" H 7150 4600 60  0000 C CNN
F 3 "" H 7150 4600 60  0000 C CNN
	1    7150 4600
	0    1    1    0   
$EndComp
$Comp
L MPU6050 MPU?
U 1 1 57B71FB9
P 4350 3650
F 0 "MPU?" H 4650 3400 60  0000 C CNN
F 1 "MPU6050" H 4550 3500 60  0000 C CNN
F 2 "" H 4350 3650 60  0000 C CNN
F 3 "" H 4350 3650 60  0000 C CNN
	1    4350 3650
	0    1    1    0   
$EndComp
$Comp
L Lonet808 SIM808?
U 1 1 57B72040
P 4250 5450
F 0 "SIM808?" H 4500 5400 60  0000 C CNN
F 1 "Lonet808" H 4500 5550 60  0000 C CNN
F 2 "" H 4250 5450 60  0000 C CNN
F 3 "" H 4250 5450 60  0000 C CNN
	1    4250 5450
	0    -1   -1   0   
$EndComp
$EndSCHEMATC
