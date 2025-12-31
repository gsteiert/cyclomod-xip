/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 * Copyringt (c) 2025 Greg Steiert
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "pico/bootrom.h"
#include "hardware/structs/qmi.h"
#include "hardware/structs/xip_ctrl.h"
#include "hardware/clocks.h"
#include "tusb.h"

#include "jbipico.h"

//#define WAIT_FOR_CONSOLE
#define RUN_DEFAULT_ACTION

#define COMMAND_BUFFER_LENGTH  127

// 0x1C000000 is uncached, untranslated CS0
// 0x1D000000 is uncached, untranslated CS1
#define XIP_BASE_ADDRESS   0x1D000000

#define MEM_TEST_RANGE     32768

jbc_uf2_hdr_t *juf2 = (jbc_uf2_hdr_t *)JUF2_ADDRESS;
const char *delimiters = ", \t";
int qmi_speed = 0;

int nextInt() {
  char * nptr = strtok(NULL, delimiters);
  if (nptr == NULL) {
    return -1;
  } else {
    char * endptr = NULL;
    unsigned long ul = strtoul(nptr, &endptr, 0);
    if (nptr == endptr) {
      return -1;
    } else {
      return ul;
    }
  }
}

void xip_init() {
  gpio_set_function(0, GPIO_FUNC_XIP_CS1); // CS for FPGA XIP
  xip_ctrl_hw->ctrl|=XIP_CTRL_WRITABLE_M1_BITS;
  qmi_speed = 75;
  qmi_hw->m[1].timing = ( 0x1   << QMI_M1_TIMING_COOLDOWN_LSB
                        | 0x0   << QMI_M1_TIMING_PAGEBREAK_LSB
                        | 0x1   << QMI_M1_TIMING_SELECT_SETUP_LSB
                        | 0x0   << QMI_M1_TIMING_SELECT_HOLD_LSB
                        | 0x0   << QMI_M1_TIMING_MAX_SELECT_LSB
                        | 0x0   << QMI_M1_TIMING_MIN_DESELECT_LSB
                        | 0x0   << QMI_M1_TIMING_RXDELAY_LSB
                        | 0x2   << QMI_M1_TIMING_CLKDIV_LSB
  );
  qmi_hw->m[1].rfmt = ( 0                                  << QMI_M1_RFMT_DTR_LSB
                      | QMI_M1_RFMT_DUMMY_LEN_VALUE_8      << QMI_M1_RFMT_DUMMY_LEN_LSB
                      | QMI_M1_RFMT_SUFFIX_LEN_VALUE_NONE  << QMI_M1_RFMT_SUFFIX_LEN_LSB 
                      | QMI_M1_RFMT_PREFIX_LEN_VALUE_8     << QMI_M1_RFMT_PREFIX_LEN_LSB 
                      | QMI_M1_RFMT_DATA_WIDTH_VALUE_Q     << QMI_M1_RFMT_DATA_WIDTH_LSB 
                      | QMI_M1_RFMT_DUMMY_WIDTH_VALUE_Q    << QMI_M1_RFMT_DUMMY_WIDTH_LSB 
                      | QMI_M1_RFMT_SUFFIX_WIDTH_VALUE_Q   << QMI_M1_RFMT_SUFFIX_WIDTH_LSB 
                      | QMI_M1_RFMT_ADDR_WIDTH_VALUE_Q     << QMI_M1_RFMT_ADDR_WIDTH_LSB 
                      | QMI_M1_RFMT_PREFIX_WIDTH_VALUE_Q   << QMI_M1_RFMT_PREFIX_WIDTH_LSB
  );
  qmi_hw->m[1].rcmd = 0x03; // use default 0x03 cmd
  qmi_hw->m[1].wfmt = ( 0                                  << QMI_M1_WFMT_DTR_LSB 
                      | QMI_M1_WFMT_DUMMY_LEN_VALUE_NONE   << QMI_M1_WFMT_DUMMY_LEN_LSB 
                      | QMI_M1_WFMT_SUFFIX_LEN_VALUE_NONE  << QMI_M1_WFMT_SUFFIX_LEN_LSB 
                      |	QMI_M1_WFMT_PREFIX_LEN_VALUE_8     << QMI_M1_WFMT_PREFIX_LEN_LSB 
                      |	QMI_M1_WFMT_DATA_WIDTH_VALUE_Q     << QMI_M1_WFMT_DATA_WIDTH_LSB 
                      |	QMI_M1_WFMT_DUMMY_WIDTH_VALUE_Q    << QMI_M1_WFMT_DUMMY_WIDTH_LSB 
                      |	QMI_M1_WFMT_SUFFIX_WIDTH_VALUE_Q   << QMI_M1_WFMT_SUFFIX_WIDTH_LSB 
                      |	QMI_M1_WFMT_ADDR_WIDTH_VALUE_Q     << QMI_M1_WFMT_ADDR_WIDTH_LSB 
                      |	QMI_M1_WFMT_PREFIX_WIDTH_VALUE_Q   << QMI_M1_WFMT_PREFIX_WIDTH_LSB
  );
  qmi_hw->m[1].wcmd = 0x02; // use default 0x02 cmd
  
}

