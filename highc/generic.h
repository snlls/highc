#pragma once

#define M_IDENTITY(x) x
#define M_STREXPAND(x) #x
#define M_STR(x) M_STREXPAND(x)
#define M_CONC(x, y) M_STREXPAND(x) ## M_STREXPAND(y)
