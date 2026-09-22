//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/value.h>
#include <opendmi/utils/name.h>

/**
 * @brief Names of measurement units.
 */
static const dmi_name_set_t dmi_unit_names =
{
    .code  = "unit",
    .name  = "Measurement units",
    .names = DMI_NAMES({
        { DMI_UNIT_HOUR,           "hour",           "h"     },
        { DMI_UNIT_MINUTE,         "minute",         "m"     },
        { DMI_UNIT_SECOND,         "second",         "s"     },
        { DMI_UNIT_MILLISECOND,    "millisecond",    "ms"    },
        { DMI_UNIT_MICROSECOND,    "microsecond",    "us"    },
        { DMI_UNIT_NANOSECOND,     "nanosecond",     "ns"    },

        { DMI_UNIT_AMPERE,         "ampere",         "A"     },
        { DMI_UNIT_MILLIAMPERE,    "milliampere",    "mA"    },
        { DMI_UNIT_VOLT,           "volt",           "V"     },
        { DMI_UNIT_MILLIVOLT,      "millivolt",      "mV"    },
        { DMI_UNIT_WATT,           "watt",           "W"     },
        { DMI_UNIT_MILLIWATT,      "milliwatt",      "mW"    },
        { DMI_UNIT_WATT_HOUR,      "watt-hour",      "W*h"   },

        { DMI_UNIT_CELSIUS,        "celsius",        "°C"    },

        { DMI_UNIT_BIT,            "bit",            "bits"  },
        { DMI_UNIT_BYTE,           "byte",           "bytes" },

        { DMI_UNIT_KILOBYTE,       "kilobyte",       "KB"    },
        { DMI_UNIT_MEGABYTE,       "megabyte",       "MB"    },
        { DMI_UNIT_GIGABYTE,       "gigabyte",       "GB"    },

        { DMI_UNIT_KIBIBYTE,       "kibibyte",       "KiB"   },
        { DMI_UNIT_MEBIBYTE,       "mebibyte",       "MiB"   },
        { DMI_UNIT_GIBIBYTE,       "gibibyte",       "GiB"   },
        { DMI_UNIT_TEBIBYTE,       "tebibyte",       "TiB"   },
        { DMI_UNIT_PEBIBYTE,       "pebibyte",       "PiB"   },
        { DMI_UNIT_EXBIBYTE,       "exbibyte",       "EiB"   },
        { DMI_UNIT_ZEBIBYTE,       "zebibyte",       "ZiB"   },
        { DMI_UNIT_YOBIBYTE,       "yobibyte",       "YiB"   },
        { DMI_UNIT_ROBIBYTE,       "robibyte",       "RiB"   },
        { DMI_UNIT_QUEBIBYTE,      "quebibyte",      "QiB"   },

        { DMI_UNIT_BAUD,           "baud",           "baud"  },
        { DMI_UNIT_MEGAXA_SECOND,  "megaxa-second",  "MT/s"  },
        { DMI_UNIT_KILOBIT_SECOND, "kilobit-second", "Kbps"  },
        { DMI_UNIT_MEGABIT_SECOND, "megabit-second", "Mbps"  },
        { DMI_UNIT_GIGABIT_SECOND, "gigabit-second", "Gbps"  },

        { DMI_UNIT_PERCENT,        "percent",        "%"     },
        { DMI_UNIT_REVOLUTION,     "revolution",     "rpm"   },
        { DMI_UNIT_RACK,           "rack",           "U"     },
        { DMI_UNIT_MHZ,            "mhz",            "MHz"   },

        { DMI_UNIT_MILLIMETER,     "millimeter",     "mm"    },
        {}
    })
};

/**
 * @brief Names of boolean values.
 */
static const dmi_name_set_t dmi_bool_names =
{
    .code  = "bool",
    .name  = "Boolean values",
    .names = DMI_NAMES({
        { false, "false", "no"  },
        { true,  "true",  "yes" },
        {}
    })
};

const char *dmi_unit_code(dmi_unit_t value)
{
    return dmi_code_lookup(&dmi_unit_names, (int)value);
}

const char *dmi_unit_name(dmi_unit_t value)
{
    return dmi_name_lookup(&dmi_unit_names, (int)value);
}

const char *dmi_bool_code(bool value)
{
    return dmi_code_lookup(&dmi_bool_names, !!value);
}

const char *dmi_bool_name(bool value)
{
    return dmi_name_lookup(&dmi_bool_names, !!value);
}