void xip_read() {
  int addr = nextInt();
  int cnt = nextInt();
  if ((addr >= 0) & (cnt >= 0)) {
    volatile uint8_t *xip_ptr = (volatile uint8_t *)(XIP_BASE_ADDRESS + addr);
    printf("\nRead %d bytes from 0x%08X\n", cnt, addr);
    while (cnt > 0) {
      printf("0x%02X\n", *xip_ptr);
      *xip_ptr++;
      cnt--;
    }
  } else {
    printf("\n! Bad read command\n");
  }
}

void xip_write() {
  int addr = nextInt();
  int cnt = 0;
  if (addr >= 0) {
    volatile uint8_t *xip_ptr = (volatile uint8_t *)(XIP_BASE_ADDRESS + addr);
    int wdata = nextInt();
    printf("\nWriting to addr 0x%08X\n", addr);
    while (wdata >=0) {
      *xip_ptr = wdata;
      printf("0x%02X\n", wdata);
      *xip_ptr++;
      wdata = nextInt();
      cnt++;
    }
    printf("Wrote %d bytes\n", cnt);
  } else {
    printf("\n! Bad write address\n");
  }
}

void mem_test() {
  int cycles = nextInt();
  int errors = 0;
  volatile uint8_t *xip_ptr = (volatile uint8_t *)(XIP_BASE_ADDRESS);
  uint8_t prbs;
  printf("QMI speed:  %d MHz\n", qmi_speed);
  printf("Testing %d cycles of %d addresses\n", cycles, MEM_TEST_RANGE);
  while (cycles > 0) {
    prbs = (cycles % 127) +1;
    int i;
    for (i=0; i<MEM_TEST_RANGE; i++) {
      prbs = (prbs << 1) | (((prbs >> 6) ^ (prbs >> 5)) & 1);
      xip_ptr[i] = prbs;
    }
    prbs = (cycles % 127) +1;
    for (i=0; i<MEM_TEST_RANGE; i++) {
      prbs = (prbs << 1) | (((prbs >> 6) ^ (prbs >> 5)) & 1);
      if (xip_ptr[i] != prbs) {
        errors += 1;
        printf("! Failed at address 0x%08X !\n", (i + XIP_BASE_ADDRESS));
      }
    }
    cycles -= 1;
  }
  printf("Test completed with %d errors\n", errors);
}

void led_ramp() {
  int cycles = nextInt();
  int ramp;
  volatile uint8_t *led_ptr = (volatile uint8_t *)(XIP_BASE_ADDRESS +0x08);
  printf("Ramping LED %d cycles\n", cycles);
  while (cycles > 0) {
    for (ramp = 0; ramp < 256; ramp++) {
      *led_ptr = ramp;
      sleep_ms(2);
    }
    for (ramp = 0; ramp < 256; ramp++) {
      *led_ptr = 255 -ramp;
      sleep_ms(2);
    }
    cycles -= 1;
  }
}

