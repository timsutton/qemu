/*
 * QEMU simulated pvpanic device (MMIO frontend)
 *
 * Copyright © 2023 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 */

#include "qemu/osdep.h"
#include "qemu/module.h"
#include "sysemu/runstate.h"

#include "hw/nvram/fw_cfg.h"
#include "hw/qdev-properties.h"
#include "hw/misc/pvpanic.h"
#include "qom/object.h"
#include "hw/isa/isa.h"
#include "standard-headers/linux/pvpanic.h"

OBJECT_DECLARE_SIMPLE_TYPE(PVPanicMMIOState, PVPANIC_MMIO_DEVICE)

#define PVPANIC_MMIO_SIZE 0x2

struct PVPanicMMIOState {
    SysBusDevice parent_obj;

    PVPanicState pvpanic;
};

static void pvpanic_mmio_initfn(Object *obj)
{
    PVPanicMMIOState *s = PVPANIC_MMIO_DEVICE(obj);

    pvpanic_setup_io(&s->pvpanic, DEVICE(s), PVPANIC_MMIO_SIZE);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->pvpanic.mr);
}

static Property pvpanic_mmio_properties[] = {
    DEFINE_PROP_UINT8("events", PVPanicMMIOState, pvpanic.events,
                      PVPANIC_PANICKED | PVPANIC_CRASH_LOADED),
    DEFINE_PROP_END_OF_LIST(),
};

static void pvpanic_mmio_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    device_class_set_props(dc, pvpanic_mmio_properties);
    set_bit(DEVICE_CATEGORY_MISC, dc->categories);
}

static const TypeInfo pvpanic_mmio_info = {
    .name          = TYPE_PVPANIC_MMIO_DEVICE,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(PVPanicMMIOState),
    .instance_init = pvpanic_mmio_initfn,
    .class_init    = pvpanic_mmio_class_init,
};

static void pvpanic_register_types(void)
{
    type_register_static(&pvpanic_mmio_info);
}

type_init(pvpanic_register_types)