#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <bitset>
#include "stdlib.h"
#include "MyMessage_m.h"
using namespace std;

enum {
    Data,
    ACK,
    NACK,
    TimeOut,
    coordinator,
    DoneProcessing,
    WindowAdvanced,
    AckProcessed,
    NackProcessed
};