void process_command(char *buf) {
  char *command = strtok(buf, delimiters);
  switch (command[0]) {
    case '?':
    case 'H':
    case 'h':
      printf("JBC Player Help\n");
      break;
    case 'R':
    case 'r':
      xip_read();
      break;
    case 'W':
    case 'w':
      xip_write();
      break;
    case 'L':
    case 'l':
      printf("LED Ramp\n");
      led_ramp();
      break;
    case 'M':
    case 'm':
      printf("Memory Test\n");
      mem_test();
      break;
    case 'D':
    case 'd':
      printf("Display details\n");
      jbi_play(juf2, NULL);
      break;
    case 'I':
    case 'i':
      printf("Check IDCODE\n");
      jbi_play(juf2, "CHECK_IDCODE");
      break;
    case 'C':
    case 'c':
      printf("Configure\n");
      jbi_play(juf2, "CONFIGURE");
      break;
    case 'P':
    case 'p':
      printf("Information\n");
      jbi_play(juf2, "PROGRAM");
      break;
    case 'U':
    case 'u':
      printf("Read Usercode\n");
      jbi_play(juf2, "READ_USERCODE");
      break;
    case 'V':
    case 'v':
      printf("Verify\n");
      jbi_play(juf2, "VERIFY");
      break;
    case 'E':
    case 'e':
      printf("Erase\n");
      jbi_play(juf2, "ERASE");
      break;
    case 'A':
    case 'a':
      printf("Default Action\n");
      jbi_play(juf2, juf2->action);
      break;
    case 'B':
    case 'b':
      printf("Reseting to bootloader\n");
	    reset_usb_boot(1<<PIN_LED, 0);
      break;
    default:
      printf("Unknown command:  %s\n", command);
  }
#ifdef CYCLOMOD
  printf("\nCycloMod");
#endif
  printf("\nXIP Commands:\n");
  printf("  R/r Read:         read XIP address\n");
  printf("  W/w Write:        write XIP address\n");
  printf("  M/m Memory Test:  test XIP memory\n");
  printf("  A/a Action:       perform default action\n");
  printf("  B/b Bootloader:   reset to bootloader\n");
  printf("  C/c Configure:    configure device\n");
  printf("  D/d Details:      display JBC details\n");
  printf("  E/e Erase:        erase device\n");
  printf("  H/h Help:         print command help\n");
  printf("  I/i IDCODE:       get device IDCODE\n");
  printf("  P/p Program:      program device\n");
  printf("  U/u Usercode:     read USERCODE\n");
  printf("  V/v Verify:       verify device\n");
}

int main() {
    bi_decl(bi_program_description("This is a port of the Altera JBC Player."));

    stdio_init_all();
    jbi_init(TRUE);
    xip_init();

#ifdef CYCLOMOD
    clock_gpio_init(PIN_CLKOUT, CLOCKS_CLK_GPOUT0_CTRL_AUXSRC_VALUE_XOSC_CLKSRC, 1);
#endif

#ifdef WAIT_FOR_CONSOLE
  // wait for something on STDIN before scanning to make sure the terminal is open
    while (!tud_cdc_connected()) {
      sleep_ms(50);
    } 
#endif

    printf("\nJAM Byte Code Player\n");
    printf("JUF2 header address:  0x%08X\n", (uint32_t)juf2);
	  printf(" Tag: 0x%08X\n Ver: 0x%08X\n Len: 0x%08X\n Adr: 0x%08X\n", juf2->tag, juf2->version, juf2->length, juf2->location);

#ifdef RUN_DEFAULT_ACTION
    jbi_play(juf2, juf2->action);
#else
    jbi_play(juf2, NULL);
#endif

    printf("\nPins:  TCK %d, TMS, %d, TDI %d, TDO %d, LED %d\n", PIN_TCK, PIN_TMS, PIN_TDI, PIN_TDO, PIN_LED);

    char cmd[COMMAND_BUFFER_LENGTH +1];
    cmd[0] = 0;
    process_command(cmd);

    while (1) {
        gets(cmd);
        printf("cmd:  %s\n", cmd);
        process_command(cmd);
        sleep_ms(1);
    }

}
