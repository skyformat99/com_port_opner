// @file main.cc
// @brief COM port opner.
// @author Mamoru Kaminaga
// @date 2017-11-04
// Copyright 2017 Mamoru Kaminaga
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <windows.h>

bool OpenComPort(int to_open_port_num) {
  to_open_port_num -= 1;
  if ((to_open_port_num < 0) || (to_open_port_num > 127)) return false;

  // COM port is opened.
  HKEY hkey = nullptr;
  DWORD dwdisposition;

  LONG result = RegCreateKeyEx(
      HKEY_LOCAL_MACHINE,
      L"SYSTEM\\CurrentControlSet\\Control\\COM Name Arbiter",
      0,
      nullptr,
      REG_OPTION_NON_VOLATILE,
      KEY_ALL_ACCESS,
      nullptr,
      &hkey,
      &dwdisposition);
  if (result != ERROR_SUCCESS) {
    return false;
  }

  // COM port is read.
  DWORD old_value[4] = {0};
  DWORD size = static_cast<DWORD>(sizeof(old_value));
  DWORD type = 0;

  result = RegQueryValueEx(
      hkey,
      L"ComDB",
      nullptr,
      &type,
      (LPBYTE) &old_value,
      &size);
  if (result != ERROR_SUCCESS) {
    RegCloseKey(hkey);
    return false;
  }

  // COP port flag is set.
  DWORD new_value[4] = {0};
  memcpy(new_value, old_value, sizeof(old_value));

  int change_index = to_open_port_num / 32;
  int bit_shift_num = to_open_port_num % 32;
  DWORD bit_mask = ~(0x01 << bit_shift_num);
  new_value[change_index] = (old_value[change_index]) & bit_mask;

  result = RegSetValueEx(
      hkey,
      L"ComDB",
      0,
      REG_DWORD,
      (CONST BYTE*) &new_value,
      static_cast<DWORD>(sizeof(new_value)));
  if (result != ERROR_SUCCESS) {
    RegCloseKey(hkey);
    return false;
  }

  // Info display.
  wprintf(L"old value: 0x%02x%02x%02x%02x\n",
      static_cast<int>(old_value[3]),
      static_cast<int>(old_value[2]),
      static_cast<int>(old_value[1]),
      static_cast<int>(old_value[0]));
  wprintf(L"new value: 0x%02x%02x%02x%02x\n",
      static_cast<int>(new_value[3]),
      static_cast<int>(new_value[2]),
      static_cast<int>(new_value[1]),
      static_cast<int>(new_value[0]));

  // COM port is closed.
  RegCloseKey(hkey);

  return true;
}

int main(int argc, char* argv[]) {
  int to_open_port_num = 0;
  wprintf(L"Which COM port to open? (1 - 128)\n");
  wprintf(L">");
  wscanf_s(L"%d", &to_open_port_num);
  fflush(stdin);

  if (!OpenComPort(to_open_port_num)) {
    wprintf(L"Failed to open COM%d\n", to_open_port_num);
    system("pause");
    return -1;
  }

  wprintf(L"Opened COM%d\n", to_open_port_num);
  system("pause");
  return 0;
}
